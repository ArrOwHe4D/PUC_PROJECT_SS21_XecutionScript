//NOTE: project was based on https://github.com/vassilych/cscscpp

#pragma once

#include "Tokens.h"
#include "ParserFunction.h"
#include "ScriptHelper.h"
#include "Variable.h"
#include "ParsingScript.h"

class Parser
{
public:
	static Variable loadAndCalculate(ParsingScript& script, const std::string& endCondition);

private:
	static std::vector<Variable> split(ParsingScript& script, const std::string& endCondition);

	static void checkConsistency(const ParsingScript& script, const string& item, const vector<Variable>& listToMerge);

	static bool isStillCollecting(const ParsingScript& script, const string& item, const string& endCondition, string& action);

	static void checkQuotes(const ParsingScript& script, char ch, bool& inQuotes);

	static Variable merge(Variable& current, size_t& index, vector<Variable>& listToMerge, bool mergeOnlyOne = false);

	static string updateAction(ParsingScript& script, const string& endCondition);

	static void updateIfBool(ParsingScript& script, Variable& current);
};

