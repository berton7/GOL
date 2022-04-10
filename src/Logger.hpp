#pragma once
#include <iostream>
#include <string>
#include <bitset>

#define LOGGER_DEFAULT_OPTIONS 0x000E

namespace logging {
	typedef std::uint8_t LevelT;
	namespace Level {
		const LevelT Debug = 0;
		const LevelT Info = 1;
		const LevelT Warn = 2;
		const LevelT Error = 3;
		const LevelT Exception = 4;
	};

	typedef std::uint16_t OptionsType;
	enum Options : OptionsType {
		NONE = 0,
		ENABLE_COLOR = 1,
		ENABLE_PRINT_NAME = 2,
		ENABLE_PRINT_LOG_LEVEL = 3
	};
	enum Color
	{
		Black = 0, Blue, Green, Aqua, Red, Purple, Yellow, White, Gray,
		LightBlue, LightGreen, LightAqua, LightRed, LightPurple, LightYellow, BrightWhite
	};
	class Logger
	{
	public:

	private:
		const std::string mName;
		LevelT mLevel{ Level::Warn };
		std::bitset<sizeof(OptionsType) * 8> mOptionsFlags;

		static std::ostream* getOut(LevelT lvl)
		{
			switch (lvl)
			{
			case Level::Debug:
			case Level::Info:
				return &std::cout;
			case Level::Warn:
			case Level::Error:
			case Level::Exception:
				return &std::cerr;
			}
			return nullptr;
		}
		void printName();
		void printLogLevel(LevelT lvl);

		template<typename Arg, typename ...Args>
		void startLog(LevelT lvl, Arg msg, Args && ...args);

		void setColor(LevelT lvl);
		void unsetColor(LevelT lvl);

	public:
		Logger(const std::string& name = "Default");
		Logger(Logger&& other);
		~Logger();

		template<typename Arg>
		static void log(LevelT lvl, Arg&& msg);
		template<typename Arg, typename... Args>
		static void log(LevelT lvl, Arg&& msg, Args &&... args);
		template<typename Arg, typename... Args>
		void debug(Arg&& first, Args &&... args);
		template<typename Arg, typename... Args>
		void info(Arg&& first, Args &&... args);
		template<typename Arg, typename... Args>
		void wrn(Arg&& first, Args &&... args);
		template<typename Arg, typename... Args>
		void warn(Arg&& first, Args &&... args);
		template<typename Arg, typename... Args>
		void error(Arg&& first, Args &&... args);
		template<typename Arg, typename... Args>
		void exception(Arg&& first, Args &&... args);

		void setLevel(LevelT lvl);
		void setOption(Options option, bool value);
		std::bitset<sizeof(OptionsType) * 8> getOptions() const;
		void enableColor(bool enable);
		bool colorEnabled();
		void enablePrintName(bool enable);
		bool printNameEnabled();
	};

	template<typename Arg, typename ...Args>
	inline void Logger::startLog(LevelT lvl, Arg msg, Args && ...args)
	{
		if (mOptionsFlags [Options::ENABLE_PRINT_NAME])
		{
			printName();
		}
		if (mOptionsFlags[Options::ENABLE_COLOR])
		{
			setColor(lvl);
		}
		if (mOptionsFlags [Options::ENABLE_PRINT_LOG_LEVEL])
		{
			printLogLevel(lvl);
		}

		*getOut(lvl) << ": ";
		log(lvl, std::forward<Arg>(msg), std::forward<Args>(args)...);
		
		if (mOptionsFlags[Options::ENABLE_COLOR])
		{
			unsetColor(lvl);
		}
	}

	template<typename Arg>
	inline void Logger::log(LevelT lvl, Arg&& msg)
	{
		*getOut(lvl) << msg << std::endl;;
	}

	template<typename Arg, typename ...Args>
	inline void Logger::log(LevelT lvl, Arg&& msg, Args && ...args)
	{
		*getOut(lvl) << msg;
		log(lvl, std::forward<Args>(args)...);
	}
	
	template<typename Arg, typename ...Args>
	inline void Logger::debug(Arg&& first, Args && ...args)
	{
		if (mLevel <= Level::Debug)
		{
			startLog(Level::Debug, std::forward<Arg>(first), std::forward<Args>(args)...);
		}
	}

	template<typename Arg, typename ...Args>
	inline void Logger::info(Arg&& first, Args && ...args)
	{
		if (mLevel <= Level::Info)
		{
			startLog(Level::Info, std::forward<Arg>(first), std::forward<Args>(args)...);
		}
	}

	template<typename Arg, typename ...Args>
	inline void Logger::warn(Arg&& first, Args && ...args)
	{
		if (mLevel <= Level::Warn)
		{
			startLog(Level::Warn, std::forward<Arg>(first), std::forward<Args>(args)...);
		}
	}

	template<typename Arg, typename ...Args>
	inline void Logger::wrn(Arg&& first, Args && ...args)
	{
		warn(std::forward<Arg>(first), std::forward<Args>(args)...);
	}

	template<typename Arg, typename ...Args>
	inline void Logger::error(Arg&& first, Args && ...args)
	{
		if (mLevel <= Level::Error)
		{
			startLog(Level::Error, std::forward<Arg>(first), std::forward<Args>(args)...);
		}
	}

	template<typename Arg, typename ...Args>
	inline void Logger::exception(Arg&& first, Args && ...args)
	{
		if (mLevel <= Level::Exception)
		{
			startLog(Level::Exception, std::forward<Arg>(first), std::forward<Args>(args)...);
		}
	}
}
