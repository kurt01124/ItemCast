#include "MainWindow.h"
#include "resource.h"
#include <CommCtrl.h>

#pragma comment(lib, "comctl32.lib")

const wchar_t* MainWindow::WINDOW_CLASS = L"ItemCastMainWindow";
const wchar_t* MainWindow::WINDOW_TITLE = L"ItemCast - 키 리맵퍼";

// 워크래프트 테마 색상
namespace Theme {
    const COLORREF BG_COLOR = RGB(26, 26, 16);          // 어두운 갈색 배경
    const COLORREF BTN_FILL = RGB(45, 37, 25);          // 버튼 내부
    const COLORREF BTN_BORDER = RGB(200, 168, 78);      // 금색 테두리
    const COLORREF BTN_HOVER = RGB(232, 200, 78);       // 밝은 금색 (호버)
    const COLORREF TEXT_COLOR = RGB(232, 213, 163);     // 밝은 금색/베이지 텍스트
    const COLORREF LABEL_GRAY = RGB(153, 153, 153);     // 슬롯 레이블 회색
}

MainWindow::MainWindow()
    : m_hwnd(nullptr)
    , m_hInstance(nullptr)
    , m_font(nullptr)
    , m_titleFont(nullptr)
    , m_bgBrush(nullptr)
    , m_btnBrush(nullptr)
    , m_textBrush(nullptr)
    , m_btnBorderPen(nullptr)
    , m_btnHoverPen(nullptr)
    , m_colorText(Theme::TEXT_COLOR)
    , m_colorLabelGray(Theme::LABEL_GRAY)
    , m_hoveredBtn(-1)
    , m_checkEnable(nullptr)
    , m_checkWar3Only(nullptr)
    , m_btnSave(nullptr)
    , m_btnTray(nullptr)
    , m_waitingForKey(false)
    , m_waitingSlot(ItemSlot::Slot1)
    , m_waitingForToggleKey(false)
    , m_toggleKeyVk(VK_F5)
    , m_btnToggleKey(nullptr)
    , m_labelToggleKey(nullptr)
{
    for (int i = 0; i < 6; ++i) {
        m_btnSlots[i] = nullptr;
        m_labelSlots[i] = nullptr;
        m_slotKeys[i] = 0;
    }

    // GDI 객체 생성
    m_bgBrush = CreateSolidBrush(Theme::BG_COLOR);
    m_btnBrush = CreateSolidBrush(Theme::BTN_FILL);
    m_textBrush = CreateSolidBrush(Theme::BG_COLOR);
    m_btnBorderPen = CreatePen(PS_SOLID, 2, Theme::BTN_BORDER);
    m_btnHoverPen = CreatePen(PS_SOLID, 2, Theme::BTN_HOVER);
}

MainWindow::~MainWindow() {
    if (m_font) {
        DeleteObject(m_font);
    }
    if (m_titleFont) {
        DeleteObject(m_titleFont);
    }
    if (m_bgBrush) {
        DeleteObject(m_bgBrush);
    }
    if (m_btnBrush) {
        DeleteObject(m_btnBrush);
    }
    if (m_textBrush) {
        DeleteObject(m_textBrush);
    }
    if (m_btnBorderPen) {
        DeleteObject(m_btnBorderPen);
    }
    if (m_btnHoverPen) {
        DeleteObject(m_btnHoverPen);
    }
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
}

bool MainWindow::create(HINSTANCE hInstance) {
    m_hInstance = hInstance;

    // Common Controls 초기화
    INITCOMMONCONTROLSEX icc = { sizeof(icc), ICC_STANDARD_CLASSES };
    InitCommonControlsEx(&icc);

    // 윈도우 클래스 등록
    WNDCLASSEXW wc = { 0 };
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = m_bgBrush;  // 커스텀 배경 브러시 사용
    wc.lpszClassName = WINDOW_CLASS;
    wc.hIcon = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON), IMAGE_ICON,
        GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
    wc.hIconSm = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON), IMAGE_ICON,
        GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    if (!wc.hIcon) wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    if (!wc.hIconSm) wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);

    if (!RegisterClassExW(&wc)) {
        return false;
    }

    // 윈도우 크기 계산
    int width = 320;
    int height = 400;

    // 화면 중앙 위치
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    // 윈도우 생성
    m_hwnd = CreateWindowExW(
        0,
        WINDOW_CLASS,
        WINDOW_TITLE,
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
        x, y, width, height,
        nullptr, nullptr, hInstance, this
    );

    if (!m_hwnd) {
        return false;
    }

    // 아이콘 직접 설정
    HICON hIconBig = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON),
        IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), 0);
    HICON hIconSmall = (HICON)LoadImageW(hInstance, MAKEINTRESOURCEW(IDI_APP_ICON),
        IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), 0);
    if (hIconBig) SendMessageW(m_hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
    if (hIconSmall) SendMessageW(m_hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);

    return true;
}

void MainWindow::show() {
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    SetForegroundWindow(m_hwnd);
}

void MainWindow::hide() {
    ShowWindow(m_hwnd, SW_HIDE);
}

bool MainWindow::isVisible() const {
    return IsWindowVisible(m_hwnd) != FALSE;
}

LRESULT CALLBACK MainWindow::windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    MainWindow* pThis = nullptr;

    if (msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = static_cast<MainWindow*>(pCreate->lpCreateParams);
        SetWindowLongPtrW(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        pThis->m_hwnd = hwnd;
    } else {
        pThis = reinterpret_cast<MainWindow*>(GetWindowLongPtrW(hwnd, GWLP_USERDATA));
    }

    if (pThis) {
        return pThis->handleMessage(msg, wParam, lParam);
    }

    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

LRESULT MainWindow::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE:
            createControls();
            // 토글 핫키 등록
            RegisterHotKey(m_hwnd, IDH_TOGGLE_ENABLE, 0, m_toggleKeyVk);
            return 0;

        case WM_ERASEBKGND: {
            // 배경 그리기
            HDC hdc = (HDC)wParam;
            RECT rc;
            GetClientRect(m_hwnd, &rc);
            FillRect(hdc, &rc, m_bgBrush);
            return 1;
        }

        case WM_CTLCOLORSTATIC: {
            // Static 컨트롤 색상 설정
            HDC hdc = (HDC)wParam;
            HWND hwndCtrl = (HWND)lParam;

            // 제목인지 확인
            bool isTitle = (GetDlgCtrlID(hwndCtrl) == IDC_STATIC_TITLE);

            // 슬롯 레이블인지 확인
            int ctrlId = GetDlgCtrlID(hwndCtrl);
            bool isSlotLabel = (ctrlId >= IDC_LABEL_SLOT1 && ctrlId <= IDC_LABEL_SLOT6);

            SetBkMode(hdc, TRANSPARENT);
            if (isTitle) {
                SetTextColor(hdc, m_colorText);
            } else if (isSlotLabel) {
                SetTextColor(hdc, m_colorLabelGray);
            } else {
                SetTextColor(hdc, m_colorText);
            }
            return (LRESULT)m_textBrush;
        }

        case WM_CTLCOLORBTN: {
            // 체크박스 색상
            HDC hdc = (HDC)wParam;
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, m_colorText);
            return (LRESULT)m_textBrush;
        }

        case WM_DRAWITEM: {
            // Owner-draw 버튼 그리기
            DRAWITEMSTRUCT* dis = (DRAWITEMSTRUCT*)lParam;
            if (dis->CtlType == ODT_BUTTON) {
                HDC hdc = dis->hDC;
                RECT rc = dis->rcItem;

                // 버튼 배경 채우기
                FillRect(hdc, &rc, m_btnBrush);

                // 테두리 그리기
                HPEN oldPen = (HPEN)SelectObject(hdc, m_btnBorderPen);
                HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

                // 포커스나 눌림 상태에 따라 테두리 색상 변경
                if (dis->itemState & ODS_SELECTED) {
                    SelectObject(hdc, m_btnHoverPen);
                }

                Rectangle(hdc, rc.left, rc.top, rc.right, rc.bottom);

                // 텍스트 그리기
                wchar_t text[256];
                GetWindowTextW(dis->hwndItem, text, 256);

                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, m_colorText);

                HFONT oldFont = (HFONT)SelectObject(hdc, m_font);
                DrawTextW(hdc, text, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

                SelectObject(hdc, oldFont);
                SelectObject(hdc, oldPen);
                SelectObject(hdc, oldBrush);

                return TRUE;
            }
            return FALSE;
        }

        case WM_COMMAND:
            onCommand(LOWORD(wParam), HIWORD(wParam));
            return 0;

        case WM_TRAYICON:
            if (m_trayCallback) {
                m_trayCallback(wParam, lParam);
            }
            return 0;

        case WM_HOTKEY:
            if (wParam == IDH_TOGGLE_ENABLE && m_toggleCallback) {
                m_toggleCallback();
            }
            return 0;

        case WM_CLOSE:
            DestroyWindow(m_hwnd);
            return 0;

        case WM_DESTROY:
            UnregisterHotKey(m_hwnd, IDH_TOGGLE_ENABLE);
            PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProcW(m_hwnd, msg, wParam, lParam);
    }
}

void MainWindow::createControls() {
    // 폰트 생성
    m_font = CreateFontW(
        -14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"맑은 고딕"
    );

    // 제목용 큰 폰트
    m_titleFont = CreateFontW(
        -18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
        L"맑은 고딕"
    );

    int y = 20;

    // 제목 레이블
    HWND titleLabel = CreateWindowExW(
        0, L"STATIC", L"ItemCast",
        WS_CHILD | WS_VISIBLE | SS_CENTER,
        20, y, 260, 30,
        m_hwnd, (HMENU)IDC_STATIC_TITLE, m_hInstance, nullptr
    );
    SendMessageW(titleLabel, WM_SETFONT, (WPARAM)m_titleFont, TRUE);

    y += 40;

    // 슬롯 버튼 레이아웃 (2x3 그리드)
    // [슬롯1] [슬롯2]  -> Num7, Num8
    // [슬롯3] [슬롯4]  -> Num4, Num5
    // [슬롯5] [슬롯6]  -> Num1, Num2

    int btnWidth = 100;
    int btnHeight = 40;
    int labelHeight = 20;
    int gapX = 20;
    int gapY = 10;

    int startX = (320 - (btnWidth * 2 + gapX)) / 2;

    const wchar_t* slotLabels[] = {
        L"Num 7", L"Num 8",
        L"Num 4", L"Num 5",
        L"Num 1", L"Num 2"
    };

    for (int row = 0; row < 3; ++row) {
        for (int col = 0; col < 2; ++col) {
            int idx = row * 2 + col;
            int x = startX + col * (btnWidth + gapX);
            int btnY = y + row * (btnHeight + labelHeight + gapY);

            // 버튼 (Owner-draw로 변경)
            m_btnSlots[idx] = CreateWindowExW(
                0, L"BUTTON", L"클릭하여 설정",
                WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
                x, btnY, btnWidth, btnHeight,
                m_hwnd, (HMENU)(UINT_PTR)(IDC_BTN_SLOT1 + idx), m_hInstance, nullptr
            );
            SendMessageW(m_btnSlots[idx], WM_SETFONT, (WPARAM)m_font, TRUE);

            // 레이블 (목표 키 표시)
            m_labelSlots[idx] = CreateWindowExW(
                0, L"STATIC", slotLabels[idx],
                WS_CHILD | WS_VISIBLE | SS_CENTER,
                x, btnY + btnHeight + 2, btnWidth, labelHeight,
                m_hwnd, (HMENU)(UINT_PTR)(IDC_LABEL_SLOT1 + idx), m_hInstance, nullptr
            );
            SendMessageW(m_labelSlots[idx], WM_SETFONT, (WPARAM)m_font, TRUE);
        }
    }

    y += 3 * (btnHeight + labelHeight + gapY) + 20;

    // 활성화 체크박스 (라벨 없이) + 토글키 버튼을 한 줄에 배치
    m_checkEnable = CreateWindowExW(
        0, L"BUTTON", L"",
        WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,
        50, y + 5, 20, 20,
        m_hwnd, (HMENU)IDC_CHECK_ENABLE, m_hInstance, nullptr
    );
    SendMessageW(m_checkEnable, BM_SETCHECK, BST_CHECKED, 0);

    HWND toggleLabel = CreateWindowExW(
        0, L"STATIC", L"토글키:",
        WS_CHILD | WS_VISIBLE | SS_LEFT,
        80, y + 5, 55, 25,
        m_hwnd, nullptr, m_hInstance, nullptr
    );
    SendMessageW(toggleLabel, WM_SETFONT, (WPARAM)m_font, TRUE);

    m_btnToggleKey = CreateWindowExW(
        0, L"BUTTON", L"F5",
        WS_CHILD | WS_VISIBLE | BS_OWNERDRAW,
        140, y, 80, 30,
        m_hwnd, (HMENU)IDC_BTN_TOGGLEKEY, m_hInstance, nullptr
    );
    SendMessageW(m_btnToggleKey, WM_SETFONT, (WPARAM)m_font, TRUE);

}

void MainWindow::onCommand(WORD id, WORD /* code */) {
    // 슬롯 버튼 클릭
    if (id >= IDC_BTN_SLOT1 && id <= IDC_BTN_SLOT6) {
        int slotIdx = id - IDC_BTN_SLOT1;
        startKeyWait(static_cast<ItemSlot>(slotIdx));
        return;
    }

    switch (id) {
        case IDC_CHECK_ENABLE:
            if (m_enableCallback) {
                bool checked = SendMessageW(m_checkEnable, BM_GETCHECK, 0, 0) == BST_CHECKED;
                m_enableCallback(checked);
            }
            break;

        case IDC_BTN_TOGGLEKEY:
            startToggleKeyWait();
            break;
    }
}

void MainWindow::setEnabled(bool enabled) {
    SendMessageW(m_checkEnable, BM_SETCHECK, enabled ? BST_CHECKED : BST_UNCHECKED, 0);
}

void MainWindow::setToggleKey(DWORD vkCode) {
    m_toggleKeyVk = vkCode;
    if (m_btnToggleKey) {
        std::wstring keyName = KeyMapper::getKeyName(vkCode);
        SetWindowTextW(m_btnToggleKey, keyName.c_str());
    }
}

void MainWindow::registerToggleHotkey(DWORD vkCode) {
    unregisterToggleHotkey();
    m_toggleKeyVk = vkCode;
    RegisterHotKey(m_hwnd, IDH_TOGGLE_ENABLE, 0, vkCode);
}

void MainWindow::unregisterToggleHotkey() {
    UnregisterHotKey(m_hwnd, IDH_TOGGLE_ENABLE);
}

void MainWindow::startToggleKeyWait() {
    m_waitingForToggleKey = true;
    unregisterToggleHotkey();
    SetWindowTextW(m_btnToggleKey, L"키 입력 대기...");

    // 다른 버튼 비활성화
    for (int i = 0; i < 6; ++i) {
        EnableWindow(m_btnSlots[i], FALSE);
    }

    if (m_keyWaitCallback) {
        m_keyWaitCallback(true);
    }
}

void MainWindow::endToggleKeyWait(DWORD vkCode) {
    if (!m_waitingForToggleKey) return;

    m_waitingForToggleKey = false;

    if (m_keyWaitCallback) {
        m_keyWaitCallback(false);
    }

    if (vkCode != 0) {
        m_toggleKeyVk = vkCode;
        std::wstring keyName = KeyMapper::getKeyName(vkCode);
        SetWindowTextW(m_btnToggleKey, keyName.c_str());
        registerToggleHotkey(vkCode);

        if (m_toggleKeyChangeCallback) {
            m_toggleKeyChangeCallback(vkCode);
        }
    } else {
        // 취소 - 원래 키 복원
        std::wstring keyName = KeyMapper::getKeyName(m_toggleKeyVk);
        SetWindowTextW(m_btnToggleKey, keyName.c_str());
        registerToggleHotkey(m_toggleKeyVk);
    }

    // 버튼 다시 활성화
    for (int i = 0; i < 6; ++i) {
        EnableWindow(m_btnSlots[i], TRUE);
    }
}

void MainWindow::setSlotKey(ItemSlot slot, DWORD vkCode) {
    int idx = static_cast<int>(slot);
    if (idx >= 0 && idx < 6) {
        m_slotKeys[idx] = vkCode;
        updateSlotDisplay(slot);
    }
}

void MainWindow::updateSlotDisplay(ItemSlot slot) {
    int idx = static_cast<int>(slot);
    if (idx < 0 || idx >= 6) return;

    std::wstring keyName = KeyMapper::getKeyName(m_slotKeys[idx]);
    SetWindowTextW(m_btnSlots[idx], keyName.c_str());
}

void MainWindow::startKeyWait(ItemSlot slot) {
    m_waitingForKey = true;
    m_waitingSlot = slot;

    int idx = static_cast<int>(slot);
    SetWindowTextW(m_btnSlots[idx], L"키 입력 대기...");

    // 버튼 비활성화 (다른 버튼)
    for (int i = 0; i < 6; ++i) {
        if (i != idx) {
            EnableWindow(m_btnSlots[i], FALSE);
        }
    }

    // 키 대기 모드 시작 알림
    if (m_keyWaitCallback) {
        m_keyWaitCallback(true);
    }
}

void MainWindow::endKeyWait(DWORD vkCode) {
    if (!m_waitingForKey) return;

    m_waitingForKey = false;
    int idx = static_cast<int>(m_waitingSlot);

    // 키 대기 모드 종료 알림
    if (m_keyWaitCallback) {
        m_keyWaitCallback(false);
    }

    if (vkCode != 0) {
        m_slotKeys[idx] = vkCode;
        updateSlotDisplay(m_waitingSlot);

        if (m_keyChangeCallback) {
            m_keyChangeCallback(m_waitingSlot, vkCode);
        }
    } else {
        // 취소됨 - 원래 값으로 복원
        updateSlotDisplay(m_waitingSlot);
    }

    // 버튼 다시 활성화
    for (int i = 0; i < 6; ++i) {
        EnableWindow(m_btnSlots[i], TRUE);
    }
}
