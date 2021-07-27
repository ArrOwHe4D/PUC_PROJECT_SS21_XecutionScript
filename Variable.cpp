//NOTE: project was based on https://github.com/vassilych/cscscpp

#include <algorithm>

#include "ScriptHelper.h"
#include "Variable.h"

Variable Variable::emptyInstance;

string Variable::toString() const 
{
	if (m_type == Tokens::VOID) 
	{
		return "";
	}
	if (m_type == Tokens::STRING) 
	{
		return m_stringValue;
	}
	if (m_type == Tokens::NUMERIC) 
	{
		return ScriptHelper::isInt(m_numericValue) ? to_string((long long)m_numericValue) : to_string(m_numericValue);
	}
	return "";
}

bool Variable::canMergeWith(const Variable& right) 
{
	return getPriority(m_action) >= getPriority(right.getAction());
}

int Variable::getPriority(const string& action) 
{
	unordered_map<string, int>::const_iterator it = Tokens::PRIORITY.find(action);

	if (it == Tokens::PRIORITY.end()) { return 0; }

	return it->second;
}

void Variable::merge(const Variable& right) 
{
	if (m_type == Tokens::STRING || right.getType() == Tokens::STRING) 
	{
		mergeStrings(right);
	}
	else 
	{
		mergeNumbers(right);
	}

	m_action = right.m_action;
}

void Variable::mergeNumbers(const Variable& right) 
{
	double tryBool = mergeBool(m_numericValue, right.m_numericValue, m_action);

	if (tryBool >= 0) 
	{
		set(tryBool);
		return;
	}

	if (m_action.compare("+") == 0) 
	{
		m_numericValue += right.m_numericValue;
	}
	else if (m_action.compare("-") == 0) 
	{
		m_numericValue -= right.m_numericValue;
	}
	else if (m_action.compare("*") == 0)
	{
		m_numericValue *= right.m_numericValue;
	}
	else if (m_action.compare("/") == 0)
	{
		m_numericValue /= right.m_numericValue;
	}
	else if (m_action.compare("^") == 0)
	{
		m_numericValue = pow(m_numericValue, right.m_numericValue);
	}
	else if (m_action.compare("%") == 0)
	{
		m_numericValue = (int)m_numericValue % (int)right.m_numericValue;
	}
	else if (m_action.compare("&&") == 0)
	{
		m_numericValue = m_numericValue && right.m_numericValue;
	}
	else if (m_action.compare("||") == 0)
	{
		m_numericValue = m_numericValue || right.m_numericValue;
	}
	else 
	{
		throw ParsingException("Syntax Error: The action [" + m_action + "] is not supported for numeric types!");
	}
}

void Variable::mergeStrings(const Variable& right) 
{
	string str_1 = toString();
	string str_2 = right.toString();

	double tryBool = mergeBool(str_1, str_2, m_action);
	if (tryBool >= 0) 
	{
		set(tryBool);
		return;
	}
	if (m_action.compare("+") == 0) 
	{
		set(str_1 + str_2);
		return;
	}

	throw ParsingException("Syntax Error: The action [" + m_action + "] is not supported for strings!");
}

template<class T>
double Variable::mergeBool(const T& param_1, const T& param_2, const string& action) 
{
	if (action.compare(">") == 0)
	{
		return param_1 > param_2;
	}
	else if (action.compare("<") == 0)
	{
		return param_1 < param_2;
	}
	else if (action.compare(">=") == 0)
	{
		return param_1 >= param_2;
	}
	else if (action.compare("<=") == 0)
	{
		return param_1 <= param_2;
	}
	else if (action.compare("==") == 0)
	{
		return param_1 == param_2;
	}
	else if (action.compare("!=") == 0)
	{
		return param_1 != param_2;
	}

	return -1.0;
}