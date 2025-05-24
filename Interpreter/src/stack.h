#pragma once

#include "logger.h"

#include <vector>

class Stack
{
private:
	Logger* m_Logger;
	std::vector<int> m_Buffer;
	int m_StackPointer;
	unsigned int m_StackSize;

public:
	Stack(unsigned int size, Logger* logger);

	void push(int number);
	int pop();
	
	void log() const;
	int top() const;
	int size() const { return m_StackPointer + 1; }
};