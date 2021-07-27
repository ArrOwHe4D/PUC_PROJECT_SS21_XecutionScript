//NOTE: project was based on https://github.com/vassilych/cscscpp

#include <iostream>
#include <fstream>

#include "ScriptHelper.h"

string ScriptHelper::findStartingToken(const string& data, const vector<string>& items) 
{
	for (size_t i = 0; i < items.size(); i++) 
	{
		if (ScriptHelper::startsWith(data, items[i])) 
		{
			return items[i];
		}
	}

	return Tokens::EMPTY;
}

bool ScriptHelper::startsWith(const string& expression, const string& pattern) 
{
	size_t index = pattern.size();

	//the searched pattern is longer than the expression we want to compare it to.
	if (index > expression.size()) 
	{
		return false;
	}

	//we compare the expression till the last character of the pattern we search for.
	int comparison = strncmp(expression.c_str(), pattern.c_str(), index);

	return comparison == 0;
}

bool ScriptHelper::contains(const string& expression, const string& pattern) 
{
	return expression.find(pattern) != string::npos;
}

bool ScriptHelper::contains(const string& expression, char ch) 
{
	return expression.find(ch) != string::npos;
}

string ScriptHelper::getValidAction(const ParsingScript& script) 
{
	if (!script.hasNext()) { return Tokens::EMPTY; }

	string expression = script.remainingScript(string::npos);
	string action = findStartingToken(expression, Tokens::ACTIONS);
	return action;
}

bool ScriptHelper::increasePointerIf(ParsingScript& script, char expected, char nextExpected) 
{
	if (script.tryCurrentChar() == expected || script.tryCurrentChar() == nextExpected) 
	{
		script.increasePointer();
		return true;
	}
	return false;
}

bool ScriptHelper::moveBackwardIf(ParsingScript& script, char notExpected) 
{
	if (script.tryCurrentChar() == notExpected) 
	{
		script.backward();
		return true;
	}
	return false;
}

bool ScriptHelper::toBool(double value) 
{
	return value != 0;
}

//Determines if a variable (NUMERIC in our language) is of type Integer or Float
bool ScriptHelper::isInt(double val) 
{
	return val < LLONG_MAX && val > LLONG_MIN && (val == floor(val));
}

void ScriptHelper::print(const string& argument, bool printNewLine)
{
    cout << argument;
    if (printNewLine) cout << endl;
}

void ScriptHelper::checkInteger(const Variable& variable) 
{
	if (variable.m_type != Tokens::NUMERIC || variable.m_numericValue - floor(variable.m_numericValue) != 0.0) 
	{
		throw ParsingException("Syntax Error: Expecting an integer but [" + variable.toString() + "] was found");
	}
}

void ScriptHelper::checkNonNegativeInteger(const Variable& variable) 
{
	checkInteger(variable);
	if (variable.m_numericValue < 0) 
	{
		throw ParsingException("Expecting a non negative number instead of [" + variable.toString() + "]");
	}
}

vector<Variable> ScriptHelper::getArguments(ParsingScript& script, char start, char end)
{
    vector<Variable> args;

    if (!script.hasNext() || script.currentChar() == Tokens::END_STATEMENT) 
    {
        return args;
    }

    ParsingScript tempScript(script.getData(), script.getPointer());
    string body = ScriptHelper::getBodyBetween(tempScript, start, end);

    while (script.getPointer() < tempScript.getPointer()) 
    {
        Variable item = ScriptHelper::getItem(script);
        args.push_back(item);
    }

    if (script.getPointer() <= tempScript.getPointer()) 
    {
        ScriptHelper::increasePointerIf(script, Tokens::END_ARG);
    }

    ScriptHelper::increasePointerIf(script, Tokens::SPACE);
    return args;
}

string ScriptHelper::getBodyBetween(ParsingScript& script, char open, char close)
{
    string result;
    int braces = 0;

    for (; script.hasNext(); script.increasePointer())
    {
        char ch = script.currentChar();

        if (ch == ' ' && result.empty()) 
        {
            continue;
        }
        else if (ch == open) 
        {
            braces++;
        }
        else if (ch == close) 
        {
            braces--;
        }

        result += ch;
        if (braces == -1)
        {
            if (ch == close) 
            {
                result.erase(result.size() - 1, 1);
            }
            break;
        }
    }

    return result;
}

void ScriptHelper::skipRemainingExpression(ParsingScript& script)
{
    int argRead = 0;
    bool inQuotes = false;
    char previous = Tokens::NULL_CHAR;

    while (script.hasNext()) {
        char currentChar = script.currentChar();
        if (inQuotes && currentChar != Tokens::QUOTE) {
            script.increasePointer();
            continue;
        }

        switch (currentChar)
        {
            case Tokens::QUOTE:
                if (previous != '\\') {
                    inQuotes = !inQuotes;
                }
                break;
            case Tokens::START_ARG:
                argRead++;
                break;
            case Tokens::END_ARG:
                argRead--;
                if (argRead < 0) {
                    return;
                }
                break;
            case Tokens::END_STATEMENT:
            case Tokens::NEXT_ARG:
                return;
            default:
                break;
        }

        script.increasePointer();
        previous = currentChar;
    }
}

string ScriptHelper::isNotSign(const string& data)
{
    return startsWith(data, Tokens::NOT) ? Tokens::NOT : Tokens::EMPTY;
}

string ScriptHelper::getNextToken(ParsingScript& script)
{
    if (!script.hasNext()) 
    {
        return "";
    }

    size_t end = script.find_first_of(Tokens::TOKEN_SEPARATORS, script.getPointer());

    if (end == string::npos) 
    {
        return "";
    }

    string var = script.substr(script.getPointer(), end - script.getPointer());
    script.setPointer(end);
    return var;
}

int ScriptHelper::goToNextStatement(ParsingScript& script)
{
    int endGroupRead = 0;
    while (script.hasNext()) 
    {
        char currentChar = script.currentChar();
        switch (currentChar)
        {
            case Tokens::END_GROUP: endGroupRead++;
                script.increasePointer();
                return endGroupRead;
            case Tokens::START_GROUP:
            case Tokens::QUOTE:
            case Tokens::SPACE:
            case Tokens::END_STATEMENT:
            case Tokens::END_ARG:
                script.increasePointer();
                break;
            default:
                return endGroupRead;
        }
    }
    return endGroupRead;
}

vector<string> ScriptHelper::tokenize(const string& data, const string& delimeters, size_t from, size_t to, bool removeEmpty)
{
    vector<string> args;
    size_t next = from;

    while (next != string::npos && next < to) 
    {
        next = data.find_first_of(delimeters, from);
        if (next == string::npos) 
        {
            if (!removeEmpty && data.size() > 0 && data[data.size() - 1] == delimeters[0]) 
            {
                string arg = data.substr(from);
                args.push_back(arg);
            }
            break;
        }

        string arg = data.substr(from, next - from);
        from = next + 1;

        if (removeEmpty) 
        {
            arg = ScriptHelper::trim(arg);
            if (arg.empty()) 
            {
                continue;
            }
        }

        args.push_back(arg);
    }

    if (from < min(data.size(), to)) 
    {
        string arg = data.substr(from);
        args.push_back(arg);
    }

    return args;
}

Variable ScriptHelper::getItem(ParsingScript& script)
{
    increasePointerIf(script, Tokens::NEXT_ARG, Tokens::SPACE);
    ScriptHelper::checkNotEnd(script, "Incomplete function definition");

    Variable value = Parser::loadAndCalculate(script, Tokens::NEXT_OR_END);
    
    increasePointerIf(script, Tokens::SPACE);
    return value;
}

string ScriptHelper::getToken(ParsingScript& script, const string& to)
{
    char curr = script.tryCurrentChar();
    char prev = script.tryPreviousChar();

    if (to.find_first_of(Tokens::SPACE) == string::npos) 
    {
        // Skip a leading space unless we are inside of quotes
        while (curr == Tokens::SPACE && prev != Tokens::QUOTE) 
        {
            script.increasePointer();
            curr = script.tryCurrentChar();
            prev = script.tryPreviousChar();
        }
    }

    // String in quotes
    bool inQuotes = curr == Tokens::QUOTE;
    if (inQuotes) 
    {
        size_t end = script.find(Tokens::QUOTE, script.getPointer() + 1);
        if (end == string::npos) 
        {
            throw ParsingException("Syntax Error: Unmatched quotes in [" + script.remainingScript() + "]");
        }
        string result = script.substr(script.getPointer() + 1, end - script.getPointer() - 1);
        script.setPointer(end + 1);
        return result;
    }

    // Nothing, empty.
    size_t end = script.find_first_of(to, script.getPointer());
    if (end == string::npos || script.getPointer() >= end) 
    {
        script.increasePointer();
        return Tokens::EMPTY;
    }

    // Skip found characters that have a backslash before.
    while ((end > 0 && script(end - 1) == '\\') && end + 1 < script.size()) 
    {
        end = script.find_first_of(to, end + 1);
    }

    if (end == string::npos) 
    {
        throw ParsingException("Parser Error: Couldn't extract token from [" + script.remainingScript() + "]");
    }

    if (script(end - 1) == Tokens::QUOTE) 
    {
        end--;
    }

    string var = script.substr(script.getPointer(), end - script.getPointer());
    script.setPointer(end);

    ScriptHelper::increasePointerIf(script, Tokens::QUOTE, Tokens::SPACE);

    return var;
}

void ScriptHelper::checkArgsNumber(size_t expected, size_t supplied, const string& name)
{
    if (expected != supplied) 
    {
        throw ParsingException("Syntax Error: Function [" + name + "] arguments mismatch: " + to_string(expected) + " expected, " + to_string(supplied) + " was found");
    }
}

void ScriptHelper::checkNotNull(const string& varName, const void* func)
{
    if (func == 0) 
    {
        throw ParsingException("Parser Error: Variable [" + varName + "] doesn't exist");
    }
}

void ScriptHelper::checkNotEnd(const ParsingScript& script, const string& name)
{
    if (!script.hasNext()) 
    {
        throw ParsingException("Syntax Error: Incomplete arguments for [" + name + "]");
    }
}

string ScriptHelper::convertToScript(const string& rawData, unordered_map<size_t, size_t>& char2Line) 
{
    string result;

    bool inQuotes = false;
    bool spaceOK = false;
    bool inComments = false;
    bool simpleComments = false;
    char previous = Tokens::NULL_CHAR;

    int parentheses = 0;
    int groups = 0;
    size_t lineNumber = 0;
    size_t lastScriptLength = 0;

    string toCheck;

    for (int i = 0; i < rawData.size(); i++)
    {
        char ch = rawData[i];
        char next = i + 1 < rawData.size() ? rawData[i + 1] : Tokens::NULL_CHAR;

        if (ch == '\n') 
        {
            if (result.size() > lastScriptLength) 
            {
                char2Line[result.size() - 1] = lineNumber;
                lastScriptLength = result.size();
            }
            lineNumber++;
        }

        if (inComments && ((simpleComments && ch != '\n') ||
            (!simpleComments && ch != '*'))) 
        {
            continue;
        }

        switch (ch)
        {
            case '/':
                if (inComments || next == '/' || next == '*') 
                {
                    inComments = true;
                    simpleComments = simpleComments || next == '/';
                    continue;
                }
                break;
            case Tokens::QUOTE:
                if (!inComments && previous != '\\') 
                {
                    inQuotes = !inQuotes;
                }
                break;
            case '\r':
            case '\t':
            case ' ':
                if (inQuotes)
                {
                    break; // the char will be added
                }
                else 
                {
                    bool keep = ScriptHelper::keepSpace(result, next);
                    spaceOK = keep || (previous != Tokens::NULL_CHAR &&
                        previous != Tokens::NEXT_ARG && spaceOK);
                    bool spaceOKonce = ScriptHelper::keepSpaceOnce(result, next);
                    if (spaceOK || spaceOKonce) 
                    {
                        break; // the char will be added
                    }
                }
                continue;
            case '\n':
                if (simpleComments) 
                {
                    inComments = simpleComments = false;
                }
                spaceOK = false;
                continue;
            case Tokens::END_ARG:
                if (!inQuotes) 
                {
                    parentheses--;
                    if (parentheses < 0) 
                    {

                    }
                    spaceOK = false;
                }
                break;
            case Tokens::START_ARG:
                if (!inQuotes) 
                {
                    parentheses++;
                }
                break;
            case Tokens::END_GROUP:
                if (!inQuotes) 
                {
                    groups--;
                    spaceOK = false;
                }
                break;
            case Tokens::START_GROUP:
                if (!inQuotes) 
                {
                    groups++;
                }
                break;
            case Tokens::END_STATEMENT:
                if (!inQuotes) 
                {
                    spaceOK = false;
                }
                break;
            default: break;
        }

        if (!inComments) 
        {
            result += ch;
            if (inQuotes && !toCheck.empty()) 
            {
                ScriptHelper::checkSpecialChars(toCheck);
                toCheck.clear();
            }
            else if (!inQuotes) {
                // We don't check whatever is in quotes
                toCheck += ch;
            }
        }
        previous = ch;
    }

    if (inQuotes && !toCheck.empty()) 
    {
        ScriptHelper::checkSpecialChars(toCheck);
    }
    return result;
}

bool ScriptHelper::endsWithFunction(const string& buffer, const vector<string>& functions)
{
    size_t bufSize = buffer.size();
    for (size_t i = 0; i < functions.size(); i++) 
    {
        const string& func = functions[i];
        size_t funcSize = func.size();
        size_t from = bufSize - funcSize;

        if (bufSize >= funcSize && buffer.compare(from, funcSize, func) == 0) 
        {
            char prev = funcSize >= bufSize ? Tokens::END_STATEMENT : buffer[bufSize - funcSize - 1];
            if (ScriptHelper::contains(Tokens::TOKEN_SEPARATORS, prev)) 
            {
                return true;
            }
        }
    }

    return false;
}

bool ScriptHelper::spaceNotNeeded(char next)
{
    return (next == Tokens::SPACE || next == Tokens::START_ARG || next == Tokens::START_GROUP || next == Tokens::NULL_CHAR);
}

bool ScriptHelper::keepSpace(const string& script, char next)
{
    if (spaceNotNeeded(next)) 
    {
        return false;
    }

    return endsWithFunction(script, Tokens::FUNCTION_WITH_SPACE);
}

bool ScriptHelper::keepSpaceOnce(const string& script, char next)
{
    if (spaceNotNeeded(next)) 
    {
        return false;
    }

    return endsWithFunction(script, Tokens::FUNCTION_WITH_SPACE_ONCE);
}

void ScriptHelper::checkSpecialChars(string const& part)
{
    wstring wstr = s2w(part);
    int pos = 0;

    for (wchar_t u : wstr) 
    {
        pos++;
        if (u >= 127 && u <= 187) 
        {
            wstring part = wstr.substr(pos - 1, Tokens::MAX_CHARS_TO_SHOW);
            string excerpt = w2s(part);

            throw ParsingException("Illegal character [" + string(1, u) +
                " (" + to_string(u) + "," + toHex(u) + ")" +
                "] at position " + to_string(pos) +
                ": " + excerpt +
                "...");
        }
    }
}

wstring ScriptHelper::s2w(const string& str)
{
    if (str.empty()) 
    {
        return L"";
    }
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    try 
    {
        wstring dest = converter.from_bytes(str);
        return dest;
    }
    catch (...) 
    {
        return L"";
    }

}

string ScriptHelper::w2s(const wstring& wstr)
{
    if (wstr.empty()) 
    {
        return "";
    }
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    try 
    {
        string dest = converter.to_bytes(wstr);
        return dest;
    }
    catch (...) 
    {
        return "";
    }
}

string ScriptHelper::toUpper(const string& str)
{
    string upper;
    transform(str.begin(), str.end(), std::back_inserter(upper), ::toupper);
    return upper;
}

string ScriptHelper::toHex(int i)
{
    stringstream stream;
    stream << ((i < 16) ? "0" : "") << ((i < 256) ? "0" : "") << ((i < 4096) ? "0" : "");
    stream << hex << i;
    return "0x" + toUpper(stream.str());
}

string ScriptHelper::trim(string const& str)
{
    size_t first = str.find_first_not_of(Tokens::WHITESPACE);

    if (first == string::npos) 
    {
        return "";
    }

    size_t last = str.find_last_not_of(Tokens::WHITESPACE);

    return str.substr(first, last - first + 1);
}

//Reads the scriptfile and returns it as string
string ScriptHelper::readScriptFile(const string& path) 
{
	ifstream fileStream(path);

	if (fileStream.fail()) 
	{
		throw std::invalid_argument{ "Could not read file from path: '" + path + "'" };
	}
	fileStream.ignore(numeric_limits<streamsize>::max());
	streamsize size = fileStream.gcount();
	fileStream.clear();
	fileStream.seekg(0, fileStream.beg);
	string fileContent(size, ' ');
	fileStream.read(&fileContent[0], size);
	return fileContent;
}