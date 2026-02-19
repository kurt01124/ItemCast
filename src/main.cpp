#include <Windows.h>
#include "App.h"

// 단일 인스턴스 확인용 뮤텍스
static const wchar_t* MUTEX_NAME = L"ItemCast_SingleInstance_Mutex";

int WINAPI wWinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    UNREFERENCED_PARAMETER(nCmdShow);

    // 단일 인스턴스 확인
    HANDLE hMutex = CreateMutexW(nullptr, TRUE, MUTEX_NAME);
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        // 이미 실행 중인 인스턴스가 있음
        // 기존 창 찾아서 활성화
        HWND existingWnd = FindWindowW(L"ItemCastMainWindow", nullptr);
        if (existingWnd) {
            ShowWindow(existingWnd, SW_SHOW);
            SetForegroundWindow(existingWnd);
        }

        if (hMutex) {
            CloseHandle(hMutex);
        }
        return 0;
    }

    // 애플리케이션 생성 및 실행
    App app;
    if (!app.initialize(hInstance)) {
        if (hMutex) {
            CloseHandle(hMutex);
        }
        return 1;
    }

    int result = app.run();

    // 뮤텍스 해제
    if (hMutex) {
        ReleaseMutex(hMutex);
        CloseHandle(hMutex);
    }

    return result;
}
