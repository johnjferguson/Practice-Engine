#include "DxgiInfoManager.h"
#include "Graphics.h"
#include "Assert.h"
#include <algorithm>

#pragma comment(lib, "dxguid.lib")

DxgiInfoManager DxgiInfoManager::infoManager;

DxgiInfoManager::DxgiInfoManager()
{
    typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

    const auto hModDxgiDebug = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    if (hModDxgiDebug == nullptr)
    {
        ASSERT_LAST_ERROR();
    }

    const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
        reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface"))
        );

    if (DxgiGetDebugInterface == nullptr)
    {
        ASSERT_LAST_ERROR();
    }
    DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue),&pDxgiInfoQueue);
}

DxgiInfoManager* DxgiInfoManager::Instance()
{
    return &infoManager;
}

void DxgiInfoManager::Set()
{
    int i = 1;
    next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
    int j = 0;
}

std::vector<std::wstring> DxgiInfoManager::GetMessages() const
{
    std::vector<std::wstring> messages;

    const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
    for (auto i = next; i < end; i++)
    {
        SIZE_T messageLength = SIZE_T(0);
        pDxgiInfoQueue->GetMessageW(DXGI_DEBUG_ALL, i, nullptr, &messageLength);
        DXGI_INFO_QUEUE_MESSAGE* pMessage = (DXGI_INFO_QUEUE_MESSAGE*)malloc(messageLength);
        pDxgiInfoQueue->GetMessageW(DXGI_DEBUG_ALL, i, pMessage, &messageLength);
        int wchars_num = MultiByteToWideChar(CP_UTF8, 0, pMessage->pDescription, -1, nullptr, 0);
        wchar_t* wstr = new wchar_t[wchars_num];
        MultiByteToWideChar(CP_UTF8, 0, pMessage->pDescription, -1, wstr, wchars_num);
        messages.emplace_back(std::wstring(wstr));
        delete[] wstr;
    }


    return messages;
}

std::wstring DxgiInfoManager::GetConcatenatedMessages() const
{
    std::vector<std::wstring> messages = GetMessages();
    std::wstring message;
    std::for_each(messages.begin(), messages.end(), [&](const std::wstring& msg) { message = msg + L"\n"; });

    return std::move(message);
}
