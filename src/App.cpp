#include "App.h"
#include "resource.h"

App* App::s_instance = nullptr;

App::App()
    : m_hInstance(nullptr)
    , m_running(false)
{
    s_instance = this;
}

App::~App() {
    if (s_instance == this) {
        s_instance = nullptr;
    }
}

bool App::initialize(HINSTANCE hInstance) {
    m_hInstance = hInstance;

    // 컴포넌트 생성
    m_keyMapper = std::make_unique<KeyMapper>();
    m_keyboardHook = std::make_unique<KeyboardHook>();
    m_config = std::make_unique<Config>();
    m_mainWindow = std::make_unique<MainWindow>();
    m_trayIcon = std::make_unique<TrayIcon>();

    // 설정 로드
    m_config->load();

    // 키보드 훅에 매퍼 연결
    m_keyboardHook->setKeyMapper(m_keyMapper.get());
    m_keyboardHook->setWarcraft3Only(m_config->isWarcraft3Only());

    // 설정을 매퍼에 적용
    m_config->applyToMapper(*m_keyMapper);

    // 메인 윈도우 생성
    if (!m_mainWindow->create(hInstance)) {
        MessageBoxW(nullptr, L"윈도우 생성 실패", L"오류", MB_ICONERROR);
        return false;
    }

    // 트레이 아이콘 생성
    m_trayIcon->create(m_mainWindow->getHandle(), WM_TRAYICON);

    // 콜백 설정
    m_mainWindow->setEnableCallback([this](bool enabled) { onEnableChanged(enabled); });
    m_mainWindow->setWarcraft3OnlyCallback([this](bool enabled) { onWarcraft3OnlyChanged(enabled); });
    m_mainWindow->setSaveCallback([this]() { onSaveRequested(); });
    m_mainWindow->setMinimizeCallback([this]() { onMinimizeToTray(); });
    m_mainWindow->setKeyChangeCallback([this](ItemSlot slot, DWORD vkCode) { onKeyChanged(slot, vkCode); });
    m_mainWindow->setToggleCallback([this]() { onToggleEnable(); });
    m_mainWindow->setToggleKeyChangeCallback([this](DWORD vkCode) {
        m_config->setToggleKey(vkCode);
        m_config->save();
    });

    m_trayIcon->setShowCallback([this]() { onTrayShow(); });
    m_trayIcon->setToggleCallback([this]() { onTrayToggle(); });
    m_trayIcon->setExitCallback([this]() { onTrayExit(); });

    // 트레이 메시지를 메인 윈도우에서 처리
    m_mainWindow->setTrayCallback([this](WPARAM wParam, LPARAM lParam) {
        m_trayIcon->handleMessage(wParam, lParam);
    });

    // 키 입력 콜백 설정 (키 설정 모드용)
    m_keyboardHook->setKeyCallback([this](DWORD vkCode, bool keyDown) {
        onKeyInput(vkCode, keyDown);
    });

    // 키 대기 모드 콜백 설정
    m_mainWindow->setKeyWaitCallback([this](bool waiting) {
        m_keyboardHook->setKeyWaitMode(waiting);
    });

    // 키보드 훅 설치
    if (!m_keyboardHook->install()) {
        MessageBoxW(nullptr, L"키보드 훅 설치 실패", L"오류", MB_ICONERROR);
        return false;
    }

    // UI 업데이트
    updateUI();

    // 초기 활성화 상태 설정
    m_keyboardHook->setEnabled(m_config->isEnabled());
    m_trayIcon->setEnabled(m_config->isEnabled());

    // 윈도우 표시 (시작 시 최소화가 아니면)
    if (!m_config->isStartMinimized()) {
        m_mainWindow->show();
    }

    m_running = true;
    return true;
}

int App::run() {
    MSG msg;
    while (GetMessageW(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    return static_cast<int>(msg.wParam);
}

void App::quit() {
    m_running = false;

    // 키보드 훅 해제
    m_keyboardHook->uninstall();

    // 트레이 아이콘 제거
    m_trayIcon->remove();

    // 애플리케이션 종료
    PostQuitMessage(0);
}

void App::onEnableChanged(bool enabled) {
    m_keyboardHook->setEnabled(enabled);
    m_trayIcon->setEnabled(enabled);
    m_config->setEnabled(enabled);
}

void App::onWarcraft3OnlyChanged(bool enabled) {
    m_keyboardHook->setWarcraft3Only(enabled);
    m_config->setWarcraft3Only(enabled);
}

void App::onSaveRequested() {
    // 현재 매핑을 Config에 저장
    m_config->loadFromMapper(*m_keyMapper);
    m_config->setEnabled(m_keyboardHook->isEnabled());

    if (m_config->save()) {
        MessageBoxW(m_mainWindow->getHandle(), L"설정이 저장되었습니다.", L"저장 완료", MB_ICONINFORMATION);
    } else {
        MessageBoxW(m_mainWindow->getHandle(), L"설정 저장에 실패했습니다.", L"오류", MB_ICONERROR);
    }
}

void App::onMinimizeToTray() {
    m_mainWindow->hide();
}

void App::onKeyChanged(ItemSlot slot, DWORD vkCode) {
    m_keyMapper->setMapping(vkCode, slot);
    m_config->setSlotKey(slot, vkCode);
    m_config->save();
}

void App::onTrayShow() {
    m_mainWindow->show();
}

void App::onTrayToggle() {
    onToggleEnable();
}

void App::onToggleEnable() {
    bool newState = !m_keyboardHook->isEnabled();
    m_keyboardHook->setEnabled(newState);
    m_trayIcon->setEnabled(newState);
    m_mainWindow->setEnabled(newState);
    m_config->setEnabled(newState);
}

void App::onTrayExit() {
    m_config->loadFromMapper(*m_keyMapper);
    m_config->save();
    quit();
}

void App::onKeyInput(DWORD vkCode, bool keyDown) {
    (void)keyDown;
    if (m_mainWindow->isWaitingForToggle()) {
        m_mainWindow->endToggleKeyWait(vkCode);
    } else if (m_mainWindow->isWaitingForKey()) {
        m_mainWindow->endKeyWait(vkCode);
    }
}

void App::applyConfig() {
    m_config->applyToMapper(*m_keyMapper);
    m_keyboardHook->setEnabled(m_config->isEnabled());
    m_keyboardHook->setWarcraft3Only(m_config->isWarcraft3Only());
}

void App::updateUI() {
    // 각 슬롯의 키 설정을 UI에 반영
    for (int i = 0; i < 6; ++i) {
        ItemSlot slot = static_cast<ItemSlot>(i);
        DWORD vkCode = m_config->getSlotKey(slot);
        m_mainWindow->setSlotKey(slot, vkCode);
    }

    m_mainWindow->setEnabled(m_config->isEnabled());
    m_mainWindow->setToggleKey(m_config->getToggleKey());
    m_mainWindow->registerToggleHotkey(m_config->getToggleKey());
}
