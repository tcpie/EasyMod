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

#ifndef _WINDOWAPI_H_
#define _WINDOWAPI_H_

namespace EasyMod
{
	public ref class WindowApi : System::MarshalByRefObject
	{
	private:
		System::IntPtr handle;
		bool is_console_window;

	internal:
		WindowApi(System::IntPtr Handle, bool IsConsoleWindow);

	public:
		System::Drawing::Rectangle GetDrawableArea();

		System::Drawing::Rectangle GetSizeAndPos();
		void SetSizeAndPos(System::Drawing::Rectangle value);

		System::String^ GetCaption();
		property bool IsConsoleWindow { bool get() { return this->is_console_window; } }
	};
}

#endif
