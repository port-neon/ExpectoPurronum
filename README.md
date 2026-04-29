# Expecto Purronum
Automatic keyboard lock app that prevents accidental typing caused by cats. 
用于避免猫猫乱踩键盘的自动键盘锁程序，防止猫猫乱打字造成的突发事件。

⚠️目前仅有mac端
😊WIN端即将上线
## Overview 项目概述  
Expecto Purronum is a keyboard protection application designed for cat owners.
Expecto Purronum（呼喵护键）是帮助猫咪铲屎官的键盘守护工具。   

It monitors keyboard input in the background. When it detects input patterns that may have been caused by a cat, the app automatically triggers a keyboard lock to prevent accidental typing.
后台监测键盘输入，当检测到可能由猫猫产生的输入模式时，应用会自动触发键盘锁，防止猫猫乱打字造成误输入。

Unlike traditional manual keyboard lockers, There is no need to set the lock in advance. The app is designed to automatically respond when a cat unexpectedly steps on the keyboard while the user is away from the computer.
与传统手动键盘锁不同，不必提前主动开启锁定。实现了不在电脑面前也能自动应对猫猫突然踩键盘的情况。

- Background keyboard activity monitoring 
  后台键盘活动监测  
  
- Automatic keyboard locking when suspicious input patterns are detected   
  检测到异常输入模式时自动锁定键盘  
  
- Detection rules for long key presses and multiple nearby key presses   
  支持长按、多键相邻误触等异常输入检测规则  
  
- Menu bar status item for quick access   
  支持 macOS 菜单栏状态图标  
  
- Unlock through mouse, touchpad, or menu bar interaction   
  通过鼠标、触控板或菜单栏操作解锁  
  
- Lock screen safety handling to avoid blocking password input   
  支持锁屏状态下的安全处理，避免影响用户输入密码

## Features 功能介绍  
1. How to start keyboard protection?
   如何开启键盘守护？
- Keyboard protection starts automatically once launched and runs in the background to detect abnormal keyboard input that may be caused by cats.
  APP自打开以后就自动开启键盘守护状态，并在后台监测可能由猫猫导致的异常键盘输入。

2. How to pause keyboard protection?
   如何关闭键盘守护？
- In Settings, choose “Pause Protection”, or click the menu bar status icon and choose “Pause Protection” from the dropdown menu.
  在设置界面选择“暂停守护键盘”或者单击菜单栏状态图标，在下拉菜单里选择“暂停守护键盘”

3. How to lock the keyboard?
   如何锁定键盘？
- The keyboard locks automatically when abnormal input patterns are detected. The required hold durations can be adjusted in Settings:
  检测到异常输入模式时自动锁定键盘（键盘被按住的时长也可以在Setting界面自行设置）
		1. A single key is held down for 2.5 seconds.
		   单独一个按键长时间按住2.5秒。
		2. Two or more adjacent keys are held down for 1.0 second.
		   有2个及以上的互相临近的键盘被长时间按住1.0秒。
		3. Certain special keys, such as Caps Lock, are held down for 2.0 seconds.
		   某些特殊键（大小写转换键等）长时间被按住2.0秒。
- Keyboard Lock can also be turn on in Settings, or click the menu bar status icon and choose “Lock” from the dropdown menu.
  也可以在设置界面选择打开键盘锁或者单击菜单栏状态图标，在下拉菜单里选择“封印键盘”。

4. How to unlock the keyboard?
   如何解锁？
- Click the menu bar status icon and choose “Unlock” from the dropdown menu.
  通过单击菜单栏状态图标，在下拉菜单里选择“解除封印”。
- The keyboard lock is also released automatically when the system goes to sleep.
  在电脑系统进入睡眠模式的时候会自动解锁键盘锁。

## Compatibility 兼容性
Expecto Purronum currently supports:
Expecto Purronum目前支持：

- macOS 13.0 or later
- macOS 13.0 及以上的版本

- Apple Silicon
- 苹果芯片

## Download 下载

Download the latest macOS version from the [Releases](../../releases) page.  
请前往 [Releases](../../releases) 页面下载最新的 macOS 版本。

## Installation / 安装方式

1. Download the latest `.dmg` file.  
   下载最新的 `.dmg` 文件。

2. Open the `.dmg` file.  
   打开 `.dmg` 文件。

3. Drag `Expecto Purronum.app` into the `Applications` folder.  
   将 `Expecto Purronum.app` 拖入 `Applications` 文件夹。

4. Open the app from `Applications`.  
   从 `Applications` 文件夹中打开 App。

## macOS Security Notice macOS 安全提示

Expecto Purronum is currently distributed outside the Mac App Store and has not yet been notarized by Apple.  
Expecto Purronum 目前通过 Mac App Store 之外的方式分发，尚未经过 Apple notarization。

When opening the app for the first time, macOS may show a warning that the developer cannot be verified.  
第一次打开 App 时，macOS 可能会提示无法验证开发者。

To open it anyway, right-click `Expecto Purronum.app`, choose **Open**, then click **Open** again in the warning dialog.  
如需继续打开，请右键点击 `Expecto Purronum.app`，选择 **Open / 打开**，然后在警告弹窗中再次点击 **Open / 打开**。

If macOS still blocks the app, go to **System Settings → Privacy & Security**, then click **Open Anyway**.  
如果 macOS 仍然阻止打开，请前往 **System Settings / 系统设置 → Privacy & Security / 隐私与安全性**，然后点击 **Open Anyway / 仍要打开**。

## Permissions 权限说明

Expecto Purronum may require macOS permissions such as **Accessibility** and **Input Monitoring** to work properly.  
Expecto Purronum 可能需要 macOS 的 **Accessibility / 辅助功能** 和 **Input Monitoring / 输入监控** 权限才能正常工作。

You can enable these permissions in **System Settings → Privacy & Security**.  
你可以在 **System Settings / 系统设置 → Privacy & Security / 隐私与安全性** 中开启相关权限。

最后...WIN端，即将上线！！！
