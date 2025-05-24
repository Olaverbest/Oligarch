#pragma once

#include <iostream>
#include <string>
#include <fstream>

enum class LogLevel {
	INFO,
	WARN,
	ERROR,
	DEBUG
};

class Logger
{
private:
	mutable std::ofstream m_File;
	bool m_ConsoleOutput;

	std::string levelToString(LogLevel level) const;
public:
	Logger(bool consoleOutput = true, const std::string& logPath = "");
	void log(LogLevel level, const std::string& message) const;
};