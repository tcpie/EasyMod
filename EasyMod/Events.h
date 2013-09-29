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

#ifndef _EVENTS_H_
#define _EVENTS_H_

#include "Serializable.h"

#pragma managed 

namespace EasyMod
{	
	public delegate void GraphicsEventHandler(System::Object^ sender, SerializableGraphicsEventArgs^ e);

	public delegate void AnonymousKeywordEventHandler(KeywordEventArgs^ e);
	public delegate void KeywordEventHandler(System::Object^ sender, KeywordEventArgs^ e);

	public delegate void AnonymousKeyEventHandler(SerializableKeyEventArgs^ e);
	public delegate void KeyEventHandler(System::Object^ sender, SerializableKeyEventArgs^ e);

	public delegate void TickEventHandler(System::Object^ sender, System::Threading::Thread^ CurrentThread);

	public delegate void ConsoleCmdHandler(System::Object^ sender, SerializableConsoleCmdEventArgs^ e);
}

#endif // _EVENTS_H_
