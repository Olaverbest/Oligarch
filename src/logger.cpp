#include "logger.h"

Logger::Logger(bool consoleOutput, const std::string& logPath)
	: m_ConsoleOutput(consoleOutput) {
	if (!logPath.empty()) {
		m_File.open(logPath);
		if (!m_File.is_open()) std::cerr << "Logger failed to open file: " << logPath << std::endl;
	}
}

std::string Logger::levelToString(LogLevel level) const {
	switch (level) {
		case LogLevel::INFO: return "[INFO]: ";
		case LogLevel::WARN: return "[WARN]: ";
		case LogLevel::ERROR: return "[ERROR]: ";
		case LogLevel::DEBUG: return "[DEBUG]: ";
		default: return "[UNKNOWN]: ";
	}
}

void Logger::log(LogLevel level, const std::string& message) const {
	std::string output = levelToString(level) + message;
	if (level == LogLevel::DEBUG) {
		if (m_File.is_open())
			m_File << output << std::endl;
		return;
	}
	if (m_File.is_open()) m_File << output << std::endl;
	if (m_ConsoleOutput) std::cout << output << std::endl;
}