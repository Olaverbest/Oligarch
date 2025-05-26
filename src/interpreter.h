#pragma once

#include "stack.h"

#include <string>
#include <vector>
#include <unordered_map>

class Interpreter
{
private:
	std::vector<std::string> m_Program;
	std::unordered_map<std::string, int> m_Label_Tracker;
	std::unordered_map<std::string, std::vector<std::string>> m_FunctionParameters;
	std::unordered_map<std::string, Value> m_Variables;
	Stack m_Stack;
	Logger *m_Logger;
public:
	Interpreter(unsigned int stackSize, Logger* logger);
	void readFile(const std::string &file);
	void run();
private:
	Value resolveValue(const std::string& token) const;
	void executeInstruction(std::string opcode, size_t& program_counter);
};