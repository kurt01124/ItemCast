#pragma once

#include <Windows.h>
#include <string>
#include <functional>
#include "KeyMapper.h"

class KeyboardHook {
public:
    using KeyCallback = std::function<void(DWORD vkCode, bool keyDown)>;

    KeyboardHook();
    ~KeyboardHook();

    // 후킹 설치/해제
    bool install();
    void uninstall();
    bool isInstalled() const { return m_hookHandle != nullptr; }

    // 활성화 상태
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    // 키 매퍼 설정
    void setKeyMapper(KeyMapper* mapper) { m_mapper = mapper; }
    KeyMapper* getKeyMapper() const { return m_mapper; }

    // 워크래프트3 전용 모드
    void setWarcraft3Only(bool enabled) { m_warcraft3Only = enabled; }
    bool isWarcraft3Only() const { return m_warcraft3Only; }

    // 키 입력 콜백 (키 설정 UI용)
    void setKeyCallback(KeyCallback callback) { m_keyCallback = callback; }
    void clearKeyCallback() { m_keyCallback = nullptr; }

    // 키 대기 모드 (이 모드일 때만 콜백 호출)
    void setKeyWaitMode(bool waiting) { m_keyWaitMode = waiting; }
    bool isKeyWaitMode() const { return m_keyWaitMode; }

    // 워크래프트3 창 감지
    static bool isWarcraft3Active();

private:
    static LRESULT CALLBACK lowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    static void sendKey(DWORD vkCode, bool keyDown);

    HHOOK m_hookHandle;
    bool m_enabled;
    bool m_warcraft3Only;
    bool m_keyWaitMode;
    KeyMapper* m_mapper;
    KeyCallback m_keyCallback;

    // 싱글톤 인스턴스 (콜백에서 접근용)
    static KeyboardHook* s_instance;
};
