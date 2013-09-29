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
#include "Console.h"

namespace EasyMod
{
	ConsoleApi::ConsoleApi()
	{
		this->cmds = gcnew System::Collections::Generic::List<ConsoleCommand^>();

		this->registerOwnCmds();
	}

	void ConsoleApi::registerOwnCmds()
	{
		this->RegisterConsoleCommand(gcnew ConsoleCommand("help", "Lists help information", 0, gcnew ConsoleCmdHandler(this, &ConsoleApi::cmd_help)));
		this->RegisterConsoleCommand(gcnew ConsoleCommand("cmdhelp", "Lists information on console commands.", 1, gcnew ConsoleCmdHandler(this, &ConsoleApi::cmd_cmdhelp)));
		this->RegisterConsoleCommand(gcnew ConsoleCommand("listcmds", "Lists all available console commands", 0, gcnew ConsoleCmdHandler(this, &ConsoleApi::cmd_listcmds)));
	}

	void ConsoleApi::UnloadAll()
	{
		this->cmds->Clear();
		this->registerOwnCmds();
	}

	ConsoleCommand^ ConsoleApi::findCmdByName(System::String^ Name)
	{
		for (int i = 0; i < this->cmds->Count; i++)
		{
			if (this->cmds[i]->Name == Name)
			{
				return this->cmds[i];
			}
		}

		return nullptr;
	}

	void ConsoleApi::cmd_help(System::Object^ sender, SerializableConsoleCmdEventArgs^ e)
	{
		this->Console->Print("EasyMod by tcpie.\n\n" +
							 "Help info:\n" +
							 "You are currently in the console. To get an overview of all available console commands, type '!listcmds'.\n" +
							 "To get help on a console command, type '!cmdhelp <cmdname>'. Example: '!cmdhelp listcmds'\n");
	}

	void ConsoleApi::cmd_listcmds(System::Object^ sender, SerializableConsoleCmdEventArgs^ e)
	{
		this->console->PrintLine("Number of registred commands: " + this->cmds->Count.ToString());
		this->console->PrintLine(" ");

		for (int i = 0; i < this->cmds->Count; i++)
		{
			this->console->PrintLine("!" + this->cmds[i]->Name);
		}
	}

	void ConsoleApi::cmd_cmdhelp(System::Object^ sender, SerializableConsoleCmdEventArgs^ e)
	{
		if (e->Arguments->Count == 0 || e->Arguments[0]->Length == 0)
		{
			return;
		}

		if (e->Arguments[0][0] == '!')
		{
			e->Arguments[0] = e->Arguments[0]->Remove(0, 1);
		}

		ConsoleCommand^ cmd = this->findCmdByName(e->Arguments[0]);

		if (cmd == nullptr)
		{
			this->console->PrintLine("cmdhelp: cmd '" + e->Arguments[0] + "' not found!");

			return;
		}

		System::String^ argc = (cmd->ArgCount >= 0) ? cmd->ArgCount.ToString() : "variable";

		this->console->PrintLine(e->Arguments[0] + ":");
		this->console->PrintLine("    Arg count: " + argc);
		this->console->PrintLine("    Description:");
		this->console->Print(cmd->Description);
	}

	void ConsoleApi::onConsoleCmd(System::Object^ Sender, SerializableConsoleCmdEventArgs^ e)
	{
		ConsoleCommand^ cmd = this->findCmdByName(e->CmdName);

		if (cmd == nullptr)
		{
			this->console->PrintLine("ERROR: console command '" + e->CmdName + "' not found!");

			return;
		}

		if (cmd->ArgCount != -1 && e->Arguments->Count != cmd->ArgCount)
		{
			this->console->PrintLine("ERROR: expected " + cmd->ArgCount.ToString() + " argument(s) for command '" + cmd->Name + "'");

			return;
		}

		cmd->Delegate(this, e);
	}

	bool ConsoleApi::RegisterConsoleCommand(ConsoleCommand^ Cmd)
	{
		if (this->cmds->Contains(Cmd))
		{
			return false;
		}

		this->cmds->Add(Cmd);

		return true;
	}

	bool ConsoleApi::UnregisterConsoleCommand(ConsoleCommand^ Cmd)
	{
		if (!this->cmds->Contains(Cmd))
		{
			return false;
		}

		return this->cmds->Remove(Cmd);
	}
}
