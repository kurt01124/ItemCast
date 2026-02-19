#include "KeyboardHook.h"
#include <algorithm>

KeyboardHook* KeyboardHook::s_instance = nullptr;

KeyboardHook::KeyboardHook()
    : m_hookHandle(nullptr)
    , m_enabled(false)
    , m_warcraft3Only(true)
    , m_keyWaitMode(false)
    , m_mapper(nullptr)
    , m_keyCallback(nullptr)
{
    s_instance = this;
}

KeyboardHook::~KeyboardHook() {
    uninstall();
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

bool KeyboardHook::install() {
    if (m_hookHandle != nullptr) {
        return true;  // 이미 설치됨
    }

    m_hookHandle = SetWindowsHookExW(
        WH_KEYBOARD_LL,
        lowLevelKeyboardProc,
        GetModuleHandleW(nullptr),
        0
    );

    return m_hookHandle != nullptr;
}

void KeyboardHook::uninstall() {
    if (m_hookHandle != nullptr) {
        UnhookWindowsHookEx(m_hookHandle);
        m_hookHandle = nullptr;
    }
}

bool KeyboardHook::isWarcraft3Active() {
    HWND foreground = GetForegroundWindow();
    if (foreground == nullptr) {
        return false;
    }

    wchar_t className[256] = { 0 };
    wchar_t windowTitle[256] = { 0 };

    GetClassNameW(foreground, className, 256);
    GetWindowTextW(foreground, windowTitle, 256);

    // 워크래프트3 클래스 이름 또는 창 제목으로 감지
    // Warcraft III 클래스: "Warcraft III"
    // Warcraft III Reforged 클래스: "OsWindow"
    std::wstring classStr(className);
    std::wstring titleStr(windowTitle);

    // 대소문자 무시 비교를 위해 소문자로 변환
    std::transform(classStr.begin(), classStr.end(), classStr.begin(), ::towlower);
    std::transform(titleStr.begin(), titleStr.end(), titleStr.begin(), ::towlower);

    // Warcraft III 감지
    if (classStr.find(L"warcraft") != std::wstring::npos) {
        return true;
    }
    if (titleStr.find(L"warcraft") != std::wstring::npos) {
        return true;
    }
    // war3.exe 창 제목
    if (titleStr.find(L"war3") != std::wstring::npos) {
        return true;
    }

    return false;
}

LRESULT CALLBACK KeyboardHook::lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0 || s_instance == nullptr) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    KBDLLHOOKSTRUCT* kbStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
    DWORD vkCode = kbStruct->vkCode;
    bool keyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
    bool keyUp = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

    // 키 대기 모드일 때만 콜백 호출 (키 설정 UI용)
    if (s_instance->m_keyWaitMode && s_instance->m_keyCallback && keyDown) {
        // Escape는 취소로 처리
        if (vkCode != VK_ESCAPE) {
            s_instance->m_keyCallback(vkCode, keyDown);
        } else {
            s_instance->m_keyCallback(0, keyDown);  // 0은 취소 의미
        }
        return 1;  // 키 입력 차단
    }

    // 활성화 상태 확인
    if (!s_instance->m_enabled) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // 키 매퍼 확인
    if (s_instance->m_mapper == nullptr) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // 매핑된 키인지 확인
    if (!s_instance->m_mapper->hasMapping(vkCode)) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // 우리가 보낸 키인지 확인 (무한 루프 방지)
    if (kbStruct->flags & LLKHF_INJECTED) {
        return CallNextHookEx(nullptr, nCode, wParam, lParam);
    }

    // 키 변환
    DWORD targetKey = s_instance->m_mapper->translate(vkCode);

    // 변환된 키 전송
    if (keyDown || keyUp) {
        sendKey(targetKey, keyDown);
        return 1;  // 원래 키 입력 차단
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void KeyboardHook::sendKey(DWORD vkCode, bool keyDown) {
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = static_cast<WORD>(vkCode);
    input.ki.wScan = static_cast<WORD>(MapVirtualKeyW(vkCode, MAPVK_VK_TO_VSC));
    input.ki.dwFlags = keyDown ? 0 : KEYEVENTF_KEYUP;

    // 확장 키 플래그 (Numpad 키는 확장 키가 아님)
    // 단, Numpad Enter나 Numpad / 같은 일부 키는 확장 키

    SendInput(1, &input, sizeof(INPUT));
}
