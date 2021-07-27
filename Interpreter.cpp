//NOTE: project was based on https://github.com/vassilych/cscscpp

#include <iostream>

#include "Interpreter.h"
#include "Functions.h"
#include "Parser.h"
#include "ParserFunction.h"

void Interpreter::initialize() 
{
	// Add control flow functions	  
	ParserFunction::addGlobalFunction(Tokens::BREAK, new BreakStatement());
	ParserFunction::addGlobalFunction(Tokens::CONTINUE, new ContinueStatement());
	ParserFunction::addGlobalFunction(Tokens::FOR, new ForStatement());
	ParserFunction::addGlobalFunction(Tokens::IF, new IfStatement());
	ParserFunction::addGlobalFunction(Tokens::WHILE, new WhileStatement());

	// Add global functions
	ParserFunction::addGlobalFunction(Tokens::PRINT, new PrintFunction(true));

	// Operator Functions
	ParserFunction::addAction(Tokens::ASSIGNMENT, new AssignFunction());
	ParserFunction::addAction(Tokens::INCREMENT, new IncrementDecrementFunction());
	ParserFunction::addAction(Tokens::DECREMENT, new IncrementDecrementFunction());

	// Add valid actions
	for (size_t i = 0; i < Tokens::OPERATOR_ACTIONS.size(); i++) 
	{
		ParserFunction::addAction(Tokens::OPERATOR_ACTIONS[i], new OperatorAssignFunction());
	}
}

Variable Interpreter::evaluate(const string& script) 
{
	unordered_map<size_t, size_t> char2Line;
	string data = ScriptHelper::convertToScript(script, char2Line);
	
	if (data.empty()) 
	{
		return Variable::emptyInstance;
	}

	ParsingScript parsingScript(data);
	parsingScript.setChar2Line(char2Line);
	parsingScript.setRawScript(script);
	Variable result;

	while (parsingScript.hasNext()) 
	{
		result = Parser::loadAndCalculate(parsingScript, Tokens::END_PARSING_STR);
		ScriptHelper::goToNextStatement(parsingScript);
	}

	return result;
}

Variable Interpreter::evaluateIf(ParsingScript& script) 
{
	size_t startIfCondition = script.getPointer();

	Variable result = Parser::loadAndCalculate(script, Tokens::END_ARG_STR);
	bool isTrue = result.m_numericValue != 0;

	if (isTrue)
	{
		result = evaluateBlock(script);

		if (result.m_type == Tokens::BREAK_STATEMENT || result.m_type == Tokens::CONTINUE_STATEMENT) 
		{
			script.setPointer(startIfCondition);
			skipBlock(script);
		}
		skipRemainingBlocks(script);
		return result;
	}

	// If above code is not called, we are in else and need to skip the whole if part
	skipBlock(script);

	ParsingScript nextData(script);
	string nextToken = ScriptHelper::getNextToken(nextData);

	if (Tokens::ELSE_IF_LIST.find(nextToken) != Tokens::ELSE_IF_LIST.end()) 
	{
		script.setPointer(nextData.getPointer() + 1);
		result = evaluateIf(script);
	}
	if (Tokens::ELSE_LIST.find(nextToken) != Tokens::ELSE_LIST.end()) 
	{
		script.setPointer(nextData.getPointer() + 1);
		result = evaluateBlock(script);
	}

	return Variable::emptyInstance;
}

Variable Interpreter::evaluateFor(ParsingScript& script) 
{
	string forStatement = ScriptHelper::getBodyBetween(script, Tokens::START_ARG, Tokens::END_ARG);
	script.increasePointer();

	evaluateStandardFor(script, forStatement);

	return Variable::emptyInstance;
}

void Interpreter::evaluateStandardFor(ParsingScript& script, const string& forStatement) 
{
	vector<string> forTokens = ScriptHelper::tokenize(forStatement, string(1, Tokens::END_STATEMENT));
	
	if (forTokens.size() != 3) 
	{
		throw ParsingException("Syntax Error: Expecting \"for (init; condition; loopStatement)\"", script);
	}

	ParsingScript initPart(forTokens[0] + Tokens::END_STATEMENT);
	ParsingScript conditionPart(forTokens[1] + Tokens::END_STATEMENT);
	ParsingScript loopPart(forTokens[2] + Tokens::END_STATEMENT);

	initPart.execute();

	size_t startForCondition = script.getPointer();

	int iterations = 0;
	bool isValid = true;
	Variable result;

	while (isValid) 
	{
		Variable conditionResult = conditionPart.executeFrom(0);
		isValid = conditionResult.m_numericValue != 0;

		if (!isValid) { break; }

		script.setPointer(startForCondition);

		if (++iterations >= Tokens::MAX_LOOPS) 
		{
			throw ParsingException("Semantic Error: Seems like an infinite loop after" + to_string(iterations) + " iterations", script);
		}

		result = evaluateBlock(script);

		if (result.m_type == Tokens::BREAK_STATEMENT) 
		{
			script.setPointer(startForCondition);
			skipBlock(script);
			break;
		}
		loopPart.executeFrom(0);
	}
}

Variable Interpreter::evaluateWhile(ParsingScript& script) 
{
	size_t startWhileCondition = script.getPointer();

	int iterations = 0;
	bool isValid = true;
	Variable result;

	while (isValid) 
	{
		script.setPointer(startWhileCondition);

		result = Parser::loadAndCalculate(script, Tokens::END_ARG_STR);
		isValid = result.m_numericValue != 0;

		if (!isValid) { break; }
		
		if (++iterations >= Tokens::MAX_LOOPS) 
		{
			throw ParsingException("Semantic Error: Seems like an infinite loop after" + to_string(iterations) + " iterations");
		}

		result = evaluateBlock(script);

		if (result.m_type == Tokens::BREAK_STATEMENT) 
		{
			script.setPointer(startWhileCondition);
			break;
		}
	}

	//while condition is now false so we skip the while block to continue with the next statement.
	skipBlock(script);
	return Variable::emptyInstance;
}

Variable Interpreter::evaluateBlock(ParsingScript& script) 
{
	size_t blockBegin = script.getPointer();
	Variable result;

	while (script.hasNext()) 
	{
		int endGroup = ScriptHelper::goToNextStatement(script);

		if (endGroup > 0) 
		{
			return result;
		}
		if (!script.hasNext()) 
		{
			throw ParsingException("Syntax Error: Could not process block [" + script.substr(blockBegin) + "]", script);
		}

		result = Parser::loadAndCalculate(script, Tokens::END_PARSING_STR);

		if (result.m_type == Tokens::BREAK_STATEMENT || result.m_type == Tokens::CONTINUE_STATEMENT) 
		{
			return result;
		}
	}

	return result;
}

void Interpreter::skipBlock(ParsingScript& script) 
{
	size_t blockBegin = script.getPointer();

	int startCount = 0;
	int endCount = 0;

	while (startCount == 0 || startCount > endCount) 
	{
		if (!script.hasNext()) 
		{
			throw ParsingException("Syntax Error: Could not skip block [" + script.substr(blockBegin) + "]", script);
		}

		char current = script.currentChar();
		script.increasePointer();

		switch (current) 
		{
			case Tokens::START_GROUP:
				startCount++;
				break;
			case Tokens::END_GROUP:
				endCount++;
				break;
		}
	}

	if (startCount != endCount) 
	{
		throw ParsingException("Syntax Error: Parantheses don´t match!", script);
	}
}

void Interpreter::skipRemainingBlocks(ParsingScript& script) 
{
	while (script.hasNext()) 
	{
		ParsingScript nextScriptData(script);
		string nextToken = ScriptHelper::getNextToken(nextScriptData);

		if (Tokens::ELSE_IF_LIST.find(nextToken) == Tokens::ELSE_IF_LIST.end() 
			&& Tokens::ELSE_LIST.find(nextToken) == Tokens::ELSE_LIST.end()) 
		{
			return;
		}
		script.setPointer(nextScriptData.getPointer());
		skipBlock(script);
	}
}