#pragma once

#include <Windows.h>
#include <string>
#include "KeyMapper.h"

class Config {
public:
    Config();
    ~Config() = default;

    // 설정 파일 경로
    void setConfigPath(const std::wstring& path);
    std::wstring getConfigPath() const { return m_configPath; }

    // 설정 로드/저장
    bool load();
    bool save();

    // 키 매핑 설정
    void setSlotKey(ItemSlot slot, DWORD vkCode);
    DWORD getSlotKey(ItemSlot slot) const;

    // 활성화 상태
    void setEnabled(bool enabled) { m_enabled = enabled; }
    bool isEnabled() const { return m_enabled; }

    // 워크래프트3 전용 모드
    void setWarcraft3Only(bool enabled) { m_warcraft3Only = enabled; }
    bool isWarcraft3Only() const { return m_warcraft3Only; }

    // 토글키
    void setToggleKey(DWORD vkCode) { m_toggleKey = vkCode; }
    DWORD getToggleKey() const { return m_toggleKey; }

    // 시작 시 최소화
    void setStartMinimized(bool minimized) { m_startMinimized = minimized; }
    bool isStartMinimized() const { return m_startMinimized; }

    // KeyMapper에 설정 적용
    void applyToMapper(KeyMapper& mapper) const;
    void loadFromMapper(const KeyMapper& mapper);

private:
    std::wstring m_configPath;
    DWORD m_slotKeys[6];
    DWORD m_toggleKey;
    bool m_enabled;
    bool m_warcraft3Only;
    bool m_startMinimized;

    static std::wstring getDefaultConfigPath();
};
