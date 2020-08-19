#pragma once
#include <comdef.h>
#include <sstream>
#include <string>
#include <vector>

#if _DEBUG

#define ASSERT_LAST_ERROR() \
	if (GetLastError() == S_OK) {} \
	else \
	{ \
		std::wstringstream wss; \
		wss << "File: " << __FILE__ << std::endl << "Line: " << __LINE__ << std::endl << "Description: " << _com_error(GetLastError()).ErrorMessage(); \
		MessageBox(nullptr, wss.str().c_str(), L"ASSERT_HWND", MB_ICONWARNING ); \
		DebugBreak(); \
	} \
	while(0)

#define ASSERT_HRESULT(hresult) \
	{ \
		HRESULT hr = hresult; \
		if (SUCCEEDED(hr)) {} \
		else \
		{ \
			std::wstringstream wss; \
			wss << "File: " << __FILE__ << std::endl << "Line: " << __LINE__ << std::endl << "Description: " << _com_error(hr).ErrorMessage(); \
			MessageBox(nullptr, wss.str().c_str(), L"ASSERT_HRESULT", MB_ICONWARNING ); \
			DebugBreak(); \
		} \
	} \
	while(0)

#define ASSERT_HRESULT_INFOQUEUE(hresult) \
	{ \
			infoManager.Set(); \
			HRESULT hr = hresult; \
			std::vector<std::wstring> messages = infoManager.GetMessages(); \
			if (SUCCEEDED(hr)) {} \
			else \
			{ \
				std::wstringstream wss; \
				wss << "File: " << __FILE__ << std::endl << "Line: " << __LINE__ << std::endl << "Description: "; \
				if (hr == DXGI_ERROR_DEVICE_REMOVED) \
				{ \
				wss << _com_error(pDevice->GetDeviceRemovedReason()).ErrorMessage() << std::endl; \
				} \
				else \
				{ \
				wss << _com_error(hr).ErrorMessage() << std::endl; \
				} \
				wss << "InfoQueue: " << std::endl; \
				for (auto it = messages.begin(); it != messages.end(); it++) \
				{ \
					wss << *it << std::endl; \
				} \
				MessageBox(nullptr, wss.str().c_str(), L"ASSERT_INFOQUEUE", MB_ICONWARNING ); \
				DebugBreak(); \
			} \
	} \
	while(0)

#define ASSERT_INFOQUEUE(func) \
	{ \
		infoManager.Set(); \
		func; \
		std::vector<std::wstring> messages = infoManager.GetMessages(); \
		if (messages.size() == 0) {} \
		else \
		{ \
			std::wstringstream wss; \
			wss << "File: " << __FILE__ << std::endl << "Line: " << __LINE__ << std::endl << "Description: "; \
			wss << "InfoQueue: " << std::endl; \
			for (auto it = messages.begin(); it != messages.end(); it++) \
			{ \
				wss << *it << std::endl; \
			} \
			MessageBox(nullptr, wss.str().c_str(), L"ASSERT_INFOQUEUE", MB_ICONWARNING ); \
			DebugBreak(); \
		} \
	} \
	while(0)

	

#else

#define ASSERT_LAST_ERROR()
#define ASSERT_HRESULT(hr) hr
#define ASSERT_HRESULT_INFOQUEUE(hresult) hresult
#define ASSERT_INFOQUEUE(func) func

#endif

template <class T>
void PrintDebug(const std::wstring& str, T i)
{
	std::wstringstream wss;
	wss << str << " : " << (unsigned long long)i << std::endl;
	OutputDebugString(wss.str().c_str());
}