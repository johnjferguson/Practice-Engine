#pragma once
#include <comdef.h>
#include <sstream>
#include <string>
#include <vector>
#include "Debug.h"
#include "DxgiInfoManager.h"

#if _DEBUG
#define ASSERT_LAST_ERROR() \
	if (GetLastError() == S_OK) {} \
	else \
	{ \
		Practice::MessageBox(Practice::LineAndFile() + Practice::LastError(), L"ASSERT LAST ERROR"); \
		DebugBreak(); \
	} \
	while(0)

#define ASSERT_HR(hresult) \
	{ \
		HRESULT hr = hresult; \
		if (SUCCEEDED(hr)) {} \
		else \
		{ \
			Practice::MessageBox(Practice::LineAndFile() + Practice::TranslateHResult(hr), L"ASSERT HRESULT"); \
			DebugBreak(); \
		} \
	} \
	while(0)

#define ASSERT_HR_INFO(hresult) \
	{ \
		DxgiInfoManager* dim = DxgiInfoManager::Instance(); \
		dim->Set(); \
		HRESULT hr = hresult; \
		std::wstring str = dim->GetConcatenatedMessages(); \
		if (SUCCEEDED(hr) && str.length() == 0) {} \
		else \
		{ \
			Practice::MessageBox(Practice::LineAndFile() + Practice::TranslateHResult(hr) + str, L"ASSERT HRESULT INFOQUEUE"); \
			DebugBreak(); \
		} \
	} \
	while(0)

#define ASSERT_INFO(func) \
	{ \
		DxgiInfoManager* dim = DxgiInfoManager::Instance(); \
		dim->Set(); \
		func; \
		std::wstring str = dim->GetConcatenatedMessages(); \
		if (str.length() == 0) {} \
		else \
		{ \
			Practice::MessageBox(Practice::LineAndFile() + str, L"ASSERT INFOQUEUE"); \
			DebugBreak(); \
		} \
	} \
	while(0)

#else

#define ASSERT_LAST_ERROR()
#define ASSERT_HR(hresult) hresult
#define ASSERT_HR_INFO(hresult) hresult
#define ASSERT_INFO(func) func

#endif
