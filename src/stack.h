#pragma once

#include "logger.h"

#include <vector>
#include <variant>

enum class ValueType {
	Int,
	String
};

struct Value {
	ValueType type;
	std::variant<int, std::string> data;

	Value() : type(ValueType::Int), data(0) {}
	Value(int value) : type(ValueType::Int), data(value) {}
	Value(const std::string& string) : type(ValueType::String), data(string) {}

	int asInt() const {
		if (type != ValueType::Int) throw std::runtime_error("Value is not an int");
		return std::get<int>(data);
	}

	const std::string& asString() const {
		if (type != ValueType::String) throw std::runtime_error("Value is not an string");
		return std::get<std::string>(data);
	}
};

class Stack
{
private:
	Logger* m_Logger;
	std::vector<Value> m_Buffer;
	int m_StackPointer;
	unsigned int m_StackSize;

public:
	Stack(unsigned int size, Logger* logger);

	void push(const Value& value);
	Value pop();
	
	void log() const;
	Value top() const;
	int size() const { return m_StackPointer + 1; }
};