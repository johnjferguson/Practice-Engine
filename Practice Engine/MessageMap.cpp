#include "MessageMap.h"
#include <fstream>
#include <sstream>
#include <iomanip>

MessageMap::MessageMap()
	:
	max_length(0)
{
	std::wfstream file("WindowsMessagesList.txt");
	DWORD dec;
	std::wstring hex;
	std::wstring symbolic;
	while (file >> hex >> dec >> symbolic)
	{
		if (symbolic.length() > max_length)
			max_length = unsigned int(symbolic.length());

		map.insert({ dec, symbolic });
	}
}

std::wstring MessageMap::operator()(DWORD msg, LPARAM lp, WPARAM wp) const
{
	std::wstringstream ss;
	auto it = map.find(msg);
	if (it != map.end())

		ss << std::left << std::setw(max_length) << it->second << std::right
		   << L" LP: 0x" << std::setfill(L'0') << std::setw(sizeof(LPARAM) * 2) << std::hex << lp
		   << L" WP: 0x" << std::setfill(L'0') << std::setw(sizeof(WPARAM) * 2) << std::hex << wp
		   << std::endl;
	else
		ss << std::left << std::setw(max_length) << "Unknown Message" << std::right
		   << L" LP: 0x" << std::setfill(L'0') << std::setw(sizeof(LPARAM) * 2) << std::hex << lp
		   << L" WP: 0x" << std::setfill(L'0') << std::setw(sizeof(WPARAM) * 2) << std::hex << wp
		   << std::endl;
	return ss.str();
}
