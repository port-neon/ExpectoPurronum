# Expecto Purronum 🐾🪄  
# 呼喵护键

一款为猫咪铲屎官设计的自动键盘守护工具。  
当猫猫突然踩上键盘、长按按键、乱触多个按键时，Expecto Purronum 会尝试自动触发键盘锁，帮助减少误输入、误操作和突发“猫猫事故”。

Automatic keyboard lock app that helps prevent accidental typing caused by cats.

---

## 🐱 项目概述

Expecto Purronum（呼喵护键）是一款为猫咪铲屎官设计的键盘守护工具。

它会在后台守护键盘输入状态。当检测到可能由猫猫产生的异常输入模式时，应用会自动触发键盘锁，防止猫猫乱打字、误触快捷键、打断正在进行的工作。

与传统手动键盘锁不同，Expecto Purronum 不需要你提前主动开启锁定。  
它的目标是：**即使你不在电脑前，也能自动应对猫猫突然踩键盘的情况。**

---

## ✨ 功能介绍

- 🐾 后台键盘活动守护  
- 🔒 检测到异常输入模式时自动锁定键盘  
- ⏱️ 支持长按、多键相邻误触等检测规则  
- 🧙 支持手动锁定 / 解锁键盘  
- 💤 支持暂停键盘守护  
- 🖥️ macOS 菜单栏状态图标  
- 🪟 Windows 系统托盘状态图标  
- 🚀 支持开机自启动  
- 🌐 支持基础多语言切换  
- 💾 支持本地设置保存  
- 🛡️ 支持锁屏 / 睡眠状态下的安全处理，避免影响用户正常输入密码

---
## 🍎 macOS 安装方式

1. 下载最新的 `.dmg` 文件。  
2. 打开 `.dmg` 文件。  
3. 将 `Expecto Purronum.app` 拖入 `Applications` 文件夹。  
4. 从 `Applications` 文件夹中打开 App。  

---

## 🪟 Windows 安装方式

1. 从 [Releases](../../releases) 页面下载 Windows 安装包。  
2. 双击运行：

```text
ExpectoPurronum-Windows-Setup-v1.1.0.exe
```

3. 按照安装向导完成安装。  
4. 从开始菜单或桌面快捷方式启动 Expecto Purronum。  
5. 启动后，应用会显示在 Windows 系统托盘区域。  

---

## 🧭 如何使用？

### 1. 如何开启键盘守护？

应用启动后会自动进入键盘守护状态，并在后台检测可能由猫猫导致的异常键盘输入。

你也可以通过菜单栏 / 系统托盘菜单选择：

```text
Start Protection
```

或：

```text
开启键盘守护
```

---

### 2. 如何暂停键盘守护？

如果你正在游戏、测试键盘，或者暂时不需要自动锁定，可以暂停守护。

macOS 端：

```text
点击菜单栏图标 → Pause Protection / 暂停守护键盘
```

Windows 端：

```text
右键系统托盘图标 → Pause Protection / 暂停守护键盘
```

暂停后，应用不会因为异常输入而自动锁定键盘。

---

### 3. 如何锁定键盘？

键盘锁可以自动触发，也可以手动触发。

自动触发规则包括：

1. 单个按键被长时间按住  
2. 两个或更多相邻按键被同时长时间按住  
3. 某些特殊键，例如 Caps Lock，被长时间按住  

这些检测时间可以在设置中进行调整。

你也可以手动选择：

```text
Lock
```

或：

```text
封印键盘
```

来立即锁定键盘。

---

### 4. 如何解锁键盘？

macOS 端：

```text
点击菜单栏图标 → Unlock / 解除封印
```

Windows 端：

```text
右键系统托盘图标 → Unlock / 解除封印
```

或在弹窗中选择解除锁定。

当系统进入睡眠状态时，键盘锁也会自动释放，避免影响之后正常使用电脑。

---

## 🖥️ 兼容性

Expecto Purronum 目前支持：

### macOS

- macOS 13.0 或更高版本
- Apple Silicon

### Windows

- Windows 10 或更高版本
- 64 位 Windows

---

## 📦 下载

请前往 [Releases](../../releases) 页面下载最新版本。

### macOS

下载最新的 `.dmg` 文件。

### Windows

下载最新的 Windows 安装包：

```text
ExpectoPurronum-Windows-Setup-v1.1.0.exe
```

---


## 🍎 macOS 安全提示

Expecto Purronum 目前通过 Mac App Store 之外的方式分发，尚未经过 Apple notarization。

第一次打开 App 时，macOS 可能会提示无法验证开发者。

如需继续打开，请右键点击 `Expecto Purronum.app`，选择 **Open / 打开**，然后在警告弹窗中再次点击 **Open / 打开**。

如果 macOS 仍然阻止打开，请前往：

```text
System Settings / 系统设置
→ Privacy & Security / 隐私与安全性
→ Open Anyway / 仍要打开
```

---

## 🪟 Windows 安全提示

Expecto Purronum Windows 版目前尚未进行代码签名。

在部分 Windows 设备上，系统可能会根据安全策略显示安装或运行提醒。  
如果你是从本项目的 GitHub Releases 页面下载的安装包，可以根据自己的判断继续安装。

---

## 🔐 权限说明

### macOS

Expecto Purronum 可能需要 macOS 的以下权限才能正常工作：

- **Accessibility / 辅助功能**
- **Input Monitoring / 输入监控**

你可以在以下位置开启相关权限：

```text
System Settings / 系统设置
→ Privacy & Security / 隐私与安全性
```

### Windows

Windows 版本主要通过系统托盘运行，并在本地保存设置。  
如果启用开机自启动，应用会写入对应的启动配置。

---

## 🕊️ 隐私说明

Expecto Purronum 的目标是判断键盘输入行为是否像“猫猫踩键盘”，而不是记录用户输入内容。

为了实现自动键盘守护，应用需要检测一些键盘事件状态，例如：

- 某个按键是否被长时间按住
- 是否有多个相邻按键同时被按住
- 特殊按键是否出现异常长按
- 当前键盘是否处于锁定 / 解锁状态

Expecto Purronum 不会：

- 记录你具体输入了什么文字
- 保存你的聊天内容、密码、账号或表单内容
- 上传键盘输入内容到服务器
- 进行云端同步或用户行为分析
- 在后台收集个人身份信息

应用只会在本地根据按键状态和持续时间进行判断，并在检测到疑似猫猫误触时触发键盘锁。

### 本地数据

Expecto Purronum 可能会在本地保存一些应用设置，例如：

- 语言设置
- 检测时间设置
- 是否开机自启动
- 当前守护相关配置

这些设置用于让应用在下次启动时保持你的偏好。

### 开机自启动

如果你启用了开机自启动，Windows 版本会写入当前用户的启动配置，使应用可以在登录系统后自动启动。  
你可以随时在应用设置中关闭开机自启动。

---

## 🧩 项目状态

当前版本：

- macOS：已发布
- Windows：已发布 v1.1.0 🎉

后续计划：

- 优化 Windows 设置界面
- 改进多语言体验
- 增强托盘状态提示
- 优化检测规则
- 增加更多猫猫友好的交互细节 🐈

---

# English Description

# Expecto Purronum 🐾🪄

Expecto Purronum is an automatic keyboard guardian utility designed for cat owners.

When your cat suddenly steps on the keyboard, holds down keys, or presses multiple nearby keys, Expecto Purronum helps detect suspicious input patterns and automatically locks the keyboard to reduce accidental typing, unwanted actions, and unexpected “cat incidents.”

---

## 🐱 Overview

Expecto Purronum is a keyboard protection application designed for cat owners.

It runs in the background and watches for abnormal keyboard input patterns that may be caused by cats. When such patterns are detected, the app automatically triggers a keyboard lock to prevent accidental typing or unwanted keyboard actions.

Unlike traditional manual keyboard lockers, Expecto Purronum does not require you to lock the keyboard in advance.

Its goal is simple: **protect your keyboard even when you are away from your computer and your cat suddenly decides to become a programmer.**

---

## ✨ Features

- 🐾 Background keyboard activity protection
- 🔒 Automatic keyboard locking when suspicious input patterns are detected
- ⏱️ Detection rules for long key presses and multiple nearby key presses
- 🧙 Manual lock / unlock support
- 💤 Pause protection support
- 🖥️ macOS menu bar status item
- 🪟 Windows system tray status icon
- 🚀 Launch at startup support
- 🌐 Basic multilingual support
- 💾 Local settings storage
- 🛡️ Safety handling for lock screen and sleep states

---

## 📦 Download

Download the latest version from the [Releases](../../releases) page.

### macOS

Download the latest `.dmg` file.

### Windows

Download the latest Windows installer:

```text
ExpectoPurronum-Windows-Setup-v1.1.0.exe
```

---

## 🍎 macOS Installation

1. Download the latest `.dmg` file.
2. Open the `.dmg` file.
3. Drag `Expecto Purronum.app` into the `Applications` folder.
4. Open the app from `Applications`.

---

## 🪟 Windows Installation

1. Download the Windows installer from the [Releases](../../releases) page.
2. Double-click:

```text
ExpectoPurronum-Windows-Setup-v1.1.0.exe
```

3. Follow the installation wizard.
4. Launch Expecto Purronum from the Start Menu or desktop shortcut.
5. After launch, the app will appear in the Windows system tray.

---

## 🧭 How to Use

### 1. How to start keyboard protection?

Keyboard protection starts automatically once the app is launched.

You can also choose:

```text
Start Protection
```

from the menu bar or system tray menu.

---

### 2. How to pause keyboard protection?

If you are gaming, testing your keyboard, or temporarily do not need automatic locking, you can pause protection.

On macOS:

```text
Click the menu bar icon → Pause Protection
```

On Windows:

```text
Right-click the system tray icon → Pause Protection
```

When protection is paused, the app will not automatically lock the keyboard.

---

### 3. How to lock the keyboard?

The keyboard can be locked automatically or manually.

Automatic detection rules include:

1. A single key is held down for a long time.
2. Two or more nearby keys are held down together.
3. Certain special keys, such as Caps Lock, are held down for a long time.

The detection durations can be adjusted in Settings.

You can also manually choose:

```text
Lock
```

to lock the keyboard immediately.

---

### 4. How to unlock the keyboard?

On macOS:

```text
Click the menu bar icon → Unlock
```

On Windows:

```text
Right-click the system tray icon → Unlock
```

You can also unlock the keyboard from the popup window when available.

The keyboard lock is automatically released when the system goes to sleep, so it will not interfere with normal usage later.

---

## 🖥️ Compatibility

Expecto Purronum currently supports:

### macOS

- macOS 13.0 or later
- Apple Silicon

### Windows

- Windows 10 or later
- 64-bit Windows

---

## 🍎 macOS Security Notice

Expecto Purronum is currently distributed outside the Mac App Store and has not yet been notarized by Apple.

When opening the app for the first time, macOS may show a warning that the developer cannot be verified.

To open it anyway, right-click `Expecto Purronum.app`, choose **Open**, then click **Open** again in the warning dialog.

If macOS still blocks the app, go to:

```text
System Settings
→ Privacy & Security
→ Open Anyway
```

---

## 🪟 Windows Security Notice

The Windows version of Expecto Purronum is not code-signed yet.

On some Windows devices, the system may show an installation or security warning depending on local security settings.  
If you downloaded the installer from this project’s GitHub Releases page, you may choose whether to continue installation.

---

## 🔐 Permissions

### macOS

Expecto Purronum may require the following macOS permissions to work properly:

- **Accessibility**
- **Input Monitoring**

You can enable these permissions in:

```text
System Settings
→ Privacy & Security
```

### Windows

The Windows version mainly runs from the system tray and stores settings locally.  
If launch at startup is enabled, the app writes the related startup configuration.

---

## 🕊️ Privacy Notice

Expecto Purronum is designed to detect whether keyboard activity looks like “a cat stepping on the keyboard”, not to record what the user types.

To provide automatic keyboard protection, the app needs to check certain keyboard event states, such as:

- Whether a key is being held down for too long
- Whether multiple nearby keys are being pressed at the same time
- Whether special keys are held abnormally
- Whether the keyboard is currently locked or unlocked

Expecto Purronum does not:

- Record the actual text you type
- Save your chats, passwords, accounts, or form content
- Upload keyboard input content to any server
- Perform cloud sync or user behavior analytics
- Collect personally identifiable information in the background

The app only uses local key-state and timing information to decide whether suspicious input may have occurred, and triggers keyboard lock protection when needed.

### Local Data

Expecto Purronum may save some app settings locally, such as:

- Language preference
- Detection timing settings
- Launch at startup preference
- Current protection-related configuration

These settings are used to keep your preferences when the app starts again.

### Launch at Startup

If launch at startup is enabled, the Windows version writes the related startup configuration for the current user so the app can start automatically after login.  
You can disable launch at startup from the app settings at any time.

---

## 🧩 Project Status

Current status:

- macOS: Released
- Windows: Released v1.1.0 🎉

Future plans:

- Improve the Windows settings interface
- Enhance multilingual support
- Improve tray status feedback
- Refine detection rules
- Add more cat-friendly interaction details 🐈
