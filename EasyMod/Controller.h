/*
Copyright (c) 2013 Stijn "tcpie" Hinterding (contact: contact at tcpie dot eu)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published 
by the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include "KeyWatchDog.h"
#include "Script.h"
#include "Events.h"
#include "KeywordEngine.h"
#include "Serializable.h"
#include "ScriptLoader.h"
#include "EasyModApi.h"
#include "D3D9Handler.h"
#include "Console.h"

#pragma managed

namespace EasyMod
{
	enum class ControllerState
	{
		Running = 0,
		Loading_Unloading = 1,
		Startup = 2,
	};

	ref class Controller : System::MarshalByRefObject
	{
	private:
		// --- Helpers
		GTA::KeyWatchDog^ key_watcher;
		KeywordEngine^ keyword_engine;
		ScriptLoader^ script_loader;
		D3D9Handler^ d3d9_handler;

		EasyModApi^ client_api;

		IConsole^ the_console;
		Script^ the_console_script;

		// --- Storage
		System::Collections::Generic::List<Script^>^ scripts;
		System::Timers::Timer^ timer;

		// --- Parameters
		System::String^ scripts_directory;
		System::String^ scripts_appdomain_name;

		System::String^ keyword_reloadscripts;
		System::String^ keyword_unloadscripts;
		System::String^ keyword_listscripts;

		System::Windows::Forms::Keys console_switch_key;
		ControllerState state;

		// --- Regular private methods
		void notifyScriptsOfEvent(ScriptEvent^ Event, bool NotifyConsole);
		void initScript(Script^ script);
		void initAllScripts();
		void initD3D9();
		bool waitForSafeUnload();
		void handleUnloadScripts();
		void handleReloadScripts();
		Script^ findConsoleScriptInScripts(System::Collections::Generic::List<Script^>^% Scripts);

		// --- Delegates
		void timerElapsed(Object^ source, System::Timers::ElapsedEventArgs^ e);
		void keyUp(System::Object^ sender, SerializableKeyEventArgs^ e);
		void keyDown(System::Object^ sender, SerializableKeyEventArgs^ e);
		void keywordEntered(System::Object^ sender, KeywordEventArgs^ e);
		void consoleCmd(System::Object^ sender, SerializableConsoleCmdEventArgs^ e);
		void endScene(System::Object^ sender, InternalD3D9EventArgs^ e);
	internal:
		// --- Constructor
		Controller(System::String^ ScriptsAppDomainName, System::String^ ScriptsDirectory);

		// --- Internal methods
		void Startup();	
		void PrintLoadedScriptsInfo();
	};

	ref class Storage
	{
	public:
		static Controller^ controller;
	};

	extern "C"
	{
		__declspec(dllexport) void InitializeMixedMode();
	}
}

#endif // _CONTROLLER_H_
