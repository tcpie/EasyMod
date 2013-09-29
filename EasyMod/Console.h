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

#ifndef _ICONSOLE_H_
#define _ICONSOLE_H_

#include "Events.h"
#include "Script.h"

namespace EasyMod
{
	ref class EasyModApi;

	public ref class ConsoleArgs
	{
	private:
		array<System::String^>^ args;

	public:
		ConsoleArgs(array<System::String^>^ Args) { this->args = Args; }
	};

	public ref class ConsoleCommand : System::MarshalByRefObject
	{
	private:
		System::String^ name;
		System::String^ description;
		int argc;

		ConsoleCmdHandler^ my_delegate;

	public:
		ConsoleCommand(System::String^ Name, System::String^ Description, int ArgCount, ConsoleCmdHandler^ Delegate) 
		{ 
			this->name = Name; 
			this->description = Description;
			this->argc = ArgCount;
			this->my_delegate = Delegate;
		}

		property System::String^ Name { System::String^ get() { return this->name; } }

		property System::String^ Description { System::String^ get() { return this->description; } }

		property int ArgCount { int get() { return this->argc; } }

		property ConsoleCmdHandler^ Delegate { ConsoleCmdHandler^ get() { return this->my_delegate; } }
	};

	public interface class IConsole
	{
	public:
		event ConsoleCmdHandler^ ConsoleCmd;

		virtual bool IsOpen();
		virtual void Open();
		virtual void Close();

		virtual void Print(System::String^ Text);
		virtual void PrintLine(System::String^ Line);
	};

	public ref class ConsoleApi : System::MarshalByRefObject
	{
	private:
		Script^ console_script;
		IConsole^ console;
		System::Collections::Generic::List<ConsoleCommand^>^ cmds;

		ConsoleCommand^ findCmdByName(System::String^ Name);
		void onConsoleCmd(System::Object^ Sender, SerializableConsoleCmdEventArgs^ e);

		void registerOwnCmds();

		void cmd_cmdhelp(System::Object^ sender, SerializableConsoleCmdEventArgs^ e);
		void cmd_listcmds(System::Object^ sender, SerializableConsoleCmdEventArgs^ e);

	internal:
		ConsoleApi();

		void UnloadAll();

		property Script^ ConsoleScript 
		{
			Script^ get() { return this->console_script; }
			void set(Script^ value) { this->console_script = value; }
		}

		property IConsole^ Console
		{
			IConsole^ get() { return this->console; }
			void set(IConsole^ value) { this->console = value; value->ConsoleCmd += gcnew ConsoleCmdHandler(this, &ConsoleApi::onConsoleCmd);}
		}
	public:
		property bool IsValid { bool get() { return this->console != nullptr && this->console_script != nullptr; } }

		bool RegisterConsoleCommand(ConsoleCommand^ Cmd);
		bool UnregisterConsoleCommand(ConsoleCommand^ Cmd);
	};	
};

#endif
