#include "std_include.hpp"
#include <conio.h>
#include "logger.hpp"
#include "ifs.hpp"

void handle_archives(const logger& logger, const std::vector<std::string>& archives)
{
	for (auto i = 0u; i < archives.size(); ++i)
	{
		std::filesystem::path archive(archives[i]);
		logger.info("Handling archive %s...\n", archive.filename().string().data());

		const ifs ifs(archives[i]);
		const auto files = ifs.get_file_list();

		if (files.empty())
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

int main(const int argc, char* argv[])
{
	const logger logger;

	try
	{
		logger.header("IFS Tool by momo5502");

		std::vector<std::string> archives;
		if (argc == 2 && std::filesystem::is_directory(argv[1]))
		{
			for (auto& itr : std::filesystem::directory_iterator(argv[1]))
			{
				if (itr.is_regular_file() && itr.path().extension() == ".ifs")
				{
					archives.push_back(itr.path().string());
				}
			}
		}
		else
		{
			for (auto i = 1; i < argc; i++)
			{
				archives.push_back(argv[1]);
			}
		}

		handle_archives(logger, archives);
	}
	catch (std::exception& e)
	{
		logger.error("%s\n", e.what());
	}

	return _getch() & 0;
}
