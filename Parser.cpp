//NOTE: project was based on https://github.com/vassilych/cscscpp

#include "Parser.h"
#include "Variable.h"

#include <algorithm>
#include <stdlib.h>
#include <ctype.h>

Variable Parser::loadAndCalculate(ParsingScript& script, const string& endCondition) 
{
	vector<Variable> vectorToMerge = split(script, endCondition);

	if (vectorToMerge.empty()) 
	{
		throw ParsingException("Syntax Error: Failed to parse [" + script.remainingScript() + "]");
	}

	if (vectorToMerge.size() == 1) 
	{
		return vectorToMerge[0];
	}

    Variable& variable = vectorToMerge[0];
    size_t index = 1;
	
	Variable result = merge(variable, index, vectorToMerge);
	return result;
}

vector<Variable> Parser::split(ParsingScript& script, const string& endCondition)
{
    vector<Variable> vectorToMerge;

    if (!script.hasNext() || ScriptHelper::contains(endCondition, script.currentChar())) 
    {
        script.increasePointer();
        vectorToMerge.emplace_back(Variable::emptyInstance);
        return vectorToMerge;
    }

    string parsingItem;
    int negated = 0;
    bool inQuotes = false;    

    do
    { // Main processing cycle of the first part.
        
        string negateSymbol = ScriptHelper::isNotSign(script.remainingScript());
        if (!negateSymbol.empty()) 
        {
            negated++;
            script.increasePointer(negateSymbol.size());
            continue;
        }

        char ch = script.currentCharAndIncreasePointer();
        checkQuotes(script, ch, inQuotes);

        string action = Tokens::EMPTY;
        
        if (inQuotes || isStillCollecting(script, parsingItem, endCondition, action))
        { 
            parsingItem += ch;            
			if (script.hasNext() && (inQuotes || !ScriptHelper::contains(endCondition, script.currentChar()))) {
                continue;
            }
        }

        checkConsistency(script, parsingItem, vectorToMerge);

        ScriptHelper::increasePointerIf(script, Tokens::SPACE);

        if (action.size() > 1) 
        {
            script.increasePointer(action.size() - 1);
        }

        // We are done getting the next token. The getValue() call below may
        // recursively call loadAndCalculate(). This will happen if extracted
        // item is a function or if the next item is starting with a START_ARG '('.
        ParserFunction func(script, parsingItem, ch, action);
        Variable current = func.getValue(script);

        if (negated > 0 && current.getType() == Tokens::NUMERIC) 
        {
            // If there has been a NOT sign, this is a boolean.
            // Use XOR (true if exactly one of the arguments is true).
            bool boolRes = !((negated % 2 == 0) ^ ScriptHelper::toBool(current.m_numericValue));
            current = Variable(boolRes);
            negated = 0;
        }

        if (action.empty()) 
        {
            action = updateAction(script, endCondition);
        }
        else 
        {
            ScriptHelper::increasePointerIf(script, action[0]);
        }

        char next = script.tryCurrentChar(); // we've already moved forward
        
        if (vectorToMerge.empty() && (next == Tokens::END_STATEMENT ||
            (action == Tokens::NULL_ACTION && current.getType() != Tokens::NUMERIC))) 
        {
            // If there is no numerical result, we are not in a math expression.
            if (!action.empty() && action != Tokens::END_ARG_STR) 
            {
                throw ParsingException("Syntax Error: Action [" + action + "] without an argument.", script);
            }
            vectorToMerge.emplace_back(current);
            return vectorToMerge;
        }

        current.m_action = action;
        updateIfBool(script, current);

        vectorToMerge.emplace_back(current);
        parsingItem.clear();

    } while (script.hasNext() && (inQuotes || !ScriptHelper::contains(endCondition, script.currentChar())));

    // This happens when called recursively inside of the math expression:
    ScriptHelper::increasePointerIf(script, Tokens::END_ARG);

    return vectorToMerge;
}

bool Parser::isStillCollecting(const ParsingScript& script, const string& item, const string& endCondition, string& action)
{
    char prev = script.tryPrePreviousChar();
    char ch = script.tryPreviousChar();
    char next = script.tryCurrentChar();

    if (ScriptHelper::contains(endCondition, ch) || ch == Tokens::START_ARG ||
        ch == Tokens::START_GROUP ||
        next == Tokens::NULL_CHAR) 
    {
        return false;
    }

    // Case of a negative number, or starting with the closing bracket:
    if (item.empty() &&
        ((ch == '-' && next != '-') || ch == Tokens::END_ARG)) 
    {
        return true;
    }

    // Otherwise, if it's an action (+, -, *, etc.) or a space
    // we're done collecting current token.
    ParsingScript tempScript(script.getData(), script.getPointer() - 1);
    action = ScriptHelper::getValidAction(tempScript);

    if (action != Tokens::EMPTY || (item.size() > 0 && ch == Tokens::SPACE)) 
    {
        return false;
    }

    return true;
}

void Parser::checkQuotes(const ParsingScript& script,
    char ch, bool& inQuotes)
{
    switch (ch)
    {
        case Tokens::QUOTE:
        {
            char prev = script.tryPrePreviousChar();
            inQuotes = prev != '\\' ? !inQuotes : inQuotes;
            return;
        }
    }
}

void Parser::checkConsistency(const ParsingScript& script, const string& item, const vector<Variable>& listToMerge)
{
    if (listToMerge.empty()) 
    {
        return;
    }
    
    if (Tokens::CONTROL_FLOW.find(item) != Tokens::CONTROL_FLOW.end()) 
    {
        // This can happen when the end of statement ";" is forgotten.
        throw ParsingException("Syntax Error: Token [" + item + "] can't be part of an expression. Check \";\".", script);
    }
}

void Parser::updateIfBool(ParsingScript& script, Variable& current)
{
    if ((current.m_action == "&&" && current.m_numericValue == 0) ||
        (current.m_action == "||" && current.m_numericValue != 0)) 
    {
        // Short circuit evaluation: don't need to evaluate more.
        ScriptHelper::skipRemainingExpression(script);
        current.m_action = Tokens::NULL_ACTION;
    }
}

string Parser::updateAction(ParsingScript& script, const string& endCondition)
{
    // We search a valid action till we get to the End of Argument ')'
    // or pass the end of string.
    if (!script.hasNext() || script.currentChar() == Tokens::END_ARG ||
        ScriptHelper::contains(endCondition, script.currentChar())) 
    {
        return Tokens::NULL_ACTION;
    }
    
    ParsingScript tempScript(script.getData(), script.getPointer());
    string action = ScriptHelper::getValidAction(tempScript);    
    
    size_t advance = action.empty() ? 0 : action.size();
    script.increasePointer(advance);
    return action.empty() ? Tokens::NULL_ACTION : action;
}

Variable Parser::merge(Variable& current, size_t& index,
        vector<Variable>& vectorToMerge, bool mergeOneOnly)
{
    while (index < vectorToMerge.size())
    {
        Variable& next = vectorToMerge[index++];

        while (!current.canMergeWith(next))
        {             
            merge(next, index, vectorToMerge, true);
        }

        current.merge(next);
        if (mergeOneOnly) 
        {
            break;
        }
    }

    return current;
}