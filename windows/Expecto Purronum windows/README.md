# Expecto Purronum for Windows

Native Win32 C++ tray MVP for the Windows version of Expecto Purronum.

## Build in VS Code on Windows

Prerequisites:

- Visual Studio 2022 Build Tools with the C++ desktop workload
- CMake
- VS Code extensions: C/C++ and CMake Tools

Open this folder in VS Code:

```text
windows/Expecto Purronum windows
```

Then run:

```text
Terminal > Run Build Task > CMake: Build
```

The executable is generated at:

```text
build/Release/ExpectoPurronum.exe
```

## MVP scope

- Native tray icon and context menu
- Global low-level keyboard hook
- Adjacent-key, single-key, and modifier-key hold detection
- Keyboard lock by suppressing keyboard events
- Manual start, pause, lock, and unlock
- Settings dialog
- English and Chinese UI text
- Per-user launch-at-login via `HKCU\Software\Microsoft\Windows\CurrentVersion\Run`
- Settings saved under `%APPDATA%\Expecto Purronum\settings.ini`
