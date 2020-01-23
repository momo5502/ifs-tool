#include "std_include.hpp"
#include "logger.hpp"
#include <conio.h>
#include "ifs.hpp"
#include <array>

#if __has_include(<zlib.h>) && __has_include(<tomcrypt.h>)
#include "tomcrypt.h"
#include <zlib.h>

constexpr std::array<uint32_t, 256> Cryptotable()
{
	constexpr uint32_t Polynomial = 0xEDB88320;
	std::array<uint32_t, 256> Table{};

	for (uint16_t i = 0; i <= 0xFF; ++i)
	{
		uint32_t Remainder{ i };

		for (uint8_t b = 8; !!b; --b)
		{
			if (!(Remainder & 1)) Remainder >>= 1;
			else Remainder = (Remainder >> 1) ^ Polynomial;
		}

		Table[i] = Remainder;
	}

	return Table;
}
bool PokeIWI(std::string_view Filename)
{
	auto Decrypt = [](const void *Input, const size_t Length, const void *Key, const void *Initialvector)
	{
		static bool doOnce{ []() { register_cipher(&aes_desc); return false; }() };

		symmetric_CTR Context{};
		const auto Buffer = std::make_unique<uint8_t[]>(Length + 32);

		ctr_start(find_cipher("aes"), (uint8_t *)Initialvector, (uint8_t *)Key, 24, 0, 1, &Context);
		ctr_setiv((uint8_t *)Initialvector, 16, &Context);

		ctr_decrypt((uint8_t *)Input, Buffer.get(), Length, &Context);
		ctr_done(&Context);

		return std::string((char *)Buffer.get(), Length);
	};
	auto Writefile = [](const std::string_view Path, const std::string &&Buffer)
	{
		std::FILE *Filehandle = std::fopen(Path.data(), "wb");
		if (!Filehandle) return false;

		std::fwrite(Buffer.c_str(), Buffer.size(), 1, Filehandle);
		std::fclose(Filehandle);
		return true;
	};
	auto Decompress = [](const std::string &&Buffer)
	{
		z_stream Stream{};
		std::string Output{};
		if (inflateInit2(&Stream, 15)) return Output;

		Stream.next_in = (uint8_t *)Buffer.data();
		Stream.avail_in = Buffer.size();

		auto Decompressbuffer = std::make_unique<uint8_t[]>(4096);
		do
		{
			Stream.next_out = Decompressbuffer.get();
			Stream.avail_out = 4096;

			const auto Result = inflate(&Stream, Z_NO_FLUSH);
			switch (Result)
			{
				case Z_NEED_DICT:
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					inflateEnd(&Stream);
					return std::string();
			}

			const auto Available = 4096 - Stream.avail_out;
			Output.append((char *)Decompressbuffer.get(), Available);

		} while (Stream.avail_out == 0);

		inflateEnd(&Stream);
		return Output;
	};
	auto Readfile = [](const std::string_view Path)
	{
		std::FILE *Filehandle = std::fopen(Path.data(), "rb");
		if (!Filehandle) return std::basic_string<uint8_t>();

		std::fseek(Filehandle, 0, SEEK_END);
		const auto Length = std::ftell(Filehandle);
		std::fseek(Filehandle, 0, SEEK_SET);

		const auto Buffer = std::make_unique<uint8_t[]>(Length);
		std::fread(Buffer.get(), Length, 1, Filehandle);
		std::fclose(Filehandle);

		return std::basic_string<uint8_t>(Buffer.get(), Length);
	};
	auto CreateIV = [](std::string_view Filename)
	{
		if (Filename.find("/") != std::string_view::npos)
		{
			Filename.remove_prefix(Filename.find_last_of("/") + 1);
		}
		if (Filename.find("\\") != std::string_view::npos)
		{
			Filename.remove_prefix(Filename.find_last_of("\\") + 1);
		}
		const uint32_t Length = std::strlen(Filename.data());

		uint32_t Block = ~Length;
		for (int i = 0; i < Length; ++i)
		{
			Block = Cryptotable()[(Block ^ Filename[i]) & 0xFF] ^ (Block >> 8);
		}

		return ~Block;
	};
	constexpr uint8_t Cryptokey[24] = {
		0x15, 0x9A, 0x03, 0x25, 0xE0, 0x75, 0x2E, 0x80,
		0xC6, 0xC0, 0x94, 0x2A, 0x50, 0x5C, 0x1C, 0x68,
		0x8C, 0x17, 0xEF, 0x53, 0x99, 0xF8, 0x68, 0x3C
	};

	uint32_t IV[4]{};
	std::string Decrypted{};
	IV[0] = CreateIV(Filename);
	auto Filebuffer = Readfile(Filename);
	IV[1] = *(uint32_t *)(Filebuffer.data() + Filebuffer.size() - 4);

	// Chunked buffer, because reasons.
	for (int i = 0; i < Filebuffer.size() / 0x8000 + 1; ++i)
	{
		IV[2] = i * 0x8000;	// Bytes written.
		IV[3] = std::min(0x8000U, Filebuffer.size() - 4 - IV[2]);
		Decrypted += Decrypt(Filebuffer.data() + IV[2], IV[3], Cryptokey, IV);
	}

	// Overwrite the original file.
	if (!Decrypted.empty())
	{
		auto Decompressed = Decompress(std::move(Decrypted));	// NOTE(tcn): "IWi" is not my typo.
		if (!Decompressed.empty() && Decompressed[0] == 'I' && Decompressed[1] == 'W' && Decompressed[2] == 'i')
			return Writefile(Filename, std::move(Decompressed));
	}

	return false;
}
#else
#pragma message ("Warning: Add Libtom and ZLIB to work with IWI files.")
bool PokeIWI(std::string_view) { return true; }
#endif

int main(const int argc, char* argv[])
{
	const logger logger;

	try
	{
		logger.header("IFS Tool by momo5502");

		for (auto i = 1; i < argc; ++i)
		{
			std::filesystem::path archive(argv[i]);
			logger.info("Handling archive %s...\n", archive.filename().string().data());

			const ifs ifs(argv[i]);
			const auto files = ifs.get_file_list();

			if(files.empty())
			{
				logger.warn("No files to extract!\n");
				continue;
			}

			logger.info("Extracting %d files...\n", files.size());

			for (const auto& file : files)
			{
				if (ifs.extract_file(file, file))
				{
					logger.info("Extracted %s\n", file.data());
					
					// Poke the IWI until it gives up its secrets..
					if (std::strstr(file.c_str(), ".iwi"))
					{
						if (!PokeIWI(file))
						{
							logger.error("Failed to poke %s\n", file.data());
						}
					}
				}
				else
				{
					logger.error("Failed to extract %s\n", file.data());
				}
			}
		}
	}
	catch (std::exception& e)
	{
		logger.error("%s\n", e.what());
	}

	return _getch() & 0;
}
