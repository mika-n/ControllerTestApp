//
// ControllerTestApp.cpp : A test app to show joystick/gamepad/controller activity using SDL controller library
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


const std::string& GetRootPath()
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


void InfoLog(const std::string& msg)
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


void AddOrRemoveJoystick(std::vector<SDL_Joystick*>& contrList, const SDL_Event& event)
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

void AddOrRemoveGamepad(std::vector<SDL_Gamepad*>& contrList, const SDL_Event& event)
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


int main()
{
	SDL_Event event;

    std::cout << "Controller test app to show joystick/gamepad/controller events." << std::endl;
    std::cout << "Press a controller buttons one by one and see if the app can detect the activity." << std::endl;
    std::cout << "Please provide the logfile for admins for closer analyzing." << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Press ESC keyboard key to close this app." << std::endl;
	std::cout << "" << std::endl;

	if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD | SDL_INIT_VIDEO) != 0)
	{
		InfoLog("Failed to initialize SDL system");
		return -1;
	}

	//SDL_Window* sdlWindow = SDL_CreateWindowFrom(someWnd);
	//SDL_Window* sdlWindow = SDL_CreateWindow("ControllerTestAppWnd", 500, 200, 0);
	SDL_SetJoystickEventsEnabled(SDL_bool::SDL_TRUE);
	SDL_SetGamepadEventsEnabled(SDL_bool::SDL_TRUE);

	std::vector<SDL_Joystick*> joys;
	std::vector<SDL_Gamepad*> gamepads;  // Note SDL can treat gamepads also as a joystick device. Gamepad is just a special case of joystick device
	joys.reserve(10);
	gamepads.reserve(10);

	int numOfObjs = 0;
	SDL_JoystickID* joyIDArray = SDL_GetJoysticks(&numOfObjs);
	InfoLog("Bootup Detected " + std::to_string(numOfObjs) + " joysticks");
	if (joyIDArray != nullptr)
		SDL_free(joyIDArray);

	numOfObjs = 0;
	joyIDArray = SDL_GetGamepads(&numOfObjs);
	InfoLog("Bootup Detected " + std::to_string(numOfObjs) + " gamepads");
	if (joyIDArray != nullptr)
		SDL_free(joyIDArray);
	
	//
	// Start listeng for SDL joystick/gamepad/controller events
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

			case SDL_EventType::SDL_EVENT_GAMEPAD_ADDED:
			case SDL_EventType::SDL_EVENT_GAMEPAD_REMOVED:
				// Handle hotplugged gamepads (usually the gamepad obj is handled also as joystick obj)
				AddOrRemoveGamepad(gamepads, event);
				break;

			case SDL_EVENT_JOYSTICK_AXIS_MOTION:
				// Use half of the range as deadzone to avoid flooding the logfile with analog axis data
				if (std::abs(event.jaxis.value) >= 32767/2)
				{
					InfoLog("joyID=" + std::to_string(event.jdevice.which)
						+ "  EventType=" + std::to_string(event.type)
						+ "  AxisID=" + std::to_string(event.jaxis.axis) 
						+ "  AxisValue=" + std::to_string(event.jaxis.value)
					);
				}
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

			case SDL_EVENT_JOYSTICK_BUTTON_DOWN:
			case SDL_EVENT_JOYSTICK_BUTTON_UP:
				InfoLog("joyID=" + std::to_string(event.jdevice.which)
					+ "  EventType=" + std::to_string(event.type)
					+ "  btnID=" + std::to_string(event.jbutton.button)
					+ "  btnState=" + std::to_string(event.jbutton.state)
				);
				break;

			case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
			case SDL_EVENT_GAMEPAD_BUTTON_UP:
				InfoLog("gamepadID=" + std::to_string(event.jdevice.which)
					+ "  EventType=" + std::to_string(event.type)
					+ "  btnID=" + std::to_string(event.gbutton.button)
					+ "  btnState=" + std::to_string(event.gbutton.state)
				);
				break;

			default:
				InfoLog("deviceID=" + std::to_string(event.jdevice.which) + "  EventType=" + std::to_string(event.type));
				break;
			} 
		}

		if (event.type == SDL_EventType::SDL_EVENT_QUIT)
			break;
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

	//SDL_DestroyWindow(sdlWindow);
	SDL_Quit();

	return 0;

}

