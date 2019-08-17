#include "std_include.hpp"
#include "logger.hpp"

logger::logger()
{
	this->handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
}

void logger::header(const std::string& text) const
{
	const short color = green | light;

	const auto width = this->get_width();

	this->filled_line(color);
	this->enclosed_line(color);
	this->enclosed_text(color, text);
	this->enclosed_line(color);
	this->filled_line(color);

	std::cout << std::endl;

	this->set_color(white);
}

void logger::set_color(const short foreground, const short background) const
{
	SetConsoleTextAttribute(this->handle_, foreground | background << 4);
}

int logger::get_width() const
{
	CONSOLE_SCREEN_BUFFER_INFO info;
	GetConsoleScreenBufferInfo(this->handle_, &info);

	return info.srWindow.Right - info.srWindow.Left + 1;
}

void logger::filled_line(const short color) const
{
	const auto width = this->get_width();

	this->set_color(black, color);

	for (auto i = 0; i < width; ++i)
	{
		std::cout << " "s;
	}
}

void logger::enclosed_line(const short color) const
{
	const auto width = this->get_width();

	this->set_color(black, color);
	std::cout << "  "s;

	this->set_color(white);
	for (auto i = 0; i < width - 4; ++i)
	{
		std::cout << " "s;
	}

	this->set_color(black, color);
	std::cout << "  "s;
}

void logger::enclosed_text(const short color, const std::string& text) const
{
	const unsigned int width = this->get_width();

	this->set_color(black, color);
	std::cout << "  "s;

	this->set_color(white | light);
	const auto space = width / 2 - text.size() / 2;
	for (auto i = 2u; i < space; ++i)
	{
		std::cout << " "s;
	}
	std::cout << text;
	for (auto i = space + text.size(); i < width - 2; ++i)
	{
		std::cout << " "s;
	}

	this->set_color(black, color);
	std::cout << "  "s;
}
