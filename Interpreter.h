//NOTE: project was based on https://github.com/vassilych/cscscpp

#pragma once

#include "ScriptHelper.h"

class Interpreter
{
public:
	static void initialize();
	static Variable evaluate(const string& script);
	
	static Variable evaluateIf(ParsingScript& script);
	static Variable evaluateWhile(ParsingScript& script);
	static Variable evaluateFor(ParsingScript& script);
	static void		evaluateStandardFor(ParsingScript& script, const string& forStatement);

private:
	static Variable evaluateBlock(ParsingScript& script);
	static void skipBlock(ParsingScript& script);
	static void skipRemainingBlocks(ParsingScript& script);
};

