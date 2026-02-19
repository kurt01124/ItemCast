cmake_minimum_required(VERSION 3.21)
project(ItemCast VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Windows 전용
if(NOT WIN32)
    message(FATAL_ERROR "This project only supports Windows")
endif()

# Unicode 지원
add_definitions(-DUNICODE -D_UNICODE)

# 소스 파일
set(SOURCES
    src/main.cpp
    src/App.cpp
    src/KeyMapper.cpp
    src/KeyboardHook.cpp
    src/Config.cpp
    src/MainWindow.cpp
    src/TrayIcon.cpp
)

# 헤더 파일
set(HEADERS
    src/App.h
    src/KeyMapper.h
    src/KeyboardHook.h
    src/Config.h
    src/MainWindow.h
    src/TrayIcon.h
    src/resource.h
)

# 리소스 파일
set(RESOURCES
    resources/app.rc
)

# 실행 파일 생성 (WIN32로 콘솔 창 숨김)
add_executable(${PROJECT_NAME} WIN32 ${SOURCES} ${HEADERS} ${RESOURCES})

# include 디렉토리
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/src
)

# Windows 라이브러리 링크
target_link_libraries(${PROJECT_NAME} PRIVATE
    user32
    gdi32
    shell32
    comctl32
)

# MSVC 설정
if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /utf-8)
    # CRT 정적 링크 (VC++ Runtime DLL 불필요)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # 매니페스트로 Common Controls 6.0 활성화
    set_target_properties(${PROJECT_NAME} PROPERTIES
        LINK_FLAGS "/MANIFEST:EMBED /MANIFESTINPUT:${CMAKE_SOURCE_DIR}/resources/app.manifest"
        MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
    )
endif()

# 출력 디렉토리
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
