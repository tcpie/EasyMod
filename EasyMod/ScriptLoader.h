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

#ifndef _SCRIPTLOADER_H_
#define _SCRIPTLOADER_H_

#include "Script.h"

namespace EasyMod
{
	ref class EasyModApi;

	ref class ScriptLoader
	{
	private:
		System::String^ scripts_directory;
		System::String^ scripts_appdomain_name;
		EasyModApi^ client_api;

	internal:
		ScriptLoader(System::String^ ScriptsDirectory, System::String^ ScriptsAppDomainName, EasyModApi^ ClientApi);

		property System::String^ ScriptsDirectory
		{
			System::String^ get() { return this->scripts_directory; }
			void set(System::String^ ScriptsDirectory) { this->scripts_directory = ScriptsDirectory; }
		}

		System::Collections::Generic::List<Script^>^ ReloadScripts(System::AppDomain^ ScriptsAppDomain);
		void UnloadAllScripts(System::AppDomain^ ScriptsAppDomain);
		System::Collections::Generic::List<Script^>^ LoadScripts();
		System::Collections::Generic::List<System::IO::FileInfo^>^ FindScriptAssembliesInDirectory(System::String^ ScriptsDirectory);
		System::Collections::Generic::List<Script^>^ LoadScriptsFromAssembly(System::String^ ScriptAssemblyLocation);
		System::Collections::Generic::List<Script^>^ ConstructScriptsInAssembly(System::Reflection::Assembly^ Assembly, System::String^ ScriptAssemblyLocation);
		System::AppDomain^ GetScriptsAppDomain();
	};
}

#endif
