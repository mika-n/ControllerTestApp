# ControllerTestApp

A simple test application to show joystick and gamepad events using SDL3 framework.

Install:
- Download the latest package from the [Releases](https://github.com/mika-n/ControllerTestApp/releases) web page.
- Unzip the downloaded ControllerTestApp_versionTag.zip file to any folder in your PC (for example c:\games\tools\).

Usage:
- Connect all joysticks and gamepads to PC
- Launch the ControllerTestApp.exe
- See if the console window and ControllerTestApp.txt logfile shows event activities each time you press a button or move axis on a controller.

Copyright (C) 2023-2024 mika-n. All rights reserved. www.rallysimfans.hu

Published into public domain and you are free to use this app for any purposes or modify, re-use and derive the code to create other apps.
See LICENSE.TXT file for more details.

## How to build this application?
- Visual Studio 2022 c++ compiler
- Open the ControllerTestApp.sln in Visual Studio and "Build"
- The app bundles a pre-compiled libs (statically linked) of SDL3 media library. See libsdl3 subfolder.
- Alternatively you can download and compile SDL3 libs from www.libsdl.org web site or to use my fork of SDL3 (known to work with this app version at the time of writing this) from https://github.com/mika-n/SDL web site.

-------------------------

This application uses SDL3 media library to listen joystick events. 

SDL3 license:
The Simple DirectMedia Layer (SDL for short) is a cross-platform library designed to make it easy to write multi-media software, such as games and emulators.

The Simple DirectMedia Layer library source code is available from:
https://www.libsdl.org/

This library is distributed under the terms of the zlib license:
http://www.zlib.net/zlib_license.html
