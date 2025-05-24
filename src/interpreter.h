#pragma once

#include <string>
#include <vector>
#include <unordered_map>

class Stack
{
private:
	std::vector<int> m_Buffer;
	int m_StackPointer;
	uint m_StackSize;
public:
	Stack(uint size);

	void log() const;
	int top() const;
	int size() const { return m_StackPointer + 1; }

	void push(int number);
	int pop();
};

class Interpreter
{
private:
	std::vector<std::string> m_Program;
	std::unordered_map<std::string, int> m_Label_Tracker;
	std::unordered_map<std::string, int> m_Variables;
	Stack m_Stack;
public:
	Interpreter(uint stackSize);
	void readFile(const std::string &file);
	void run();
private:
	int resolveValue(const std::string& token) const;
	void executeInstruction(std::string opcode, size_t& program_counter);
};