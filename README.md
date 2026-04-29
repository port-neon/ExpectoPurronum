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
- macOS 13.9 及以上的版本

- Apple Silicon
- 苹果芯片


最后...WIN端，即将上线！！！