//NOTE: project was based on https://github.com/vassilych/cscscpp

#include <iostream>

#include "ParserFunction.h"
#include "Functions.h"

unordered_map<string, ParserFunction*> ParserFunction::m_functions;
unordered_map<string, ParserFunction*> ParserFunction::m_globals;
unordered_map<string, ActionFunction*> ParserFunction::m_actions;

StringOrNumericFunction* ParserFunction::m_strOrNumericFunction = new StringOrNumericFunction();
IdentityFunction* ParserFunction::m_idFunction = new IdentityFunction();

ParserFunction::ParserFunction(ParsingScript& script, const string& item, char ch, string& action) : m_newInstance(false) 
{
	if (item.empty() && (ch == Tokens::START_ARG || !script.hasNext())) 
	{
		//only an expression
		m_implementation = m_idFunction;
		return;
	}

	m_implementation = getRegisteredAction(item, action);
	if (m_implementation != 0) { return; }

	m_implementation = getFunction(item);
	if (m_implementation != 0) { return; }

	if (m_implementation == m_strOrNumericFunction && item.empty()) 
	{
		string problem = !action.empty() ? action : string(1, ch);
		string remainingData = string(1, ch) + script.remainingScript();
		throw ParsingException("Could not parse [" + problem + "] in " + remainingData + "...");
	}

	//function was not found, try to parse this as string in quotes or as number.
	m_implementation = m_strOrNumericFunction;
	m_strOrNumericFunction->setItem(item);
}

ParserFunction::~ParserFunction() 
{
	if (m_implementation != 0 && m_implementation != this && m_implementation->isNewInstance()) 
	{
		delete m_implementation;
	}
}

Variable ParserFunction::getValue(ParsingScript& script) 
{
	Variable result = m_implementation->evaluate(script);
	return result;
}

ParserFunction* ParserFunction::getFunction(const string& name, bool& isGlobal) 
{
	isGlobal = true;

	//check if a global variable exists
	auto it = m_globals.find(name);
	if (it != m_globals.end()) 
	{
		return it->second;
	}

	//check if a registered global function with the given name exists
	it = m_functions.find(name);
	if (it != m_functions.end()) 
	{
		return it->second;
	}
	
	return 0;
}

ActionFunction* ParserFunction::getRegisteredAction(const string& name, string& action) 
{
	ActionFunction* actionFunction = getAction(action);
	if (actionFunction == 0) { return 0; }

	//if the passed action exists and is registered we are done.
	ActionFunction* actionPtr = actionFunction->newInstance();
	actionPtr->setName(name);
	actionPtr->setAction(action);

	action = Tokens::EMPTY;
	return actionPtr;
}

ActionFunction* ParserFunction::getAction(const string& action) 
{
	if (action.empty()) { return 0; }

	auto it = m_actions.find(action);
	if (it == m_actions.end()) { return 0; }

	return it->second;
}

void ParserFunction::addAction(const string& name, ActionFunction* action) 
{
	m_actions[name] = action;
}

void ParserFunction::addGlobalFunction(const string& name, ParserFunction* function, bool isNative) 
{
	add(m_functions, function, name, isNative);
}

void ParserFunction::addGlobalOrLocalVariable(const string& name, ParserFunction* function, bool onlyGlobal) 
{
	//we only have global variables since we didnt get to the point to implement custom functions
	addGlobalVariable(name, function);
}

void ParserFunction::addGlobalVariable(const string& name, ParserFunction* function) 
{
	add(m_globals, function, name, false);
}

template<class T, class S>
void ParserFunction::add(T& container, S& value, const string& key, bool isNative) 
{
	if (value->getName().empty()) 
	{
		value->setName(key);
	}

	auto tryInsert = container.insert({ key, value });
	if (!tryInsert.second) 
	{
		//variable or function already exists
		if (isNative) { throw ParsingException("Global name [" + key + "] already exists!"); }

		//delete and replace with the new one
		delete tryInsert.first->second;
		tryInsert.first->second = value;
	}
}

// We need the hack below in order to access the Stack container.
// And we need its container to iterate over all its elements.
template <class ADAPTER>
typename ADAPTER::container_type& getContainer(ADAPTER& a)
{
	struct hack : ADAPTER 
	{
		static typename ADAPTER::container_type& get(ADAPTER& a) 
		{
			return a.* & hack::c;
		}
	};
	return hack::get(a);
}