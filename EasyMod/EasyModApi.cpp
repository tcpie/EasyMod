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
#include "EasyModApi.h"

#include "KeywordEngine.h"
#include "IO.h"
#include "GraphicsApi.h"
#include "EnvironmentApi.h"
#include "Console.h"

namespace EasyMod
{
	EasyModApi::EasyModApi(KeywordEngine^ KeywordWatcher, GraphicsApi^ Graphics, ConsoleApi^ ConsoleApi)
	{		
		this->keyword_watcher = KeywordWatcher;
		this->graphics_api = Graphics;
		this->console_api = ConsoleApi;

		this->io_api = gcnew IOApi(KeywordWatcher);
		this->environment_api = gcnew EnvironmentApi();
	}

	void EasyModApi::unloadAll()
	{
		this->graphics_api->clearDrawables();
		this->console_api->UnloadAll();
	}
}
