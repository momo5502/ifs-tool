#pragma once

class ifs
{
public:
	ifs(const std::string& file);
	~ifs();

	bool read_file(const std::string& filename, std::string* buffer) const;

	bool is_file(const std::string& filename) const;

	bool extract_file(const std::string& filename, const std::string& outfile) const;

	[[nodiscard]] std::vector<std::string> get_file_list() const;

private:
	struct archive
	{
		char pad[0x138];
		size_t allocation_size;
	};

	struct file
	{
		char pad[0x40];
		char type[4];
	};

	archive* handle_;

	static char* get_library();

	static archive* SFileOpenArchive(const char* file, int unk);
	static void SFileCloseFile(HANDLE handle);
	static void SFileReadFile(HANDLE handle, const char* buffer, size_t size, const size_t* length, int unk);
	static bool SFileExtractFile(HANDLE handle, const char* filename, const char* outfile);

	static bool NIFSOpenFileEx(HANDLE archive, const char* filename, DWORD scope, file** handle, DWORD unk);
};
