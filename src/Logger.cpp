#include "Logger.hpp"
#ifdef _WIN32
#include <Windows.h>
#endif

namespace logging
{
const char *colorToCode(Color color)
{
	switch (color)
	{
	case Color::Gray:
		return "\033[90m";
	default:
	case Color::White:
		return "\033[37m";
	case Color::Yellow:
		return "\033[93m";
	case Color::Red:
		return "\033[91m";
	}
}

void Logger::setColor(LevelT lvl)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	Color color;
	switch (lvl)
	{
	case Level::Debug:
		color = Color::Gray;
		break;
	default:
	case Level::Info:
		color = Color::White;
		break;
	case Level::Warn:
		color = Color::Yellow;
		break;
	case Level::Error:
		color = Color::Red;
		break;
	case Level::Exception:
		color = Color::Red;
		break;
	}
#ifdef _WIN32
	SetConsoleTextAttribute(hConsole, color);
#elif defined(__linux__)
	*getOut(lvl) <<  colorToCode(color);
#endif
}

void Logger::unsetColor(LevelT lvl)
{
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, Color::White);
#elif defined(__linux__)
	*getOut(lvl) << colorToCode(Color::White);
#endif
}

Logger::Logger(const std::string& name) : mName(name), mOptionsFlags(LOGGER_DEFAULT_OPTIONS)
{
	setLevel(Level::Warn);
}

Logger::Logger(Logger&& other) : mName(other.mName), mLevel(other.mLevel), mOptionsFlags(other.mOptionsFlags)
{
}

Logger::~Logger()
{
}

void Logger::printName()
{
	*getOut(mLevel) << "[" << mName << "]";
}

void Logger::printLogLevel(LevelT lvl)
{
	const char *lvlStr = nullptr;
	switch (lvl)
	{
	case Level::Debug:
		lvlStr = " [D]";
		break;
	case Level::Info:
		lvlStr = " [I]";
		break;
	case Level::Warn:
		lvlStr = " [W]";
		break;
	case Level::Error:
		lvlStr = " [E]";
		break;
	case Level::Exception:
		lvlStr = " [X]";
		break;
	}

	*getOut(mLevel) << lvlStr;
}

void Logger::setLevel(LevelT lvl)
{
	if (lvl < Level::Debug || lvl > Level::Exception)
	{
		debug("[InternalLogger] Tried to set to an invalid level: ", "lvl");
		return;
	}
	mLevel = lvl;
}

void Logger::setOption(Options option, bool value)
{
	mOptionsFlags[option] = value;
}

std::bitset<sizeof(OptionsType) * 8> Logger::getOptions() const
{
	return mOptionsFlags;
}

void Logger::enableColor(bool value)
{
	setOption(Options::ENABLE_COLOR, value);
}

bool Logger::colorEnabled()
{
	return mOptionsFlags[Options::ENABLE_COLOR];
}

void Logger::enablePrintName(bool value)
{
	setOption(Options::ENABLE_PRINT_NAME, value);
}

bool Logger::printNameEnabled()
{
	return mOptionsFlags[Options::ENABLE_PRINT_NAME];
}
}