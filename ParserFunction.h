//NOTE: project was based on https://github.com/vassilych/cscscpp

#pragma once

#include "Tokens.h"
#include "ScriptHelper.h"
#include "Variable.h"
#include "ParsingScript.h"

class ParserFunction;
class ActionFunction;
class StringOrNumericFunction;
class IdentityFunction;

class ParserFunction
{
public:
	ParserFunction() : m_implementation(this), m_newInstance(false) {}

	ParserFunction(ParsingScript& script, const string& item, char ch, string& action);

	virtual ~ParserFunction();

	//return self by default but let derived functions be able return something else
	virtual ParserFunction* newInstance() { return this; }

	const string& getName() const { return m_name; }
	void setName(const string& name) { m_name = name; }

	bool isGlobal() const { return m_isGlobal; }
	void setGlobal(bool isGlobal) { m_isGlobal = isGlobal; }

	void setNewInstance() { m_newInstance = true; }
	bool isNewInstance() { return m_newInstance; }

	Variable getValue(ParsingScript& script);

	static ParserFunction* getFunction(const string& name) { bool isGlobal = false; return getFunction(name, isGlobal); }
	static ParserFunction* getFunction(const string& name, bool& isGlobal);

	static ActionFunction* getRegisteredAction(const string& name, string& action);
	static ActionFunction* getAction(const string& action);

	static void addAction(const string& name, ActionFunction* action);

	static void addGlobalOrLocalVariable(const string& name, ParserFunction* function, bool onlyGlobal = false);

	static void addGlobalFunction(const string& name, ParserFunction* function, bool isNative = true);

	static void addGlobalVariable(const string& name, ParserFunction* variable);

	template<class T, class S>
	static void add(T& container, S& value, const string& key, bool isNative = true);

	//MEMBERS
protected:

	//This is going to be overwritten by any derived class to create the implementation of the specific derived function
	virtual Variable evaluate(ParsingScript& script) { return Variable::emptyInstance; }

	string m_name;
	bool m_isGlobal = true;
	bool m_isNative = true;

private:
	ParserFunction* m_implementation;
	bool m_newInstance;

	static unordered_map<string, ParserFunction*> m_functions;
	static unordered_map<string, ParserFunction*> m_globals;
	static unordered_map<string, ActionFunction*> m_actions;

	static StringOrNumericFunction* m_strOrNumericFunction;
	static IdentityFunction*		m_idFunction;
};

class ActionFunction : public ParserFunction 
{
public:
	virtual ~ActionFunction() {}
	virtual ActionFunction* newInstance() { return this; }

	void setAction(const string& action) { m_action = action; }

protected:
	string m_action;
};
