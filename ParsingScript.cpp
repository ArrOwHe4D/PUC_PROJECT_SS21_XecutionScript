//NOTE: project was based on https://github.com/vassilych/cscscpp

#include "ParsingScript.h"

#include "Parser.h"
#include "ScriptHelper.h"
#include "Variable.h"

template<class K, class V>
vector<K> ParsingScript::getKeys(const unordered_map<K, V>& map) 
{
	vector<K> keys;
	keys.reserve(map.size());

	for (auto pair : map) 
	{
		keys.push_back(pair.first);
	}

	std::sort(keys.begin(), keys.end());

	return keys;
}

string ParsingScript::getRawLine(size_t& lineNumber) const 
{
	lineNumber = getRawLineNumber();
	if (lineNumber == string::npos) 
	{
		return "";
	}

	vector<string> lines = ScriptHelper::tokenize(m_rawScript);

	if (lineNumber < lines.size()) 
	{
		return lines[lineNumber];
	}

	return "";
}

size_t ParsingScript::getRawLineNumber() const 
{
	unordered_map<size_t, size_t>& char2Line = getChar2Line();

	if (char2Line.empty()) 
	{
		return string::npos;
	}

	size_t position = m_scriptOffset + m_currentPosition;
	vector<size_t> lineStart = getKeys(char2Line);

	size_t lower = 0;
	size_t index = lower;

	if (position <= lineStart[lower]) 
	{
		return char2Line[lineStart[lower]];
	}
	
	size_t upper = lineStart.size() - 1;

	if (position >= lineStart[upper]) 
	{
		return char2Line[lineStart[upper]];
	}

	while (lower <= upper) 
	{
		index = (lower + upper) / 2;
		size_t guessPosition = lineStart[index];

		if (position == guessPosition) 
		{
			break;
		}
		if (position < guessPosition) 
		{
			if (index == 0 || position > lineStart[index - 1]) 
			{
				break;
			}
			upper = index - 1;
		}
		else 
		{
			lower = index + 1;
		}
	}

	return char2Line[lineStart[index]];
}

Variable ParsingScript::execute(const string& to) 
{
	if (m_data.empty()) 
	{
		return Variable::emptyInstance;
	}
	if (m_data[m_data.size() - 1] != Tokens::END_STATEMENT) 
	{
		m_data += Tokens::END_STATEMENT;
	}
	Variable result = Parser::loadAndCalculate(*this, to);
	return result;
}

Variable ParsingScript::executeFrom(size_t index, const string& to) 
{
	m_currentPosition = index;
	return execute(to);
}

Variable ParsingScript::executeAll(const string& to) 
{
	Variable result;
	
	while (hasNext()) 
	{
		result = Parser::loadAndCalculate(*this, to);
		ScriptHelper::goToNextStatement(*this);
	}

	return result;
}