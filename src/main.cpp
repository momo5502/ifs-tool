#include "std_include.hpp"
#include <conio.h>
#include "logger.hpp"
#include "ifs.hpp"

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
