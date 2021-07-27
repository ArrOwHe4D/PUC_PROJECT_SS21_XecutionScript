//NOTE: project was based on https://github.com/vassilych/cscscpp

#pragma once

#include "ParserFunction.h"
#include "Interpreter.h"
#include "ScriptHelper.h"

class StringOrNumericFunction : public ParserFunction 
{
public:
	virtual Variable evaluate(ParsingScript& script);
	void setItem(const string& item) { m_item = item; }

private:
	string m_item;
};

class IdentityFunction : public ParserFunction 
{
public:
	virtual Variable evaluate(ParsingScript& script);
};

class PrintFunction : public ParserFunction
{
public:
	PrintFunction(bool newLine = true) : m_newLine(newLine) {}

	virtual Variable evaluate(ParsingScript& script);
private:
	bool m_newLine;
};

//CONTROL FLOW
class ForStatement : public ParserFunction
{
public:
	virtual Variable evaluate(ParsingScript& script);
};

class IfStatement : public ParserFunction
{
public:
	virtual Variable evaluate(ParsingScript& script);
};

class WhileStatement : public ParserFunction
{
public:
	virtual Variable evaluate(ParsingScript& script);
};

class BreakStatement : public ParserFunction
{
public:
	virtual Variable evaluate(ParsingScript& script);
};

class ContinueStatement : public ParserFunction
{
public:
	virtual Variable evaluate(ParsingScript& script);
};

class GetVarFunction : public ParserFunction 
{
public:
	GetVarFunction(const Variable& value) : m_value(value), m_delta(0) {}

	virtual Variable evaluate(ParsingScript& script);
	const Variable& getValue() const { return m_value; }
	void setDelta(size_t delta) { m_delta = delta; }

private:
	size_t m_delta;
	Variable m_value;
};

//GENERAL FUNCTIONS
class AssignFunction : public ActionFunction
{
public:
	virtual Variable evaluate(ParsingScript& script);
	virtual ActionFunction* newInstance();
};

class OperatorAssignFunction : public ActionFunction
{
public:
	virtual Variable evaluate(ParsingScript& script);
	virtual ActionFunction* newInstance();

	static void numberOperator(Variable& left, const Variable& right, const string& action);
	static void stringOperator(Variable& left, const Variable& right, const string& action);
};

class IncrementDecrementFunction : public ActionFunction
{
public:
	virtual Variable evaluate(ParsingScript& script);
	virtual ActionFunction* newInstance();
};

