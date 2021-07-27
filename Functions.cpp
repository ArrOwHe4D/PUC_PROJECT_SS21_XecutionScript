//NOTE: project was based on https://github.com/vassilych/cscscpp

#include "Functions.h"
#include "Interpreter.h"
#include "Parser.h"
#include "ScriptHelper.h"

Variable IdentityFunction::evaluate(ParsingScript& script)
{
	return Parser::loadAndCalculate(script, Tokens::END_ARG_STR);
}

Variable StringOrNumericFunction::evaluate(ParsingScript& script) 
{
	//check if the expression is a string between quotes for example "test"
	if (m_item.size() > 1 
		&& m_item[0] == Tokens::QUOTE 
		&& m_item[m_item.size() - 1] == Tokens::QUOTE) 
	{
		//return a new Variable holding the str value between the quotes
		string result = m_item.substr(1, m_item.size() - 2);
		return Variable(result);
	}

	//otherwise we have a number -> convert to double and return a new Variable with that numeric value
	char* tmp;
	double number = ::strtod(m_item.c_str(), &tmp);

	return Variable(number);
}

//GENERAL FUNCTIONS
Variable PrintFunction::evaluate(ParsingScript& script) 
{
	vector<Variable> arguments = ScriptHelper::getArguments(script, Tokens::START_ARG, Tokens::END_ARG);

	for (size_t i = 0; i < arguments.size(); i++) 
	{
		ScriptHelper::print(arguments[i].toString());
	}

	if (m_newLine) 
	{
		ScriptHelper::print("", true);
	}
	return Variable::emptyInstance;
}

//VARIABLES
Variable GetVarFunction::evaluate(ParsingScript& script)
{
	return m_value;
}

//CONTROL STRUCTURES
Variable ForStatement::evaluate(ParsingScript& script)
{
	return Interpreter::evaluateFor(script);
}

Variable IfStatement::evaluate(ParsingScript& script)
{
	return Interpreter::evaluateIf(script);
}

Variable WhileStatement::evaluate(ParsingScript& script)
{
	return Interpreter::evaluateWhile(script);
}

Variable ContinueStatement::evaluate(ParsingScript& script)
{
	return Variable(Tokens::CONTINUE_STATEMENT);
}

Variable BreakStatement::evaluate(ParsingScript& script)
{
	return Variable(Tokens::BREAK_STATEMENT);
}

//ASSIGN FUNCTION
Variable AssignFunction::evaluate(ParsingScript& script)
{
	Variable varValue = ScriptHelper::getItem(script);

	// Special case for adding a string (or a number) to a string.
	while (varValue.m_type == Tokens::STRING && script.tryPreviousChar() == '+') 
	{
		Variable addition = ScriptHelper::getItem(script);
		varValue.m_stringValue += addition.toString();
	}

	ParserFunction::addGlobalOrLocalVariable(m_name, new GetVarFunction(varValue));
	return varValue;
}

ActionFunction* AssignFunction::newInstance()
{
	ActionFunction* newInstance = new AssignFunction();
	newInstance->setNewInstance();
	return newInstance;
}

//OPERATOR ASSIGN FUNCTION
Variable OperatorAssignFunction::evaluate(ParsingScript& script)
{
	Variable right = ScriptHelper::getItem(script);

	bool isGlobal = true;
	ParserFunction* parserFunction = ParserFunction::getFunction(m_name, isGlobal);
	ScriptHelper::checkNotNull(m_name, parserFunction);
	Variable currentValue = parserFunction->getValue(script);
	Variable left = currentValue;

	if (left.m_type == Tokens::NUMERIC) 
	{
		numberOperator(left, right, m_action);
	}
	else 
	{
		stringOperator(left, right, m_action);
	}

	ParserFunction::addGlobalOrLocalVariable(m_name, new GetVarFunction(left), isGlobal);

	return left;
}

void OperatorAssignFunction::numberOperator(Variable& left, const Variable& right, const string& action)
{
	if (action.compare("+=") == 0) 
	{
		left.m_numericValue += right.m_numericValue;
	}
	else if (action.compare("-=") == 0) 
	{
		left.m_numericValue -= right.m_numericValue;
	}
	else if (action.compare("*=") == 0) 
	{
		left.m_numericValue *= right.m_numericValue;
	}
	else if (action.compare("/=") == 0) 
	{
		left.m_numericValue /= right.m_numericValue;
	}
	else if (action.compare("^=") == 0) 
	{
		left.m_numericValue = pow(left.m_numericValue, right.m_numericValue);
	}
	else if (action.compare("%=") == 0) 
	{
		left.m_numericValue = (int)left.m_numericValue % (int)right.m_numericValue;
	}
	else if (action.compare("&=") == 0) 
	{
		left.m_numericValue = (int)left.m_numericValue & (int)right.m_numericValue;
	}
	else if (action.compare("|=") == 0) 
	{
		left.m_numericValue = (int)left.m_numericValue | (int)right.m_numericValue;
	}
}

void OperatorAssignFunction::stringOperator(Variable& left, const Variable& right, const string& action)
{
	if (action.compare("+=") == 0) 
	{
		left.m_stringValue += right.toString();
	}
}

ActionFunction* OperatorAssignFunction::newInstance()
{
	ActionFunction* newInstance = new OperatorAssignFunction();
	newInstance->setNewInstance();
	return newInstance;
}

//INCREMENT + DECREMENT FUNCTION
Variable IncrementDecrementFunction::evaluate(ParsingScript& script)
{
	bool prefix = m_name.empty();
	if (prefix) 
	{
		m_name = ScriptHelper::getToken(script, Tokens::TOKEN_SEPARATORS);
	}

	// check if we need to increment or decrement depending on the action we find
	int valueDelta = m_action == Tokens::INCREMENT ? 1 : -1;
	int returnDelta = prefix ? valueDelta : 0;
	double newValue = 0;

	bool isGlobal = true;
	ParserFunction* func = ParserFunction::getFunction(m_name, isGlobal);
	ScriptHelper::checkNotNull(m_name, func);

	Variable currentValue = func->getValue(script);

	newValue = currentValue.m_numericValue + returnDelta;
	currentValue.m_numericValue += valueDelta;

	ParserFunction::addGlobalOrLocalVariable(m_name, new GetVarFunction(currentValue), isGlobal);
	return newValue;
}

ActionFunction* IncrementDecrementFunction::newInstance()
{
	ActionFunction* newInstance = new IncrementDecrementFunction();
	newInstance->setNewInstance();
	return newInstance;
}