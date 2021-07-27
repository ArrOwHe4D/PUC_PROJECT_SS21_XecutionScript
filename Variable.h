//NOTE: project was based on https://github.com/vassilych/cscscpp

#pragma once
#include "Tokens.h"
#include <vector>
#include <string>

using namespace std;

class Parser;

class Variable
{
public:
	Variable() : m_type(Tokens::VOID) {}

	Variable(double value) : m_type(Tokens::NUMERIC), m_numericValue(value) {}

	Variable(string stringvalue) : m_type(Tokens::STRING), m_stringValue(stringvalue) {}

	Variable(Tokens::Type type) : m_type(type) {}

	virtual ~Variable() {}

	void set(const string& str) { m_stringValue = str; m_type = Tokens::STRING; }
	void set(const double& val) { m_numericValue = val; m_type = Tokens::NUMERIC; }

	const string& getAction() const { return m_action; }
	Tokens::Type getType() const { return m_type; }

	string toString() const;

	bool canMergeWith(const Variable& right);
	void merge(const Variable& right);

	void mergeNumbers(const Variable& right);
	void mergeStrings(const Variable& right);

	static Variable emptyInstance;

	static int getPriority(const string& action);

	template<typename T>
	static double mergeBool(const T& param1, const T& param2, const string& action);

	//Value related members
	double			 m_numericValue = 0.0;  //NUMERICS have initial value of 0.0
	string			 m_stringValue;

	//Variable information related members
	string			 m_action;
	string			 m_variableName;
	Tokens::Type	 m_type = Tokens::VOID; //Initial type is VOID
	bool			 m_isReturn = false;
};

