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

#include "stdafx.h"
#include "Controller.h"
#include "KeywordEngine.h"

#include "GraphicsApi.h"

namespace EasyMod
{
	void InitializeMixedMode()
	{
		if (Storage::controller == nullptr)
		{
			Storage::controller = gcnew Controller("easymod_scripts_appdomain", ".\\scripts");
			Storage::controller->Startup();
		}
	}

	Controller::Controller(System::String^ ScriptsAppDomainName, System::String^ ScriptsDirectory)
	{
		this->state = ControllerState::Startup;
		System::Windows::Forms::MessageBox::Show("Controller startup. Current AppDomain: " + System::AppDomain::CurrentDomain->FriendlyName + " id: " + System::AppDomain::CurrentDomain->Id);
		System::Console::WriteLine("Startup :-)");

		this->scripts_appdomain_name = ScriptsAppDomainName;
		this->scripts_directory = ScriptsDirectory;

		this->key_watcher = gcnew GTA::KeyWatchDog();
		this->scripts = gcnew System::Collections::Generic::List<Script^>();
		this->keyword_engine = gcnew KeywordEngine();
		this->d3d9_handler = gcnew D3D9Handler();

		this->keyword_reloadscripts = "reloadscripts";
		this->keyword_unloadscripts = "unloadscripts";
		this->keyword_listscripts   = "listscripts";

		this->client_api = gcnew EasyModApi(this->keyword_engine, gcnew GraphicsApi(this->d3d9_handler), gcnew ConsoleApi());

		this->script_loader = gcnew ScriptLoader(this->scripts_directory, this->scripts_appdomain_name, this->client_api);

		this->timer = gcnew System::Timers::Timer();
		this->timer->Interval = 1;
		this->timer->Start();
		this->timer->Elapsed += gcnew System::Timers::ElapsedEventHandler(this, &Controller::timerElapsed);			
	}

	void Controller::timerElapsed(Object^ source, System::Timers::ElapsedEventArgs^ e)
	{
		this->key_watcher->Check();
	}

	void Controller::Startup()
	{
		this->keyword_engine->KeywordEntered += gcnew KeywordEventHandler(this, &Controller::keywordEntered);
		this->keyword_engine->AddKeyword(this->keyword_reloadscripts);
		this->keyword_engine->AddKeyword(this->keyword_unloadscripts);
		this->keyword_engine->AddKeyword(this->keyword_listscripts);

		this->key_watcher->KeyUp   += gcnew KeyEventHandler(this, &Controller::keyUp);
		this->key_watcher->KeyDown += gcnew KeyEventHandler(this, &Controller::keyDown);

		this->console_switch_key  = System::Windows::Forms::Keys::F2;

		// Check D3D9:
		if (GetModuleHandleA("d3d9.dll") != NULL)
		{
			// We can probably use DirectX 9 :-)
			System::Console::WriteLine("d3d9.dll loaded. Let's try to get DirectX support...");

			this->initD3D9();
		}
		else
		{
			System::Console::WriteLine("Current app does not seem to use DirectX 9");
		}

		System::Collections::Generic::List<Script^>^ loaded_scripts = this->script_loader->LoadScripts();

		if (loaded_scripts != nullptr)
		{
			Script^ console = this->findConsoleScriptInScripts(loaded_scripts);
			
			if (console != nullptr)
			{
				this->the_console_script = console;
				this->the_console = (IConsole^)console;
				this->the_console->ConsoleCmd += gcnew ConsoleCmdHandler(this, &Controller::consoleCmd);

				this->client_api->Console->Console = this->the_console;
				this->client_api->Console->ConsoleScript = this->the_console_script;
			}

			this->scripts->AddRange(loaded_scripts);
			this->initAllScripts();
		}

		this->state = ControllerState::Running;
	}

	void Controller::initD3D9()
	{		
		bool success = this->d3d9_handler->SetHooks();

		if (!success)
		{
			System::Console::WriteLine("DX hooking failed!");
			return;
		}

		System::Console::WriteLine("DX hooking succeeded!");

		this->d3d9_handler->EndScene += gcnew InternalD3D9EventHandler(this, &Controller::endScene);
	}

	void Controller::endScene(System::Object^ sender, InternalD3D9EventArgs^ e)
	{
		if (this->state == ControllerState::Running)
		{
			this->client_api->Graphics->onEndScene();
		}
	}

	void Controller::PrintLoadedScriptsInfo()
	{
		System::Console::WriteLine("Loaded scripts: (total count: " + this->scripts->Count + ")");
		
		for (int i = 0; i < this->scripts->Count; i++)
		{
			System::Console::Write(i + ": \"" + this->scripts[i]->Name + "\": \"" + this->scripts[i]->Description + "\"");
		}
	}

	void Controller::notifyScriptsOfEvent(ScriptEvent^ Event, bool NotifyConsole)
	{
		if (this->the_console_script != nullptr && NotifyConsole)
		{
			this->the_console_script->handleEvent(Event);
		}

		for (int i = 0; i < this->scripts->Count; i++)
		{
			this->scripts[i]->handleEvent(Event);
		}
	}

	void Controller::initAllScripts()
	{
		for (int i = 0; i < this->scripts->Count; i++)
		{
			this->initScript(this->scripts[i]);
		}
	}

	void Controller::initScript(Script^ script)
	{
	}

	Script^ Controller::findConsoleScriptInScripts(System::Collections::Generic::List<Script^>^% Scripts)
	{
		// Check if there is a console specified
		for (int i = 0; i < Scripts->Count; i++)
		{
			if (IConsole::typeid->IsAssignableFrom(Scripts[i]->GetType()))
			{
				Script^ ret_val = Scripts[i];
				Scripts->RemoveAt(i);
				
				return ret_val;
			}
		}

		return nullptr;
	}

	void Controller::handleUnloadScripts()
	{
		this->state = ControllerState::Loading_Unloading;

		if (!this->waitForSafeUnload())
		{
			return;
		}

		this->scripts->Clear();
		this->client_api->unloadAll();
		this->the_console = nullptr;
		this->the_console_script = nullptr;
		this->script_loader->UnloadAllScripts(this->script_loader->GetScriptsAppDomain());
		this->client_api->unloadAll();

		System::Console::WriteLine("Unloading completed");
		this->state = ControllerState::Running;
	}

	void Controller::handleReloadScripts()
	{
		this->state = ControllerState::Loading_Unloading;

		if (!this->waitForSafeUnload())
		{
			return;
		}

		this->scripts->Clear();		
		this->client_api->unloadAll();
		this->the_console = nullptr;
		this->the_console_script = nullptr;
		this->script_loader->UnloadAllScripts(this->script_loader->GetScriptsAppDomain());
		this->client_api->unloadAll();

		System::Collections::Generic::List<Script^>^ loaded_scripts = this->script_loader->LoadScripts();
			
		if (loaded_scripts != nullptr)
		{
			Script^ console = this->findConsoleScriptInScripts(loaded_scripts);
			
			if (console != nullptr)
			{
				this->the_console_script = console;
				this->the_console = (IConsole^)console;
				this->the_console->ConsoleCmd += gcnew ConsoleCmdHandler(this, &Controller::consoleCmd);

				this->client_api->Console->Console = this->the_console;
				this->client_api->Console->ConsoleScript = this->the_console_script;
			}

			this->scripts->AddRange(loaded_scripts);
			this->initAllScripts();
		}

		this->state = ControllerState::Running;
	}

	bool Controller::waitForSafeUnload()
	{
		for (int i = 0; i < 5000; i += 100)
		{
			if (!this->client_api->Graphics->IsInFrame)
			{
				System::Threading::Thread::CurrentThread->Sleep(100);

				return true;
			}

			System::Threading::Thread::CurrentThread->Sleep(100);
		}

		return false;
	}
	
	void Controller::consoleCmd(System::Object^ sender, SerializableConsoleCmdEventArgs^ e)
	{
		ScriptEvent^ script_event = gcnew ScriptEvent(e, EventType::ConsoleCmdEvent);
		this->notifyScriptsOfEvent(script_event, true);
	}

	void Controller::keywordEntered(System::Object^ sender, KeywordEventArgs^ e)
	{
		if (e->Keyword == this->keyword_reloadscripts)
		{
			this->handleReloadScripts();
		}
		else if (e->Keyword == this->keyword_unloadscripts)
		{
			this->handleUnloadScripts();
		}
		else if (e->Keyword == this->keyword_listscripts)
		{
			this->PrintLoadedScriptsInfo();
		}

		if (this->state == ControllerState::Running)
		{
			this->notifyScriptsOfEvent(gcnew ScriptEvent(e, EventType::KeywordEvent), true);
		}
	}

	void Controller::keyUp(System::Object^ sender, SerializableKeyEventArgs^ e)
	{
		if (this->state == ControllerState::Running)
		{
			this->notifyScriptsOfEvent(gcnew ScriptEvent(e, EventType::KeyUpEvent), true);
		}
	}

	void Controller::keyDown(System::Object^ sender, SerializableKeyEventArgs^ e)
	{
		if (this->state == ControllerState::Running)
		{
			this->keyword_engine->keyDown(sender, e);

			if (e->Key == this->console_switch_key)
			{
				if (this->the_console->IsOpen())
				{
					this->the_console->Close();
				}
				else
				{
					this->the_console->Open();
				}

				this->notifyScriptsOfEvent(gcnew ScriptEvent(e, EventType::KeyDownEvent), false);
			}
			else
			{
				this->notifyScriptsOfEvent(gcnew ScriptEvent(e, EventType::KeyDownEvent), true);
			}
		}
	}
}
