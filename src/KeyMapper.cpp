#include "KeyMapper.h"

// 슬롯별 Numpad 키 매핑
// 슬롯 레이아웃:
// [7][8]  -> Slot1, Slot2
// [4][5]  -> Slot3, Slot4
// [1][2]  -> Slot5, Slot6
const DWORD KeyMapper::s_numpadKeys[6] = {
    VK_NUMPAD7,  // Slot1
    VK_NUMPAD8,  // Slot2
    VK_NUMPAD4,  // Slot3
    VK_NUMPAD5,  // Slot4
    VK_NUMPAD1,  // Slot5
    VK_NUMPAD2   // Slot6
};

KeyMapper::KeyMapper() {
    for (int i = 0; i < 6; ++i) {
        m_slotKeys[i] = 0;  // 매핑되지 않음
    }
}

void KeyMapper::setMapping(DWORD fromKey, ItemSlot slot) {
    if (fromKey == 0) return;

    int slotIdx = static_cast<int>(slot);
    if (slotIdx < 0 || slotIdx >= 6) return;

    // 기존 매핑 제거
    if (m_slotKeys[slotIdx] != 0) {
        m_keyMap.erase(m_slotKeys[slotIdx]);
    }

    // 새 매핑 설정
    m_slotKeys[slotIdx] = fromKey;
    m_keyMap[fromKey] = s_numpadKeys[slotIdx];
}

void KeyMapper::setMappingBySlot(ItemSlot slot, DWORD fromKey) {
    setMapping(fromKey, slot);
}

void KeyMapper::clearMapping(DWORD fromKey) {
    auto it = m_keyMap.find(fromKey);
    if (it != m_keyMap.end()) {
        // 해당 슬롯 찾아서 초기화
        for (int i = 0; i < 6; ++i) {
            if (m_slotKeys[i] == fromKey) {
                m_slotKeys[i] = 0;
                break;
            }
        }
        m_keyMap.erase(it);
    }
}

void KeyMapper::clearSlot(ItemSlot slot) {
    int slotIdx = static_cast<int>(slot);
    if (slotIdx < 0 || slotIdx >= 6) return;

    if (m_slotKeys[slotIdx] != 0) {
        m_keyMap.erase(m_slotKeys[slotIdx]);
        m_slotKeys[slotIdx] = 0;
    }
}

void KeyMapper::clearAll() {
    m_keyMap.clear();
    for (int i = 0; i < 6; ++i) {
        m_slotKeys[i] = 0;
    }
}

bool KeyMapper::hasMapping(DWORD vkCode) const {
    return m_keyMap.find(vkCode) != m_keyMap.end();
}

DWORD KeyMapper::translate(DWORD vkCode) const {
    auto it = m_keyMap.find(vkCode);
    if (it != m_keyMap.end()) {
        return it->second;
    }
    return vkCode;  // 매핑이 없으면 원래 키 반환
}

DWORD KeyMapper::getSlotTargetKey(ItemSlot slot) const {
    int slotIdx = static_cast<int>(slot);
    if (slotIdx < 0 || slotIdx >= 6) return 0;
    return s_numpadKeys[slotIdx];
}

DWORD KeyMapper::getSlotSourceKey(ItemSlot slot) const {
    int slotIdx = static_cast<int>(slot);
    if (slotIdx < 0 || slotIdx >= 6) return 0;
    return m_slotKeys[slotIdx];
}

DWORD KeyMapper::getNumpadKey(ItemSlot slot) {
    int slotIdx = static_cast<int>(slot);
    if (slotIdx < 0 || slotIdx >= 6) return 0;
    return s_numpadKeys[slotIdx];
}

std::wstring KeyMapper::getKeyName(DWORD vkCode) {
    if (vkCode == 0) {
        return L"없음";
    }

    // 특수 키 이름
    switch (vkCode) {
        case VK_NUMPAD0: return L"Num 0";
        case VK_NUMPAD1: return L"Num 1";
        case VK_NUMPAD2: return L"Num 2";
        case VK_NUMPAD3: return L"Num 3";
        case VK_NUMPAD4: return L"Num 4";
        case VK_NUMPAD5: return L"Num 5";
        case VK_NUMPAD6: return L"Num 6";
        case VK_NUMPAD7: return L"Num 7";
        case VK_NUMPAD8: return L"Num 8";
        case VK_NUMPAD9: return L"Num 9";
        case VK_MULTIPLY: return L"Num *";
        case VK_ADD: return L"Num +";
        case VK_SUBTRACT: return L"Num -";
        case VK_DECIMAL: return L"Num .";
        case VK_DIVIDE: return L"Num /";
        case VK_SPACE: return L"Space";
        case VK_TAB: return L"Tab";
        case VK_RETURN: return L"Enter";
        case VK_ESCAPE: return L"Esc";
        case VK_BACK: return L"Backspace";
        case VK_DELETE: return L"Delete";
        case VK_INSERT: return L"Insert";
        case VK_HOME: return L"Home";
        case VK_END: return L"End";
        case VK_PRIOR: return L"Page Up";
        case VK_NEXT: return L"Page Down";
        case VK_LEFT: return L"←";
        case VK_RIGHT: return L"→";
        case VK_UP: return L"↑";
        case VK_DOWN: return L"↓";
        case VK_F1: return L"F1";
        case VK_F2: return L"F2";
        case VK_F3: return L"F3";
        case VK_F4: return L"F4";
        case VK_F5: return L"F5";
        case VK_F6: return L"F6";
        case VK_F7: return L"F7";
        case VK_F8: return L"F8";
        case VK_F9: return L"F9";
        case VK_F10: return L"F10";
        case VK_F11: return L"F11";
        case VK_F12: return L"F12";
        case VK_OEM_1: return L";";
        case VK_OEM_PLUS: return L"=";
        case VK_OEM_COMMA: return L",";
        case VK_OEM_MINUS: return L"-";
        case VK_OEM_PERIOD: return L".";
        case VK_OEM_2: return L"/";
        case VK_OEM_3: return L"`";
        case VK_OEM_4: return L"[";
        case VK_OEM_5: return L"\\";
        case VK_OEM_6: return L"]";
        case VK_OEM_7: return L"'";
    }

    // 알파벳/숫자 키
    if ((vkCode >= 'A' && vkCode <= 'Z') || (vkCode >= '0' && vkCode <= '9')) {
        return std::wstring(1, static_cast<wchar_t>(vkCode));
    }

    // 기타: GetKeyNameText 사용
    UINT scanCode = MapVirtualKeyW(vkCode, MAPVK_VK_TO_VSC);
    wchar_t keyName[64] = { 0 };
    if (GetKeyNameTextW(scanCode << 16, keyName, 64) > 0) {
        return keyName;
    }

    return L"Unknown";
}
