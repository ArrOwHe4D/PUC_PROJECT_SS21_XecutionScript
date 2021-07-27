//NOTE: project was based on https://github.com/vassilych/cscscpp

#pragma once

#include <iostream>
#include <sstream>
#include <algorithm>
#include <locale>
#include <memory>
#include <codecvt>

#include "Parser.h"
#include "Variable.h"
#include "ScriptHelper.h"
#include "ParsingScript.h"

/* 
*  This class contains a variety of static helper
*  and convenience functions that help
*  to analyze and evaluate the script.
*/

//Custom Exception class to throw parsing exceptions
class ParsingException : public exception 
{
public:
	ParsingException(const string& error) : exception(), m_error(error) 
	{ 
		cout << this->m_error;
		exit(-1); 
	}

	ParsingException(const string& error, const ParsingScript& script) : exception(), m_error(error)
	{
		cout << this->m_error << " at line " << script.getRawLineNumber();
		exit(-1);
	}

private:
	string m_error;
};

class ScriptHelper
{
public:
	static string findStartingToken(const string& expression, const vector<string>& items);
	static bool startsWith(const string& expression, const string& pattern);

	static bool contains(const string& expression, const string& pattern);
	static bool contains(const string& expression, char character);

	static string getValidAction(const ParsingScript& script);

	static bool increasePointerIf(ParsingScript& script, char expected, char nextExpected = Tokens::NULL_CHAR);
	static bool moveBackwardIf(ParsingScript& script, char notExpected);	

	static bool toBool(double value);
	static bool isInt(double value);

	static void print(const string& argument, bool printNewLine = false);

	static string readScriptFile(const string& path);

	static void checkInteger(const Variable& variable);
	static void checkNonNegativeInteger(const Variable& variable);

	static vector<Variable> getArguments(ParsingScript& script, char start, char end);

	static string getBodyBetween(ParsingScript& script, char start, char end);

	static void skipRemainingExpression(ParsingScript& script);

	static string isNotSign(const string& data);

	static string getNextToken(ParsingScript& script);

	static int goToNextStatement(ParsingScript& script);

	static vector<string> tokenize(const string& data, const string& delimeters = Tokens::NEW_LINE, size_t from = 0, size_t to = string::npos, bool removeEmpty = false);

	static Variable getItem(ParsingScript& script);
	static string getToken(ParsingScript& script, const string& to = Tokens::END_PARSING_STR);

	static void checkArgsNumber(size_t expected, size_t supplied, const string& name);
	static void checkNotNull(const string& varName, const void* func);
	static void checkNotEnd(const ParsingScript& script, const string& name);

	static string convertToScript(const string& rawData, unordered_map<size_t, size_t>& char2Line);

	//RELATED TO CONVERT TO SCRIPT
	static bool endsWithFunction(const string& buffer, const vector<string>& functions);
	static bool spaceNotNeeded(char next);
	static bool keepSpace(const string& script, char next);
	static bool keepSpaceOnce(const string& script, char next);
	static void checkSpecialChars(string const& part);
	static string toUpper(const string& str);
	static string toHex(int i);
	static string trim(string const& str);

	//converts a string to a wide string (UTF-8)
	static wstring s2w(const string& str);
	//other direction
	static string  w2s(const wstring& wstr);
};

