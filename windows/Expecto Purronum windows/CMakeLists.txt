cmake_minimum_required(VERSION 3.24)

project(ExpectoPurronumWindows LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(ExpectoPurronum WIN32
    src/AppState.cpp
    src/ExpectoPurronum.rc
    src/KeyboardGuard.cpp
    src/LaunchAtLoginService.cpp
    src/Localization.cpp
    src/SettingsStore.cpp
    src/SettingsWindow.cpp
    src/TrayIcon.cpp
    src/main.cpp
)

target_include_directories(ExpectoPurronum PRIVATE src)

target_compile_definitions(ExpectoPurronum PRIVATE
    UNICODE
    _UNICODE
    WIN32_LEAN_AND_MEAN
    NOMINMAX
)

if(MSVC)
    target_compile_options(ExpectoPurronum PRIVATE /EHsc)
endif()

if(MINGW)
    target_link_options(ExpectoPurronum PRIVATE -municode)
endif()

target_link_libraries(ExpectoPurronum PRIVATE
    user32
    shell32
    advapi32
    ole32
    shlwapi
)
