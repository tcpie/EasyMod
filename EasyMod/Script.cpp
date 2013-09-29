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
#include "Script.h"
#include "Controller.h"
#include "KeyWatchDog.h"
#include "EasyModApi.h"
#include "Console.h"

#pragma managed

namespace EasyMod
{
	Script::Script(EasyModApi^ ApiInterface)
	{
		System::Windows::Forms::MessageBox::Show("Script ctor!");

		if (this->name == System::String::Empty)
		{
			this->name = this->GetType()->FullName;
		}

		this->api = ApiInterface;

		this->timer = gcnew System::Timers::Timer();
		this->timer->Interval = 1;		
		this->timer->Start();
		this->timer->Elapsed += gcnew System::Timers::ElapsedEventHandler(this, &Script::timerElapsed);
	}

	void Script::onTick(Object^ sender, System::Threading::Thread^ CurrentThread)
	{
		this->tick(sender, CurrentThread);
	}

	void Script::timerElapsed(Object^ source, System::Timers::ElapsedEventArgs^ e)
	{
		this->onTick(this, System::Threading::Thread::CurrentThread);
	}

	void Script::handleEvent(ScriptEvent^ Event)
	{
		switch (Event->TypeOfEvent)
		{
		case EventType::KeywordEvent:
			this->KeyWord(this, (KeywordEventArgs^)Event->Args);

			break;

		case EventType::KeyUpEvent:
			this->KeyUp(this, (SerializableKeyEventArgs^)Event->Args);

			break;

		case EventType::KeyDownEvent:
			this->KeyDown(this, (SerializableKeyEventArgs^)Event->Args);
			
			break;

		case EventType::ConsoleCmdEvent:

			break;
		}
	}
}
