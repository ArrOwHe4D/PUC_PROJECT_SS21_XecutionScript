//NOTE: project was based on https://github.com/vassilych/cscscpp

#pragma once
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace std;

class Tokens
{
public:

	enum Type
	{
		VOID,
		NUMERIC,
		STRING,
		BREAK_STATEMENT,
		CONTINUE_STATEMENT
	};

	static const size_t MAX_LOOPS		  = 100000;
	static const size_t MAX_CHARS_TO_SHOW = 40;

	//BASIC CHARS
	static const char START_ARG		= '(';
	static const char END_ARG		= ')';
	static const char START_GROUP	= '{';
	static const char END_GROUP		= '}';
	static const char NEXT_ARG		= ',';
	static const char END_LINE		= '\n';
	static const char NULL_CHAR		= '\0';
	static const char NEXT_LINE		= '\\';
	static const char SPACE			= ' ';
	static const char QUOTE			= '"';
	static const char FOR_EACH		= ':';
	static const char END_STATEMENT = ';';

	//LOGIC OPERATORS
	static const string ASSIGNMENT;
	static const string INCREMENT;
	static const string DECREMENT;
	static const string NOT;

	//IDENTIFIER TOKENS
	static const string EMPTY;
	static const string END_ARG_STR;
	static const string END_PARSING_STR;
	static const string FUNCTION_SIGNATURE_SEPARATORS;
	static const string INVALID_CHAR;
	static const string NEXT_OR_END;
	static const string NEW_LINE;
	static const string NULL_ACTION;
	static const string TOKEN_SEPARATORS;
	static const string WHITESPACE;

	//CONTROL STRUCTURES
	static const string BREAK;
	static const string COMMENT;
	static const string CONTINUE;
	static const string IF;
	static const string ELSE;
	static const string ELSE_IF;
	static const string EXIT;
	static const string FOR;
	static const string FUNCTION;
	static const string INCLUDE;
	static const string RETURN;
	static const string SIZE;
	static const string WHILE;

	//GENERAL BUILT IN GLOBAL FUNCTIONS
	static const string PRINT;
	static const string TYPE;

	static const vector<string> ACTIONS;
	static const vector<string> MATH_ACTIONS;
	static const vector<string> OPERATOR_ACTIONS;

	static vector<string> FUNCTION_WITH_SPACE;
	static vector<string> FUNCTION_WITH_SPACE_ONCE;

	static set<string> ELSE_LIST;
	static set<string> ELSE_IF_LIST;

	static set<string> CONTROL_FLOW;

	static const unordered_map<string, int> PRIORITY;

	static string typeToString(Type type);
};



