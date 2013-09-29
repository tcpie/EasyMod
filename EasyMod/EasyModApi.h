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

#ifndef _EASYMODAPI_H_
#define _EASYMODAPI_H_

namespace EasyMod 
{
	ref class KeywordEngine;
	ref class IOApi;
	ref class GraphicsApi;
	ref class EnvironmentApi;
	ref class ConsoleApi;

	public ref class EasyModApi : System::MarshalByRefObject
	{
	private:
		KeywordEngine^ keyword_watcher;
		
		IOApi^ io_api;
		GraphicsApi^ graphics_api;
		EnvironmentApi^ environment_api;
		ConsoleApi^ console_api;

	internal:
		EasyModApi(KeywordEngine^ KeywordWatcher, GraphicsApi^ Graphics, ConsoleApi^ ConsoleApi);

		void unloadAll();

	public:
		property IOApi^ IO
		{
			IOApi^ get() { return this->io_api; }
		}		

		property GraphicsApi^ Graphics
		{
			GraphicsApi^ get() { return this->graphics_api; }
		}

		property EnvironmentApi^ Env
		{
			EnvironmentApi^ get() { return this->environment_api; }
		}

		property ConsoleApi^ Console
		{
			ConsoleApi^ get() { return this->console_api; }
		}
	};
}

#endif
