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

    // 토글키
    void setToggleKey(DWORD vkCode) { m_toggleKey = vkCode; }
    DWORD getToggleKey() const { return m_toggleKey; }

    // KeyMapper에 설정 적용
    void applyToMapper(KeyMapper& mapper) const;
    void loadFromMapper(const KeyMapper& mapper);

private:
    std::wstring m_configPath;
    DWORD m_slotKeys[6];
    DWORD m_toggleKey;
    bool m_enabled;

    static std::wstring getDefaultConfigPath();
};
