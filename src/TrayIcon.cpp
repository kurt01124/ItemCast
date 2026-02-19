#include "TrayIcon.h"
#include "resource.h"

TrayIcon::TrayIcon()
    : m_parentHwnd(nullptr)
    , m_created(false)
    , m_enabled(true)
{
    ZeroMemory(&m_nid, sizeof(m_nid));
}

TrayIcon::~TrayIcon() {
    remove();
}

bool TrayIcon::create(HWND parentHwnd, UINT callbackMessage) {
    if (m_created) return true;

    m_parentHwnd = parentHwnd;

    m_nid.cbSize = sizeof(NOTIFYICONDATAW);
    m_nid.hWnd = parentHwnd;
    m_nid.uID = 1;
    m_nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    m_nid.uCallbackMessage = callbackMessage;

    // 앱 아이콘 사용
    m_nid.hIcon = LoadIconW(GetModuleHandleW(nullptr), MAKEINTRESOURCEW(IDI_APP_ICON));
    if (!m_nid.hIcon) m_nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);

    // 툴팁 설정
    wcscpy_s(m_nid.szTip, L"ItemCast - 활성화됨");

    if (Shell_NotifyIconW(NIM_ADD, &m_nid)) {
        m_created = true;
        return true;
    }

    return false;
}

void TrayIcon::remove() {
    if (m_created) {
        Shell_NotifyIconW(NIM_DELETE, &m_nid);
        m_created = false;
    }
}

void TrayIcon::setEnabled(bool enabled) {
    m_enabled = enabled;

    if (m_created) {
        wcscpy_s(m_nid.szTip, enabled ? L"ItemCast - 활성화됨" : L"ItemCast - 비활성화됨");
        Shell_NotifyIconW(NIM_MODIFY, &m_nid);
    }
}

void TrayIcon::setTooltip(const wchar_t* tooltip) {
    if (m_created) {
        wcscpy_s(m_nid.szTip, tooltip);
        Shell_NotifyIconW(NIM_MODIFY, &m_nid);
    }
}

void TrayIcon::handleMessage(WPARAM /* wParam */, LPARAM lParam) {
    switch (LOWORD(lParam)) {
        case WM_LBUTTONDBLCLK:
            // 더블 클릭 - 창 표시
            if (m_showCallback) {
                m_showCallback();
            }
            break;

        case WM_RBUTTONUP:
            // 우클릭 - 컨텍스트 메뉴
            showContextMenu();
            break;
    }
}

void TrayIcon::showContextMenu() {
    HMENU hMenu = CreatePopupMenu();
    if (!hMenu) return;

    // 메뉴 항목 추가
    AppendMenuW(hMenu, MF_STRING, IDM_TRAY_SHOW, L"설정 열기");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING | (m_enabled ? MF_CHECKED : 0), IDM_TRAY_ENABLE, L"활성화");
    AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);
    AppendMenuW(hMenu, MF_STRING, IDM_TRAY_EXIT, L"종료");

    // 메뉴 표시 위치
    POINT pt;
    GetCursorPos(&pt);

    // 포그라운드 윈도우 설정 (메뉴가 제대로 닫히도록)
    SetForegroundWindow(m_parentHwnd);

    // 메뉴 표시
    UINT cmd = TrackPopupMenu(
        hMenu,
        TPM_RETURNCMD | TPM_RIGHTBUTTON,
        pt.x, pt.y,
        0,
        m_parentHwnd,
        nullptr
    );

    DestroyMenu(hMenu);

    // 메뉴 선택 처리
    switch (cmd) {
        case IDM_TRAY_SHOW:
            if (m_showCallback) {
                m_showCallback();
            }
            break;

        case IDM_TRAY_ENABLE:
            if (m_toggleCallback) {
                m_toggleCallback();
            }
            break;

        case IDM_TRAY_EXIT:
            if (m_exitCallback) {
                m_exitCallback();
            }
            break;
    }
}
