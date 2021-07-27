//NOTE: project was based on https://github.com/vassilych/cscscpp

#pragma once
#include <iostream>
#include <string>

#include "Interpreter.h"

void processScript(const string& scriptData);

int main(int argc, char* argv[]) 
{
	Interpreter::initialize();

	string sourceFilePath;

	if (argv[1]) 
	{
		sourceFilePath = argv[1];
	}
	else 
	{
		throw ParsingException("No scriptfile was provided to run the XecutionScript Interpreter!");
	}

	cout << "-- XecutionScript Interpreter 0.1 --" << endl;
	cout << "Loading Scriptfile: " << sourceFilePath << "..." << endl;
	cout << "Output:" << endl;

	string sourceFileData = ScriptHelper::readScriptFile(sourceFilePath);

	if (sourceFileData.empty()) { throw ParsingException("The file that was provided is empty. Nothing to Parse!"); };

	processScript(sourceFileData);
}

void processScript(const string& scriptData) 
{
	Variable result;
	result = Interpreter::evaluate(scriptData);
}