#include "Config.h"
#include <json.hpp>
#include <fstream>
#include <ShlObj.h>

using json = nlohmann::json;

Config::Config()
    : m_toggleKey(VK_F5)
    , m_enabled(true)
    , m_warcraft3Only(true)
    , m_startMinimized(false)
{
    for (int i = 0; i < 6; ++i) {
        m_slotKeys[i] = 0;
    }
    m_configPath = getDefaultConfigPath();
}

void Config::setConfigPath(const std::wstring& path) {
    m_configPath = path;
}

std::wstring Config::getDefaultConfigPath() {
    wchar_t appDataPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, appDataPath))) {
        std::wstring path(appDataPath);
        path += L"\\ItemCast";

        // 디렉토리 생성
        CreateDirectoryW(path.c_str(), nullptr);

        path += L"\\config.json";
        return path;
    }

    // 폴백: 실행 파일 경로
    wchar_t exePath[MAX_PATH];
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    std::wstring path(exePath);
    size_t lastSlash = path.find_last_of(L"\\/");
    if (lastSlash != std::wstring::npos) {
        path = path.substr(0, lastSlash + 1);
    }
    path += L"config.json";
    return path;
}

bool Config::load() {
    // UTF-8 파일 읽기
    std::string configPathUtf8;
    int size = WideCharToMultiByte(CP_UTF8, 0, m_configPath.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size > 0) {
        configPathUtf8.resize(size - 1);
        WideCharToMultiByte(CP_UTF8, 0, m_configPath.c_str(), -1, &configPathUtf8[0], size, nullptr, nullptr);
    }

    std::ifstream file(m_configPath);
    if (!file.is_open()) {
        return false;  // 파일 없음 - 기본값 사용
    }

    try {
        json j;
        file >> j;

        // 슬롯 키 로드
        if (j.contains("slots") && j["slots"].is_array()) {
            auto& slots = j["slots"];
            for (int i = 0; i < 6 && i < static_cast<int>(slots.size()); ++i) {
                if (slots[i].is_number()) {
                    m_slotKeys[i] = slots[i].get<DWORD>();
                }
            }
        }

        // 설정 로드
        if (j.contains("enabled") && j["enabled"].is_boolean()) {
            m_enabled = j["enabled"].get<bool>();
        }
        if (j.contains("warcraft3Only") && j["warcraft3Only"].is_boolean()) {
            m_warcraft3Only = j["warcraft3Only"].get<bool>();
        }
        if (j.contains("startMinimized") && j["startMinimized"].is_boolean()) {
            m_startMinimized = j["startMinimized"].get<bool>();
        }
        if (j.contains("toggleKey") && j["toggleKey"].is_number()) {
            m_toggleKey = j["toggleKey"].get<DWORD>();
        }

        return true;
    }
    catch (const json::exception&) {
        return false;
    }
}

bool Config::save() {
    try {
        json j;

        // 슬롯 키 저장
        j["slots"] = json::array();
        for (int i = 0; i < 6; ++i) {
            j["slots"].push_back(m_slotKeys[i]);
        }

        // 설정 저장
        j["enabled"] = m_enabled;
        j["warcraft3Only"] = m_warcraft3Only;
        j["startMinimized"] = m_startMinimized;
        j["toggleKey"] = m_toggleKey;

        // 파일 쓰기
        std::ofstream file(m_configPath);
        if (!file.is_open()) {
            return false;
        }

        file << j.dump(2);  // 들여쓰기 2칸
        return true;
    }
    catch (const json::exception&) {
        return false;
    }
}

void Config::setSlotKey(ItemSlot slot, DWORD vkCode) {
    int slotIdx = static_cast<int>(slot);
    if (slotIdx >= 0 && slotIdx < 6) {
        m_slotKeys[slotIdx] = vkCode;
    }
}

DWORD Config::getSlotKey(ItemSlot slot) const {
    int slotIdx = static_cast<int>(slot);
    if (slotIdx >= 0 && slotIdx < 6) {
        return m_slotKeys[slotIdx];
    }
    return 0;
}

void Config::applyToMapper(KeyMapper& mapper) const {
    mapper.clearAll();
    for (int i = 0; i < 6; ++i) {
        if (m_slotKeys[i] != 0) {
            mapper.setMapping(m_slotKeys[i], static_cast<ItemSlot>(i));
        }
    }
}

void Config::loadFromMapper(const KeyMapper& mapper) {
    for (int i = 0; i < 6; ++i) {
        m_slotKeys[i] = mapper.getSlotSourceKey(static_cast<ItemSlot>(i));
    }
}
