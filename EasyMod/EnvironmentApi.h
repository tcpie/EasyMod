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

#ifndef _ENVIRONMENTAPI_H_
#define _ENVIRONMENTAPI_H_

namespace EasyMod
{
	ref class WindowApi;

	public ref class EnvironmentApi : System::MarshalByRefObject
	{
	private:
		System::Diagnostics::Process^ current_process;

		System::Collections::Generic::List<WindowApi^>^ windows;
		WindowApi^ main_window;
		WindowApi^ console_window;

	internal:
		EnvironmentApi();

	public:
		property System::Drawing::Size^ MainMonitorSize { System::Drawing::Size^ get() { return System::Windows::Forms::SystemInformation::PrimaryMonitorSize; } }

		property System::Diagnostics::Process^ CurrentProcess { System::Diagnostics::Process^ get() { return this->current_process; } }

		property WindowApi^ MainWindow { WindowApi^ get() { return this->main_window; } }

		property WindowApi^ ConsoleWindow { WindowApi^ get() { return this->console_window; } }

		property System::Collections::Generic::List<WindowApi^>^ Windows { System::Collections::Generic::List<WindowApi^>^ get() { return this->windows; } }
	};

}


#endif
