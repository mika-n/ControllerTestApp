//
// ControllerTestApp.cpp : A test app to show joystick activity.
// This app uses SDL3 controller library (directInput) to listen for joystick events.
// Note! Here joystick controller includes gamepads, steeringWheels and pedals also.
// 
// Copyright (c) 2023-2024 MIKA-N. All rights reserved. www.rallysimfans.hu.
// 
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR 
// PURPOSE AND NONINFRINGEMENT. USE THIS APP AT YOUR OWN RISK. IF YOU DON'T AGREE THEN
// REMOVE THIS APP AND ALL FILES WITH THE PACKAGE FROM YOUR PC IMMEDIATLY AND WALK AWAY.
// 
// THERE IS NO SUPPORT ANY KIND FOR THIS APPLICATION. IF IT DOESN'T WORK THEN COULD BE
// IT IS TOO OLD TO WORK IN PRESENT DAY SYSTEMS.
// 
// If this app burns your house or fries a gamepad or joystick then you are responsible 
// of the buggy house and a buggy gamepad/joystick device.
// 
// License: This app is released as an open source app and you can use and modify this app 
//          or use portions of this code in another applications as long you mention in credits
//          where you got the idea and code snippets. You don't have to publish the derived
//          work as an open-source application, but certainly it would be highly appreciated by
//          open source communiities.
//

#define WIN32_LEAN_AND_MEAN // Exclude rarely-used stuff from Windows headers
#include <windows.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#pragma warning(push)
#pragma warning(disable : 0135) // Ignore warning about "unannotated switch case fallthrough". Yes, we know about it and don't care in SDL.h
#pragma warning(disable : 26819)
#include <SDL3/SDL.h>
#pragma warning(pop)

#ifdef _DEBUG
#pragma comment(lib, "SDL3_MTd.lib")
#else
#pragma comment(lib, "SDL3_MT.lib")
#endif

// SDL3 requires these libs
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "Imm32.lib")
#pragma comment(lib, "Cfgmgr32.lib")

#define STR_VERSION "1.9"

#define BIND_NAME_MAXLEN 128


/// <summary>
/// Return the root path of this process. The logfile will be created in this folder.
/// </summary>
/// <returns></returns>
static const std::string& GetRootPath()
{
	static std::string sRootPath;
	if (sRootPath.empty())
	{
		char szModulePath[_MAX_PATH * 2] = { 0 };
		::GetModuleFileNameA(NULL, szModulePath, sizeof(szModulePath) - 1);
		sRootPath = std::filesystem::path(szModulePath).parent_path().string();
	}
	return sRootPath;
}


/// <summary>
/// Quick and dirty logger command. Dump out a log message both on screen and logfile.
/// </summary>
/// <param name="msg"></param>
static void InfoLog(const std::string& msg)
{
	static std::ofstream logFile;

	char szTimeText[16]{ 0 };
	_strtime_s(szTimeText, 16);

	std::cout << "[" << szTimeText << "] " << msg << std::endl;

	if (!logFile.is_open())
	{
		try
		{
			logFile.open(GetRootPath() + "\\ControllerTestApp.txt");
		}
		catch (...)
		{
			// Eat all exceptions if for some reason the logfile opening fails. In this case msg is shown on console screen only
		}
	}

	if (logFile.is_open())
	{
		logFile << "[" << szTimeText << "] " << msg << std::endl;
		logFile.flush();
	}
}


/// <summary>
/// Hotplug a new joystick and listen button events from it. Or remove a disconnected joystick device handle.
/// </summary>
/// <param name="contrList"></param>
/// <param name="event"></param>
static void AddOrRemoveJoystick(std::vector<SDL_Joystick*>& contrList, const SDL_Event& event)
{
	// Try to find an existing instance of the controller from the list
	SDL_Joystick* sdlContr = nullptr;
	for (auto& contr : contrList)
	{
		if (contr != nullptr && SDL_GetJoystickInstanceID(contr) == event.jdevice.which)
		{
			sdlContr = contr;
			break;
		}
	}

	// Add or remove the controller connection
	if (event.type == SDL_EVENT_JOYSTICK_ADDED)
	{
		if (sdlContr == nullptr)
		{		
			sdlContr = SDL_OpenJoystick(event.jdevice.which);
			contrList.push_back(sdlContr);
			InfoLog("Connected joyID" + std::to_string(event.jdevice.which) + " " + SDL_GetJoystickName(sdlContr));
		}
		else
		{
			InfoLog("Warning. Tried to re-connect an existing joyID" + std::to_string(event.jdevice.which));
		}
	}
	else if (event.type == SDL_EVENT_JOYSTICK_REMOVED)
	{
		if (sdlContr != nullptr)
		{
			InfoLog("Disconnected joyID#" + std::to_string(event.jdevice.which));
			contrList.erase(std::remove(contrList.begin(), contrList.end(), sdlContr), contrList.end());
			SDL_CloseJoystick(sdlContr);
		}
		else
		{
			InfoLog("Warning. Tried to disconnect a non-existing joyID" + std::to_string(event.jdevice.which));
		}
	}

}

// Gamepad interface. Not used at the moment because all gamepads are also joysticks. 
// GamePad interface would have more user friendly button names (those familiar xbox and dualShock button names), but we don't care about those names in this test app.
static void AddOrRemoveGamepad(std::vector<SDL_Gamepad*>& contrList, const SDL_Event& event)
{
	// Try to find an existing instance of the controller from the list
	SDL_Gamepad* sdlContr = nullptr;
	for (auto& contr : contrList)
	{
		if (contr != nullptr && SDL_GetGamepadInstanceID(contr) == event.jdevice.which)
		{
			sdlContr = contr;
			break;
		}
	}
	
	// Add or remove the controller connection
	if (event.type == SDL_EVENT_GAMEPAD_ADDED)
	{
		if (sdlContr == nullptr)
		{
			sdlContr = SDL_OpenGamepad(event.jdevice.which);
			contrList.push_back(sdlContr);
			InfoLog("Connected gamepadID" + std::to_string(event.jdevice.which) + " " + SDL_GetGamepadName(sdlContr));
		}
		else
		{
			InfoLog("Warning. Tried to re-connect an existing gamepadID" + std::to_string(event.jdevice.which));
		}
	}
	else if (event.type == SDL_EVENT_GAMEPAD_REMOVED)
	{
		if (sdlContr != nullptr)
		{
			InfoLog("Disconnected gamepadID#" + std::to_string(event.jdevice.which));
			contrList.erase(std::remove(contrList.begin(), contrList.end(), sdlContr), contrList.end());
			SDL_CloseGamepad(sdlContr);
		}
		else
		{
			InfoLog("Warning. Tried to disconnect a non-existing gamepadID" + std::to_string(event.jdevice.which));
		}
	}
}


/// <summary>
/// Return the bind displayName of the controller button/axis/hat
/// </summary>
/// <param name="event"></param>
/// <returns></returns>
static const char* GetBindName(const SDL_Event& event)
{
	static char szLatestKeyName[BIND_NAME_MAXLEN] = { 0 };
	static SDL_JoystickID latestJoyID = 0;
	static Uint8 latestButton = 0xFF;

	static char szLatestAxisName[BIND_NAME_MAXLEN] = { 0 };
	static SDL_JoystickID latestAxisJoyID = 0;
	static Uint8 latestAxis = 0xFF;

	static char szLatestHatName[BIND_NAME_MAXLEN] = { 0 };
	static SDL_JoystickID latestHatJoyID = 0;
	static Uint8 latestHat = 0xFF;
	static Uint8 latestHatValue = SDL_HAT_CENTERED;

	static char szLatestScanCodeName[BIND_NAME_MAXLEN] = { 0 };
	static SDL_Scancode latestKeyScanCode = SDL_SCANCODE_UNKNOWN;

	if (event.type >= SDL_EventType::SDL_EVENT_JOYSTICK_BUTTON_DOWN || event.type == SDL_EventType::SDL_EVENT_JOYSTICK_BUTTON_UP)
	{
		// Joy button key down/up event
		if (latestButton != event.jbutton.button || latestJoyID != event.jbutton.which)
		{
			snprintf(szLatestKeyName, sizeof(szLatestKeyName), "#%d %s",
				event.jbutton.button,
				SDL_GetJoystickName(SDL_GetJoystickFromInstanceID(event.jbutton.which))
			);

			latestButton = event.jbutton.button;
			latestJoyID = event.jbutton.which;
		}
		return szLatestKeyName;
	}
	else if (event.type == SDL_EventType::SDL_EVENT_JOYSTICK_HAT_MOTION)
	{
		// Hat movement event
		/*
		static const char* szHatDirectionName[] {
			"CENTERED",			// 00 = No HAT direction (centered)
			"UP",				// 01
			"RIGHT",			// 02
			"RIGHTUP",			// 03
			"DOWN",				// 04
			"UPDOWN",			// 05 UP+DOWN bitflag value. Should not be possible
			"RIGHTDOWN",		// 06
			"UPDOWNRIGHT",		// 07 UP+DOWN+RIGHT bitflag value. Should not be possible
			"LEFT",				// 08
			"LEFTUP",			// 09
			"RIGHTLEFT",		// 10 RIGHT+LEFT bitflag value. Should not be possible
			"RIGHTLEFTUP",		// 11 RIGHT+LEFT+UP bitflag value. Should not be possible
			"LEFTDOWN",			// 12
			"UNKNOWN"			// 13 Unknown fallback axis name for everything hat.value>12
		};
		*/

		if (latestHat != event.jhat.hat || latestHatJoyID != event.jhat.which || latestHatValue != event.jhat.value)
		{
			snprintf(szLatestHatName, sizeof(szLatestHatName), "H%d.%d %s",
				event.jhat.hat,
				event.jhat.value,   //szHatDirectionName[(event.jhat.value <= 12 ? event.jhat.value : 13)]
				SDL_GetJoystickName(SDL_GetJoystickFromInstanceID(event.jhat.which))
			);

			latestHatValue = event.jhat.value;
			latestHat = event.jhat.hat;
			latestHatJoyID = event.jhat.which;
		}
		return szLatestHatName;
	}
	else if (event.type == SDL_EventType::SDL_EVENT_KEY_DOWN || event.type == SDL_EventType::SDL_EVENT_KEY_UP)
	{
		if (latestKeyScanCode != event.key.keysym.scancode)
		{
			snprintf(szLatestScanCodeName, sizeof(szLatestScanCodeName), "K%d Keyboard", event.key.keysym.scancode);
			latestKeyScanCode = event.key.keysym.scancode;
		}
		return szLatestScanCodeName;
	}
	else if (event.type == SDL_EventType::SDL_EVENT_JOYSTICK_AXIS_MOTION)
	{
		if (latestAxis != event.jaxis.axis || latestAxisJoyID != event.jaxis.which)
		{
			snprintf(szLatestAxisName, sizeof(szLatestAxisName), "A%d %s",
				event.jaxis.axis,
				SDL_GetJoystickName(SDL_GetJoystickFromInstanceID(event.jaxis.which))
			);

			latestAxis = event.jaxis.axis;
			latestAxisJoyID = event.jaxis.which;
		}
		return szLatestAxisName;
	}
	else
	{
		// Unknown event. should never come here
		InfoLog("GetBindName. Unknown SDL event type");
		return "";
	}
}


/// <summary>
/// MAIN - Start listening joystick events and dump out button/axis/hat events to a console screen and logfile.
/// </summary>
/// <returns></returns>
int main()
{
	std::vector<std::pair<int,int>> logMsgIgnoredEventID; // EventIDs of those SDL joystick events which are no longer shown in the log file (ie. to avoid flooding it)
	bool ignoreLogMsg = false;

	SDL_Event event;

    std::cout << "Controller test app to show DirectInput joystick/gamepad/controller events." << std::endl;
	std::cout << "This application uses SDL3 www.libsdl.org media library to talk with directIput devicse." << std::endl;
	std::cout << "Please visit www.libsdl.org web page for more info about SDL. Credits of SDL goes there." << std::endl;
	std::cout << "" << std::endl;
    std::cout << "Press controller buttons and move analog controls one by one and see if the app can detect the activity." << std::endl;
    std::cout << "Please provide the logfile for admins for closer analyzing." << std::endl;
	std::cout << "Logfile=" + GetRootPath() + "\\ControllerTestApp.txt" << std::endl;
	std::cout << "AppVersion " << STR_VERSION << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Press ESC keyboard key to close this app." << std::endl;
	std::cout << "" << std::endl;

	if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO) != 0)
	{
		InfoLog("Failed to initialize SDL system");
		return -1;
	}

	InfoLog("AppVersion " STR_VERSION);

	std::vector<SDL_Joystick*> joys;
	joys.reserve(10);
	SDL_SetJoystickEventsEnabled(SDL_bool::SDL_TRUE);

	int numOfObjs = 0;
	SDL_JoystickID* joyIDArray = SDL_GetJoysticks(&numOfObjs);
	InfoLog("Bootup Detected " + std::to_string(numOfObjs) + " joysticks");
	if (joyIDArray != nullptr)
	{
		SDL_free(joyIDArray);
	}

	// Note SDL can treat gamepads also as a joystick device. Gamepad is just a special case of joystick device, so we're not interested in "duplicated" GamePad events in this app
	/*
	std::vector<SDL_Gamepad*> gamepads;
	gamepads.reserve(10);
	SDL_SetGamepadEventsEnabled(SDL_bool::SDL_TRUE);

	numOfObjs = 0;
	joyIDArray = SDL_GetGamepads(&numOfObjs);
	InfoLog("Bootup Detected " + std::to_string(numOfObjs) + " gamepads");
	if (joyIDArray != nullptr)
	{
		SDL_free(joyIDArray);
	}
	*/


	//
	// Start listening for SDL joystick/gamepad/controller events.
	// Keyboard ESC key quits this app
	//
	while ((GetAsyncKeyState(VK_ESCAPE) & 0x8000) == 0)
	{
		::Sleep(200);

		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_EventType::SDL_EVENT_JOYSTICK_ADDED:
			case SDL_EventType::SDL_EVENT_JOYSTICK_REMOVED:
				// Handle hotplugged joysticks
				AddOrRemoveJoystick(joys, event);
				break;

			case SDL_EVENT_JOYSTICK_AXIS_MOTION:
				// Use half of the range as deadzone to avoid flooding the logfile with analog axis data
				if (std::abs(event.jaxis.value) >= 32767/2)
				{
					InfoLog("joyID=" + std::to_string(event.jdevice.which)
						+ "  EventType=" + std::to_string(event.type)
						+ "  AxisID=" + std::to_string(event.jaxis.axis) 
						+ "  AxisValue=" + std::to_string(event.jaxis.value)
						+ "  BindName=" + GetBindName(event)
					);
				}
				break;


			case SDL_EventType::SDL_EVENT_JOYSTICK_HAT_MOTION:
				// Use half of the range as deadzone to avoid flooding the logfile with analog axis data
				if (event.jhat.value != SDL_HAT_CENTERED)
				{
					InfoLog("joyID=" + std::to_string(event.jhat.which)
						+ "  EventType=" + std::to_string(event.type)
						+ "  HatID=" + std::to_string(event.jhat.hat)
						+ "  HatValue=" + std::to_string(event.jhat.value)
						+ "  BindName=" + GetBindName(event)
					);
				}
				break;

			case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
			case SDL_EVENT_JOYSTICK_BUTTON_UP:
				InfoLog("joyID=" + std::to_string(event.jdevice.which)
					+ "  EventType=" + std::to_string(event.type)
					+ "  btnID=" + std::to_string(event.jbutton.button)
					+ "  btnState=" + std::to_string(event.jbutton.state)
					+ "  BindName=" + GetBindName(event)
				);
				break;


			/*
			case SDL_EventType::SDL_EVENT_GAMEPAD_ADDED:
			case SDL_EventType::SDL_EVENT_GAMEPAD_REMOVED:
				// Handle hotplugged gamepads (usually the gamepad obj is handled also as joystick obj)
				AddOrRemoveGamepad(gamepads, event);
				break;

			case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			case SDL_EVENT_GAMEPAD_BUTTON_UP:
				InfoLog("gamepadID=" + std::to_string(event.jdevice.which)
					+ "  EventType=" + std::to_string(event.type)
					+ "  btnID=" + std::to_string(event.gbutton.button)
					+ "  btnState=" + std::to_string(event.gbutton.state)
				);
				break;

			case SDL_EVENT_GAMEPAD_AXIS_MOTION:
				// Use half of the range as deadzone to avoid flooding the logfile with analog axis data
				if (std::abs(event.gaxis.value) >= 32767/2)
				{
					InfoLog("gamepadID=" + std::to_string(event.jdevice.which)
						+ "  EventType=" + std::to_string(event.type)
						+ "  AxisID=" + std::to_string(event.gaxis.axis)
						+ "  AxisValue=" + std::to_string(event.gaxis.value)
					);
				}
				break;
			*/


			default:
				ignoreLogMsg = false;
				for (auto& item : logMsgIgnoredEventID)
				{
					if (item.first == static_cast<int>(event.jdevice.which) && item.second == static_cast<int>(event.type))
					{
						// This msg has been logged already. Do not flood the logfile with the same "unknown" event
						ignoreLogMsg = true;
						break;
					}
				}

				if (ignoreLogMsg == false)
				{
					InfoLog("deviceID=" + std::to_string(event.jdevice.which) + "  EventType=" + std::to_string(event.type));

					logMsgIgnoredEventID.push_back(std::make_pair(static_cast<int>(event.jdevice.which), static_cast<int>(event.type)));
				}
				break;
			} 
		}

		if (event.type == SDL_EventType::SDL_EVENT_QUIT)
		{
			break;
		}
	}

	InfoLog("The app is closing...");

	// Make sure joys and gamepads list is empty in case xxx_REMOVED sdl event was never sent
	for (auto& joy : joys)
	{
		try
		{
			if (joy != nullptr)
				SDL_CloseJoystick(joy);
		}
		catch (...)
		{
			// Eat all exceptions
		}
	}

	/*
	for (auto& gamepad : gamepads)
	{
		try
		{
			if (gamepad != nullptr)
				SDL_CloseGamepad(gamepad);
		}
		catch (...)
		{
			// Eat all exceptions
		}
	}
	*/

	SDL_Quit();

	return 0;
}
