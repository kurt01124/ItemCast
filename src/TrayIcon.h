#pragma once

#include <Windows.h>
#include <shellapi.h>
#include <functional>

class TrayIcon {
public:
    using ShowCallback = std::function<void()>;
    using ToggleCallback = std::function<void()>;
    using ExitCallback = std::function<void()>;

    TrayIcon();
    ~TrayIcon();

    // 트레이 아이콘 생성/제거
    bool create(HWND parentHwnd, UINT callbackMessage);
    void remove();

    // 아이콘 업데이트
    void setEnabled(bool enabled);
    void setTooltip(const wchar_t* tooltip);

    // 콜백 설정
    void setShowCallback(ShowCallback callback) { m_showCallback = callback; }
    void setToggleCallback(ToggleCallback callback) { m_toggleCallback = callback; }
    void setExitCallback(ExitCallback callback) { m_exitCallback = callback; }

    // 메시지 처리
    void handleMessage(WPARAM wParam, LPARAM lParam);

private:
    void showContextMenu();

    HWND m_parentHwnd;
    NOTIFYICONDATAW m_nid;
    bool m_created;
    bool m_enabled;

    ShowCallback m_showCallback;
    ToggleCallback m_toggleCallback;
    ExitCallback m_exitCallback;
};
