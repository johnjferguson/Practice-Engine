#pragma once
#include "LeanWindows.h"
#include <unordered_map>
#include <string>

class MessageMap
{
public:
	MessageMap();
	std::wstring operator() (DWORD msg, LPARAM lp, WPARAM wp) const;
private:
	std::unordered_map<int, std::wstring> map;
	// length for formating message ouput
	unsigned max_length;
};
