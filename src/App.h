#pragma once

#include <Windows.h>
#include <memory>
#include "KeyMapper.h"
#include "KeyboardHook.h"
#include "Config.h"
#include "MainWindow.h"
#include "TrayIcon.h"

class App {
public:
    App();
    ~App();

    // 애플리케이션 초기화 및 실행
    bool initialize(HINSTANCE hInstance);
    int run();
    void quit();

    // 싱글톤 인스턴스
    static App* getInstance() { return s_instance; }

private:
    // 콜백 핸들러
    void onEnableChanged(bool enabled);
    void onKeyChanged(ItemSlot slot, DWORD vkCode);
    void onTrayShow();
    void onTrayToggle();
    void onTrayExit();
    void onToggleEnable();
    void onKeyInput(DWORD vkCode, bool keyDown);

    // 설정 적용
    void applyConfig();
    void updateUI();

    HINSTANCE m_hInstance;

    // 컴포넌트들
    std::unique_ptr<KeyMapper> m_keyMapper;
    std::unique_ptr<KeyboardHook> m_keyboardHook;
    std::unique_ptr<Config> m_config;
    std::unique_ptr<MainWindow> m_mainWindow;
    std::unique_ptr<TrayIcon> m_trayIcon;

    bool m_running;

    static App* s_instance;
};
