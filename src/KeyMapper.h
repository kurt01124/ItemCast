#pragma once

#include <Windows.h>
#include <map>
#include <string>

// 아이템 슬롯 인덱스 (0-5)
enum class ItemSlot {
    Slot1 = 0,  // Numpad 7
    Slot2 = 1,  // Numpad 8
    Slot3 = 2,  // Numpad 4
    Slot4 = 3,  // Numpad 5
    Slot5 = 4,  // Numpad 1
    Slot6 = 5,  // Numpad 2
    Count = 6
};

class KeyMapper {
public:
    KeyMapper();
    ~KeyMapper() = default;

    // 키 매핑 설정
    void setMapping(DWORD fromKey, ItemSlot slot);
    void setMappingBySlot(ItemSlot slot, DWORD fromKey);
    void clearMapping(DWORD fromKey);
    void clearSlot(ItemSlot slot);
    void clearAll();

    // 키 변환
    bool hasMapping(DWORD vkCode) const;
    DWORD translate(DWORD vkCode) const;

    // 슬롯 정보 조회
    DWORD getSlotTargetKey(ItemSlot slot) const;
    DWORD getSlotSourceKey(ItemSlot slot) const;

    // 키 이름 변환
    static std::wstring getKeyName(DWORD vkCode);
    static DWORD getNumpadKey(ItemSlot slot);

private:
    std::map<DWORD, DWORD> m_keyMap;      // 입력키 → Numpad 키
    DWORD m_slotKeys[6];                   // 슬롯별 입력키 저장

    static const DWORD s_numpadKeys[6];    // 슬롯별 Numpad 키
};
