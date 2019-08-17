#pragma once

class logger
{
public:
	logger();

	template <typename ... Args>
	void info(const std::string& format, Args ... args) const
	{
		this->set_color(white);

		const auto message = this->format(format, args...);
		std::cout << message;
	}

	template <typename ... Args>
	void warn(const std::string& format, Args ... args) const
	{
		this->set_color(yellow | light);

		const auto message = this->format(format, args...);
		std::cout << message;

		this->set_color(white);
	}

	template <typename ... Args>
	void error(const std::string& format, Args ... args) const
	{
		this->set_color(red | light);

		const auto message = this->format(format, args...);
		std::cout << message;

		this->set_color(white);
	}

	template <typename ... Args>
	void success(const std::string& format, Args ... args) const
	{
		this->set_color(green | light);

		const auto message = this->format(format, args...);
		std::cout << message;

		this->set_color(white);
	}

	void header(const std::string& text) const;

private:
	enum color
	{
		black = 0,
		blue = 1,
		green = 2,
		cyan = 3,
		red = 4,
		pink = 5,
		yellow = 6,
		white = 7,
		light = 8,
	};

	void set_color(short foreground, short background = black) const;

	[[nodiscard]] int get_width() const;

	template <typename ... Args>
	static std::string format(const std::string& format, Args ... args)
	{
		const size_t size = snprintf(nullptr, 0, format.data(), args ...) + 1;

		std::vector<char> buf;
		buf.resize(size);
		snprintf(buf.data(), size, format.data(), args ...);

		return std::string(buf.data(), buf.data() + size - 1);
	}

	void filled_line(short color) const;
	void enclosed_line(short color) const;
	void enclosed_text(short color, const std::string& text) const;

	HANDLE handle_;
};
