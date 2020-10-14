#pragma once
#include "LeanWindows.h"
#include <string>
#include <sstream>
#include <comdef.h>

namespace Practice
{
	inline std::wstring LineAndFile()
	{
		std::wstringstream ss;
		ss << "[LINE]: " << __LINE__ << std::endl << "[FILE]: " << __FILE__ << std::endl;
		return ss.str();
	}

	inline std::wstring TranslateHResult(HRESULT hr)
	{
		std::wstringstream ss;
		ss << "[HRESULT]: " << hr << _com_error(hr).ErrorMessage() << std::endl;
		return ss.str();
	}

	inline std::wstring LastError()
	{
		return _com_error(GetLastError()).ErrorMessage();
	}

	inline void MessageBox(std::wstring message, std::wstring title)
	{
		::MessageBox(nullptr, message.c_str(), title.c_str(), MB_ICONWARNING);
	}

	template <class T>
	inline void DebugPrint(const std::wstring& str, T i)
	{
		std::wstringstream wss;
		wss << str << " : " << i << std::endl;
		OutputDebugString(wss.str().c_str());
	}

}
