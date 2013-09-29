#include "stdafx.h"
#include "EnvironmentApi.h"

#include "WindowApi.h"

#include <vector>

namespace EasyMod
{
	struct EnumWindowsCallbackArgs 
	{
		EnumWindowsCallbackArgs( DWORD p ) : pid( p ) { }
		const DWORD pid;
		std::vector<HWND> handles;
	};

	static BOOL CALLBACK EnumWindowsCallback( HWND hnd, LPARAM lParam )
	{
		EnumWindowsCallbackArgs *args = (EnumWindowsCallbackArgs *)lParam;

		DWORD windowPID;
		GetWindowThreadProcessId(hnd, &windowPID);

		if ( windowPID == args->pid ) 
		{
			args->handles.push_back(hnd);
		}

		return TRUE;
	}

	std::vector<HWND> getToplevelWindows()
	{
		EnumWindowsCallbackArgs args(GetCurrentProcessId());

		if (EnumWindows(&EnumWindowsCallback, (LPARAM)&args) == FALSE) 
		{
		  // XXX Log error here
		  return std::vector<HWND>();
		}

		return args.handles;
	}

	EnvironmentApi::EnvironmentApi()
	{
		this->current_process = System::Diagnostics::Process::GetCurrentProcess();
		this->windows = gcnew System::Collections::Generic::List<WindowApi^>();

		HWND console_window_handle = GetConsoleWindow();
		std::vector<HWND> my_windows = getToplevelWindows();

		for (size_t i = 0; i < my_windows.size(); i++)
		{
			if (my_windows[i] == NULL)
			{
				continue;
			}

			if (my_windows[i] == console_window_handle)
			{
				this->windows->Add(gcnew WindowApi(System::IntPtr((void*)my_windows[i]), true));

				this->console_window =this->windows[this->windows->Count - 1];
			}
			else
			{
				this->windows->Add(gcnew WindowApi(System::IntPtr((void*)my_windows[i]), false));

				System::Drawing::Rectangle r = this->windows[i]->GetSizeAndPos();

				if (this->main_window == nullptr && !(r.X == -1 && r.Y == -1) && r.Width > 0 && r.Height > 0)
				{
					this->main_window = this->windows[i];
				}
			}
		}
	}
}