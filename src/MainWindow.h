#pragma once

#include <Windows.h>
#include <functional>
#include "KeyMapper.h"

class MainWindow {
public:
    using EnableCallback = std::function<void(bool)>;
    using SaveCallback = std::function<void()>;
    using MinimizeCallback = std::function<void()>;
    using KeyChangeCallback = std::function<void(ItemSlot, DWORD)>;
    using TrayCallback = std::function<void(WPARAM, LPARAM)>;
    using KeyWaitCallback = std::function<void(bool)>;  // true=시작, false=종료
    using ToggleCallback = std::function<void()>;
    using ToggleKeyChangeCallback = std::function<void(DWORD)>;

    MainWindow();
    ~MainWindow();

    // 윈도우 생성/표시
    bool create(HINSTANCE hInstance);
    void show();
    void hide();
    bool isVisible() const;

    // 윈도우 핸들
    HWND getHandle() const { return m_hwnd; }

    // 상태 업데이트
    void setEnabled(bool enabled);
    void setSlotKey(ItemSlot slot, DWORD vkCode);
    void updateSlotDisplay(ItemSlot slot);
    void setToggleKey(DWORD vkCode);
    void registerToggleHotkey(DWORD vkCode);
    void unregisterToggleHotkey();

    // 콜백 설정
    void setEnableCallback(EnableCallback callback) { m_enableCallback = callback; }
    void setWarcraft3OnlyCallback(EnableCallback callback) { m_warcraft3OnlyCallback = callback; }
    void setSaveCallback(SaveCallback callback) { m_saveCallback = callback; }
    void setMinimizeCallback(MinimizeCallback callback) { m_minimizeCallback = callback; }
    void setKeyChangeCallback(KeyChangeCallback callback) { m_keyChangeCallback = callback; }
    void setTrayCallback(TrayCallback callback) { m_trayCallback = callback; }
    void setKeyWaitCallback(KeyWaitCallback callback) { m_keyWaitCallback = callback; }
    void setToggleCallback(ToggleCallback callback) { m_toggleCallback = callback; }
    void setToggleKeyChangeCallback(ToggleKeyChangeCallback callback) { m_toggleKeyChangeCallback = callback; }

    // 키 입력 대기 모드
    void startKeyWait(ItemSlot slot);
    void endKeyWait(DWORD vkCode);
    bool isWaitingForKey() const { return m_waitingForKey || m_waitingForToggleKey; }
    ItemSlot getWaitingSlot() const { return m_waitingSlot; }
    bool isWaitingForToggle() const { return m_waitingForToggleKey; }
    void startToggleKeyWait();
    void endToggleKeyWait(DWORD vkCode);

private:
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

    void createControls();
    void onCommand(WORD id, WORD code);

    HWND m_hwnd;
    HINSTANCE m_hInstance;
    HFONT m_font;
    HFONT m_titleFont;

    // 테마용 GDI 객체
    HBRUSH m_bgBrush;           // 배경 브러시
    HBRUSH m_btnBrush;          // 버튼 내부 브러시
    HBRUSH m_textBrush;         // 텍스트 배경 브러시
    HPEN m_btnBorderPen;        // 버튼 테두리 펜
    HPEN m_btnHoverPen;         // 버튼 호버 펜
    COLORREF m_colorText;       // 텍스트 색상
    COLORREF m_colorLabelGray;  // 레이블 회색
    int m_hoveredBtn;           // 호버 중인 버튼 인덱스 (-1: 없음, 0-5: 슬롯, 6: 토글, 7: 저장)

    // 컨트롤 핸들
    HWND m_checkEnable;
    HWND m_checkWar3Only;
    HWND m_btnSave;
    HWND m_btnTray;
    HWND m_btnToggleKey;
    HWND m_labelToggleKey;
    HWND m_btnSlots[6];
    HWND m_labelSlots[6];

    // 키 매핑 상태
    DWORD m_slotKeys[6];

    // 키 입력 대기
    bool m_waitingForKey;
    ItemSlot m_waitingSlot;
    bool m_waitingForToggleKey;
    DWORD m_toggleKeyVk;

    // 콜백
    EnableCallback m_enableCallback;
    EnableCallback m_warcraft3OnlyCallback;
    SaveCallback m_saveCallback;
    MinimizeCallback m_minimizeCallback;
    KeyChangeCallback m_keyChangeCallback;
    TrayCallback m_trayCallback;
    KeyWaitCallback m_keyWaitCallback;
    ToggleCallback m_toggleCallback;
    ToggleKeyChangeCallback m_toggleKeyChangeCallback;

    static const wchar_t* WINDOW_CLASS;
    static const wchar_t* WINDOW_TITLE;
};
