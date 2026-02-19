# ItemCast

Warcraft III 아이템 슬롯 키 리매핑 유틸리티.

워크래프트 3의 아이템 슬롯은 넘패드(Numpad 7/8/4/5/1/2)에 고정되어 있어 텐키리스 키보드 사용자나 빠른 아이템 사용이 필요한 유저에게 불편합니다. ItemCast는 이 슬롯 키를 원하는 키로 리매핑하여 아이템 사용을 편리하게 만들어 줍니다.

## 주요 기능

- **아이템 슬롯 리매핑** — 6개 슬롯 각각에 원하는 키를 바인딩
- **토글 단축키** — 설정한 핫키(기본 F5)로 리매핑 활성/비활성 전환
- **시스템 트레이** — 트레이 아이콘으로 상주, 우클릭 메뉴 제공
- **설정 자동 저장** — 키 변경 시 즉시 `%APPDATA%\ItemCast\config.json`에 저장
- **단일 인스턴스** — 중복 실행 시 기존 창을 활성화

## 슬롯 매핑

워크3 인벤토리 배치에 대응하는 기본 넘패드 키:

```
┌──────────┬──────────┐
│ Slot 1   │ Slot 2   │
│ Numpad 7 │ Numpad 8 │
├──────────┼──────────┤
│ Slot 3   │ Slot 4   │
│ Numpad 4 │ Numpad 5 │
├──────────┼──────────┤
│ Slot 5   │ Slot 6   │
│ Numpad 1 │ Numpad 2 │
└──────────┴──────────┘
```

예를 들어 Slot 1에 `Q`, Slot 2에 `W`를 매핑하면 Q를 누를 때 Numpad 7이, W를 누를 때 Numpad 8이 입력됩니다.

## 빌드

### 요구사항

- Windows 10 이상
- [Visual Studio 2022](https://visualstudio.microsoft.com/) (C++ 데스크톱 개발 워크로드)
- [CMake 3.21+](https://cmake.org/) (VS2022에 포함)

### 빌드 방법

`build.bat`을 더블클릭하면 빌드가 진행되고, 완료 시 출력 폴더가 자동으로 열립니다.

또는 직접 명령어를 실행:

```bash
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

실행 파일은 `build/bin/Release/ItemCast.exe`에 생성됩니다.

### 의존성

- [nlohmann/json](https://github.com/nlohmann/json) — `include/json.hpp`에 포함 (별도 설치 불필요)
- Windows API (user32, gdi32, shell32, comctl32) — 시스템 라이브러리

## 사용법

1. `ItemCast.exe`를 실행합니다.
2. 각 슬롯 버튼을 클릭하고 매핑할 키를 누릅니다. (ESC로 취소)
3. 키를 설정하면 자동으로 저장됩니다.
4. 토글 키 버튼을 클릭하여 활성/비활성 전환 단축키를 변경할 수 있습니다. (기본: F5)

### 트레이 아이콘

- **더블 클릭** — 설정 창 열기
- **우클릭** → 열기 / 토글 / 종료

## 동작 원리

Windows Low-Level Keyboard Hook(`WH_KEYBOARD_LL`)을 사용하여 키 입력을 가로채고, 매핑된 키를 `SendInput`으로 변환 전송합니다. 자체 발생시킨 키(`LLKHF_INJECTED`)는 무시하여 무한 루프를 방지합니다.

## 프로젝트 구조

```
ItemCast/
├── src/
│   ├── main.cpp          # 엔트리포인트, 단일 인스턴스 처리
│   ├── App.cpp/h         # 애플리케이션 코어, 컴포넌트 조율
│   ├── KeyMapper.cpp/h   # 키 매핑 테이블 관리
│   ├── KeyboardHook.cpp/h # Low-Level 키보드 훅
│   ├── Config.cpp/h      # JSON 설정 로드/저장
│   ├── MainWindow.cpp/h  # Win32 GUI 윈도우
│   ├── TrayIcon.cpp/h    # 시스템 트레이 아이콘
│   └── resource.h        # 리소스 ID 정의
├── include/
│   └── json.hpp          # nlohmann/json 헤더
├── resources/
│   ├── app.rc            # 리소스 스크립트
│   ├── app.manifest      # Common Controls 6.0 매니페스트
│   └── icon*.ico         # 아이콘 파일
├── CMakeLists.txt
└── build.bat             # 원클릭 빌드 스크립트
```

## 라이선스

[MIT](LICENSE) — Lee Hyun
