#include "stack.h"

#include <iostream>

Stack::Stack(unsigned int size, Logger* logger)
	: m_Logger(logger), m_Buffer(size, 0), m_StackPointer(-1), m_StackSize(size) {}

void Stack::log() const {
	std::cout << "Stack contents (bottom -> top): ";
	if (m_StackPointer < 0) {
		std::cout << "<Empty>";
	} else {
		for (int i = 0; i <= m_StackPointer; i++) {
			const Value& val  = m_Buffer[i];

			switch (val.type)
			{
			case ValueType::Int:
				std::cout << val.asInt();
				break;
			case ValueType::String:
				std::cout << '"' << val.asString() << '"';
				break;
			default:
				std::cout << "<unknown>";
			}
			std::cout << " ";
		}
	}
	std::cout << std::endl;
}

Value Stack::top() const {
	if (m_StackPointer < 0) {
		m_Logger->log(LogLevel::ERROR, "Attempting to read from an empty stack!");
		std::exit(EXIT_FAILURE);
	}
	return m_Buffer[m_StackPointer];
}

void Stack::push(const Value& value) {
	if (m_StackPointer + 1 >= static_cast<int>(m_StackSize)) {
		m_Logger->log(LogLevel::ERROR, "Attempting to push over the stack limit of " + std::to_string(m_StackSize));
		std::exit(EXIT_FAILURE);
	}
	m_Buffer[++m_StackPointer] = value;
}

Value Stack::pop() {
	if (m_StackPointer < 0) {
		m_Logger->log(LogLevel::ERROR, "Attempting to pop from an empty stack!");
		std::exit(EXIT_FAILURE);
	}
	return m_Buffer[m_StackPointer--];
}