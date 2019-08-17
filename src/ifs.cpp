#include "std_include.hpp"
#include "ifs.hpp"

ifs::ifs(const std::string& file)
{
	this->handle_ = SFileOpenArchive(file.data(), 0);
}

ifs::~ifs()
{
	SFileCloseFile(this->handle_);
}

bool ifs::read_file(const std::string& filename, std::string* buffer) const
{
	file* file_handle;
	NIFSOpenFileEx(this->handle_, filename.data(), 1, &file_handle, 0);
	if (!file_handle) return false;

	size_t res_size = 0;
	buffer->resize(this->handle_->allocation_size);

	SFileReadFile(file_handle, buffer->data(), buffer->size(), &res_size, 1);
	SFileCloseFile(file_handle);

	buffer->resize(res_size);

	return res_size != 0;
}

bool ifs::is_file(const std::string& filename) const
{
	file* file_handle = nullptr;
	NIFSOpenFileEx(this->handle_, filename.data(), 1, &file_handle, 0);
	if (!file_handle) return false;

	const bool is_file_type = std::memcmp(file_handle->type, "FILE", 4);

	SFileCloseFile(file_handle);

	return is_file_type;
}

bool ifs::extract_file(const std::string& filename, const std::string& outfile) const
{
	return SFileExtractFile(this->handle_, filename.data(), outfile.data());
}

std::vector<std::string> ifs::get_file_list() const
{
	std::vector<std::string> result;

	std::string list_file;
	if (!this->read_file("(listfile)", &list_file)) return result;

	std::stringstream stream(list_file);
	std::string target;

	while (std::getline(stream, target, '\n'))
	{
		if (!target.empty() && target.back() == '\r')
		{
			target.pop_back();
		}

		if (!target.empty() && this->is_file(target))
		{
			result.push_back(target);
		}
	}

	return result;
}

char* ifs::get_library()
{
	const auto library = LoadLibraryA("IFS2.dll");
	if (!library)
	{
		throw std::runtime_error("Unable to load IFS2.dll");
	}

	return reinterpret_cast<char*>(library);
}

ifs::archive* ifs::SFileOpenArchive(const char* file, const int unk)
{
	const auto library = get_library();
	return reinterpret_cast<archive*(__stdcall*)(const char*, int)>(library + 0x163E0)(file, unk);
}

void ifs::SFileCloseFile(HANDLE handle)
{
	__asm
	{
		call get_library
		add eax, 210A0h
		mov esi, handle
		call eax
	}
}

void ifs::SFileReadFile(HANDLE handle, const char* buffer, const size_t _size, const size_t* _length, const int unk)
{
	__asm
	{
		call get_library
		add eax, 22570h
		mov edx, buffer
		mov ecx, handle
		push unk
		push _length
		push _size
		call eax
	}
}

bool ifs::SFileExtractFile(HANDLE handle, const char* filename, const char* outfile)
{
	auto result = false;

	__asm
	{
		call get_library
		add eax, 25DF0h
		mov ecx, handle
		push outfile
		push filename
		call eax
		mov result, al
	}

	return result;
}

bool ifs::NIFSOpenFileEx(const HANDLE archive, const char* filename, const DWORD scope, file** handle, const DWORD unk)
{
	const auto library = get_library();
	return reinterpret_cast<bool(__stdcall*)(HANDLE, const char*, DWORD, file* *, DWORD)>(library + 0x1FED0)(
		archive, filename, scope, handle, unk);
}
