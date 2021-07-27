//NOTE: project was based on https://github.com/vassilych/cscscpp

#include "Tokens.h"

//IDENTIFIER
const string Tokens::EMPTY = "";
const string Tokens::WHITESPACE = " \t\r";
const string Tokens::END_ARG_STR(1, END_ARG);
const string Tokens::NULL_ACTION(1, END_ARG);
const string Tokens::NEW_LINE = "\n";
const string Tokens::INVALID_CHAR = "\1";

//IDENTIFIER TOKENS
const string Tokens::END_PARSING_STR = SPACE + END_ARG_STR + END_GROUP + NEW_LINE + END_STATEMENT;
const string Tokens::FUNCTION_SIGNATURE_SEPARATORS = NEXT_ARG + END_ARG_STR;
const string Tokens::NEXT_OR_END = END_PARSING_STR + NEXT_ARG;
const string Tokens::TOKEN_SEPARATORS = "<>=+-*/%&|^,!()[]{}\t\n; ";

//LOGIC OPERATORS
const string Tokens::ASSIGNMENT	= "=";
const string Tokens::INCREMENT	= "++";
const string Tokens::DECREMENT	= "--";
const string Tokens::NOT		= "!";

//CONTROL STRUCTURES
const string Tokens::BREAK		= "break";
const string Tokens::COMMENT	= "//";
const string Tokens::CONTINUE	= "continue";
const string Tokens::IF			= "if";
const string Tokens::ELSE		= "else";
const string Tokens::ELSE_IF	= "eif";
const string Tokens::EXIT		= "exit";
const string Tokens::FOR		= "for";
const string Tokens::FUNCTION	= "function";
const string Tokens::RETURN		= "return";
const string Tokens::WHILE		= "while";
const string Tokens::TYPE		= "typeof";

//GENERAL BUILT IN FUNCTIONS
const string Tokens::PRINT		= "print";

vector<string> Tokens::FUNCTION_WITH_SPACE = { };
vector<string> Tokens::FUNCTION_WITH_SPACE_ONCE = { RETURN };

const vector<string> Tokens::MATH_ACTIONS = { "&&", "||", "==", "!=", "<=", ">=", "++", "--", "%", "*", "/", "+", "-", "^", "<", ">", "=" };
const vector<string> Tokens::OPERATOR_ACTIONS = { "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=" };

set<string> Tokens::ELSE_LIST = { ELSE };
set<string> Tokens::ELSE_IF_LIST = { ELSE_IF };

set<string> Tokens::CONTROL_FLOW = { BREAK, CONTINUE, FUNCTION, IF, WHILE, RETURN };

vector<string> initActions()
{
	vector<string> allActions(Tokens::OPERATOR_ACTIONS);

	allActions.insert(allActions.end(), Tokens::MATH_ACTIONS.begin(), Tokens::MATH_ACTIONS.end());
	return allActions;
}

const vector<string> Tokens::ACTIONS(initActions());

unordered_map<string, int> initPriorities() 
{
	unordered_map<string, int> priorities;
	priorities["++"] = 10;
	priorities["--"] = 10;
	priorities["^"]  = 9;
	priorities["%"]  = 8;
	priorities["*"]  = 8;
	priorities["/"]  = 8;
	priorities["+"]  = 7;
	priorities["-"]  = 7;
	priorities["<"]  = 6;
	priorities[">"]  = 6;
	priorities["<="] = 6;
	priorities[">="] = 6;
	priorities["=="] = 5;
	priorities["!="] = 5;
	priorities["&&"] = 4;
	priorities["||"] = 3;
	priorities["+="] = 2;
	priorities["-="] = 2;
	priorities["*="] = 2;
	priorities["/="] = 2;
	priorities["%="] = 2;
	priorities["="]  = 2;

	return priorities;
}

const unordered_map<string, int> Tokens::PRIORITY(initPriorities());

string Tokens::typeToString(Type type) 
{
	switch (type) 
	{
		case NUMERIC:				return "NUMERIC";
		case STRING:				return "STRING";
		case BREAK_STATEMENT:		return "BREAK";
		case CONTINUE_STATEMENT:	return "CONTINUE";
		default:					return "VOID";
	}
}