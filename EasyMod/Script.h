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

#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include "Events.h"
#include "KeyWatchDog.h"

#pragma managed

namespace EasyMod
{
	ref class Controller;
	ref class EasyModApi;
	ref class ConsoleCommand;

	public ref class Script abstract : System::MarshalByRefObject 
	{
	private:
		Controller^ my_owner;
		System::Timers::Timer^ timer;

		void timerElapsed(Object^ source, System::Timers::ElapsedEventArgs^ e);
		void onTick(Object^ sender, System::Threading::Thread^ CurrentThread);

	protected:
		// --- Protected variables
		System::String^ description;
		System::String^ name;
		System::Guid guid;

		EasyModApi^ api;

		// --- Events
		event TickEventHandler^ tick;
		event KeyEventHandler^ KeyDown;
		event KeyEventHandler^ KeyUp;
		event KeywordEventHandler^ KeyWord;

		// --- Methods to be used by children

		// --- Properties
		property System::Double tickInterval
		{
			System::Double get() { return this->timer->Interval; }

			void set(System::Double tickInterval) {	this->timer->Interval = tickInterval; }
		}

		property System::ComponentModel::ISynchronizeInvoke^ timerSynchronizationObject
		{
			System::ComponentModel::ISynchronizeInvoke^ get() { return this->timer->SynchronizingObject; }

			void set(System::ComponentModel::ISynchronizeInvoke^ timerSynchronizationObject) { this->timer->SynchronizingObject = timerSynchronizationObject; }
		}

		property Controller^ owner
		{
			Controller^ get() { return this->my_owner; }
		}

	internal:
		void handleEvent(ScriptEvent^ Event);

	public:
		Script(EasyModApi^ ApiInterface);

		property System::String^ Description
		{
			System::String^ get() { return this->description; }
		}

		property System::Guid GUID
		{
			System::Guid get() { return this->guid; }
		}

		property System::String^ Name
		{
			System::String^ get() { return this->name; }
		}
	};
}

#endif // _SCRIPT_H_
