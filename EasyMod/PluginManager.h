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

#ifndef _PLUGINMANAGER_H_
#define _PLUGINMANAGER_H_

#include "Plugin.h"

namespace EasyMod {

public ref class PluginManager
{
private:
	PluginManager();
	static PluginManager^ instance;

	System::Collections::Generic::List<Plugin^>^ plugins;
	System::AppDomain^ pluginAppDomain;
	void resetPluginAppDomain();

public:
	static PluginManager^ Get();
	System::Reflection::Assembly^ LoadPlugin(System::String^ PluginLocation, bool LoadInSeperateAppDomain, bool InitializeAssembly);
	System::Reflection::Assembly^ LoadPlugin(System::String^ PluginLocation, System::String^ AppDomainName, bool InitializeAssembly);
	bool UnloadPlugin(System::String^ PluginFileName);
	void UnloadAllPlugins();

	array<Plugin^>^ GetPlugins();

	delegate PluginManager^ GetPluginManagerDelegate();
};

}


#endif
