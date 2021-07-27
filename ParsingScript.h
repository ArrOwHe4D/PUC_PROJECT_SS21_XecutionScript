//NOTE: project was based on https://github.com/vassilych/cscscpp

#pragma once

#include "Tokens.h"
#include "Variable.h"


class ParsingScript
{
public:
	ParsingScript(const string& data, size_t from = 0) : m_data(data), m_currentPosition(from) {}

	ParsingScript(const ParsingScript& other) :
		m_data(other.m_data),
		m_currentPosition(other.m_currentPosition),
		m_filename(other.m_filename),
		m_rawScript(other.m_rawScript),
		m_scriptOffset(other.m_scriptOffset),
		m_char2Line(other.getChar2Line())
	{

	}

	inline size_t size() const			 { return m_data.size(); }
	inline bool hasNext() const			 { return m_currentPosition < m_data.size(); }
	inline size_t getPointer() const	 { return m_currentPosition; }
	inline const string& getData() const { return m_data; }

	inline size_t find(char ch, size_t fromDelta = 0) const { return m_data.find(ch, fromDelta); }

	inline size_t find_first_of(const string& str, size_t fromDelta = 0) const
	{
		return m_data.find_first_of(str, fromDelta);
	}

	inline string substr(size_t from, size_t len = string::npos) const
	{
		return m_data.substr(from, len);
	}

	inline char operator()(size_t i) const	{ return m_data[i]; }

	inline char currentChar() const			{ return m_data[m_currentPosition]; }
	inline char currentCharAndIncreasePointer()		{ return m_data[m_currentPosition++]; }

	inline char tryCurrentChar() const		{ return m_currentPosition < m_data.size() ? m_data[m_currentPosition] : Tokens::NULL_CHAR; }
	inline char tryNextChar() const			{ return m_currentPosition + 1 < m_data.size() ? m_data[m_currentPosition + 1] : Tokens::NULL_CHAR; }
	inline char tryPreviousChar() const		{ return m_currentPosition >= 1 ? m_data[m_currentPosition - 1] : Tokens::NULL_CHAR; }
	inline char tryPrePreviousChar() const	{ return m_currentPosition >= 2 ? m_data[m_currentPosition - 2] : Tokens::NULL_CHAR; }

	inline string remainingScript(size_t maxChars = Tokens::MAX_CHARS_TO_SHOW) const { return m_currentPosition < m_data.size() ? m_data.substr(m_currentPosition, maxChars) : ""; }

	inline void setChar2Line(const unordered_map<size_t, size_t>& char2Line) { m_char2Line = char2Line; }

	inline unordered_map<size_t, size_t>& getChar2Line() const { return m_char2Line; }

	inline void setOffset(size_t offset) { m_scriptOffset = offset; }

	inline void setFilename(const string& filename) { m_filename = filename; }
	inline const string& getFilename() const		{ return m_filename; }

	inline void setRawScript(const string& script) { m_rawScript = script; }
	inline const string& getRawScript() { return m_rawScript; }

	inline void setPointer(size_t ptr)  { m_currentPosition = ptr; }
	inline void increasePointer(size_t to = 1)  { m_currentPosition += to; }
	inline void backward(size_t to = 1) { if (m_currentPosition >= to) m_currentPosition -= to; }

	Variable execute(const string& to = Tokens::END_PARSING_STR);
	Variable executeFrom(size_t index, const string& to = Tokens::END_PARSING_STR);
	Variable executeAll(const string& to = Tokens::END_PARSING_STR);

	string getRawLine(size_t& lineNumber) const;
	size_t getRawLineNumber() const;

	template<class K, class V>
	static vector<K> getKeys(const unordered_map<K, V>& map);

private:
	string m_data; //contains the complete script as string
	size_t m_currentPosition; //pointer to the script

	string m_filename; //filename that contains the script
	string m_rawScript; // original raw script
	size_t m_scriptOffset = 0; // used in functiond defined in bigger scripts
	mutable unordered_map<size_t, size_t> m_char2Line;
	mutable unordered_map<size_t, size_t>* m_char2LinePtr = nullptr;
};

