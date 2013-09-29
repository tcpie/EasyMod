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
#include "ScriptLoader.h"
#include "EasyModApi.h"
#include "ClrAssemblyHelper.h"
#include "PluginManager.h"
#include "AppDomainHelper.h"

namespace EasyMod
{
	ScriptLoader::ScriptLoader(System::String^ ScriptsDirectory, System::String^ ScriptsAppDomainName, EasyModApi^ ClientApi)
	{
		this->scripts_directory = ScriptsDirectory;
		this->scripts_appdomain_name = ScriptsAppDomainName;
		this->client_api = ClientApi;
	}

	System::Collections::Generic::List<Script^>^ ScriptLoader::ReloadScripts(System::AppDomain^ ScriptsAppDomain)
	{
		System::Console::WriteLine("Reloading scripts...");

		this->UnloadAllScripts(ScriptsAppDomain);
		System::Collections::Generic::List<Script^>^ ret = this->LoadScripts();

		System::Console::WriteLine("Done.");

		return ret;
	}

	void ScriptLoader::UnloadAllScripts(System::AppDomain^ ScriptsAppDomain)
	{
		if (ScriptsAppDomain != nullptr)
		{
			System::AppDomain::Unload(ScriptsAppDomain);
		}
	}

	System::Collections::Generic::List<Script^>^ ScriptLoader::LoadScripts()
	{
		System::Collections::Generic::List<System::IO::FileInfo^>^ file_info = this->FindScriptAssembliesInDirectory(this->scripts_directory);
		
		if (file_info == nullptr)
		{
			return nullptr;
		}

		System::Collections::Generic::List<Script^>^ loaded_scripts = gcnew System::Collections::Generic::List<Script^>();

		for (int i = 0; i < file_info->Count; i++)
		{
			System::Collections::Generic::List<Script^>^ temp_list = this->LoadScriptsFromAssembly(file_info[i]->FullName);

			if (temp_list != nullptr)
			{
				loaded_scripts->AddRange(temp_list);
			}
		}

		return loaded_scripts;
	}

	System::Collections::Generic::List<System::IO::FileInfo^>^ ScriptLoader::FindScriptAssembliesInDirectory(System::String^ ScriptsDirectory)
	{
		System::Collections::Generic::IEnumerable<System::IO::FileInfo^>^ file_info = nullptr;

		try
		{
			System::IO::DirectoryInfo^ di = gcnew System::IO::DirectoryInfo(ScriptsDirectory);
			file_info = di->EnumerateFiles("*.dll");
		}
		catch (...)
		{
			return nullptr;
		}

		System::Collections::Generic::List<System::IO::FileInfo^>^  ret = gcnew System::Collections::Generic::List<System::IO::FileInfo^>();
		ret->AddRange(file_info);

		return ret;
	}

	System::Collections::Generic::List<Script^>^ ScriptLoader::LoadScriptsFromAssembly(System::String^ ScriptAssemblyLocation)
	{
		System::AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(ClrAssemblyHelper::CurrentDomain_AssemblyResolve);

		System::Console::WriteLine("Getting plugin manager...");
		PluginManager^ manager = PluginManager::Get();
		System::Console::WriteLine("Got manager");

		System::Reflection::Assembly^ script_assembly = nullptr;
		
		try 
		{
			// System::Console::WriteLine("Loading EasyMod assembly...");
			// System::Reflection::Assembly^ this_assembly = manager->LoadPlugin(System::Reflection::Assembly::GetExecutingAssembly()->Location, this->scripts_appdomain_name, false);
			// System::Console::WriteLine("Done.");
			
			System::Console::WriteLine("Getting scripts assembly...");
			script_assembly = manager->LoadPlugin(ScriptAssemblyLocation, this->scripts_appdomain_name, false);
			System::Console::WriteLine("Done.");
		}
		catch (System::Exception^ ex)
		{
			System::Console::Write(ex->ToString());
		}
		
		if (script_assembly == nullptr)
		{
			return nullptr;
		}

		return this->ConstructScriptsInAssembly(script_assembly, ScriptAssemblyLocation);
	}

	System::Collections::Generic::List<Script^>^ ScriptLoader::ConstructScriptsInAssembly(System::Reflection::Assembly^ Assembly, System::String^ ScriptAssemblyLocation)
	{
		System::AppDomain^ scripts_appdomain = this->GetScriptsAppDomain();

		if (scripts_appdomain == nullptr)
		{
			scripts_appdomain = System::AppDomain::CreateDomain(this->scripts_appdomain_name);

			if (scripts_appdomain == nullptr)
			{
				return nullptr;
			}
		}

		System::Collections::Generic::List<Script^>^ ret = gcnew System::Collections::Generic::List<Script^>();
		array<System::Type^>^ types = nullptr;
		
		try
		{
			types = Assembly->GetTypes();
		}
		catch (System::Reflection::ReflectionTypeLoadException^ ex)
		{
			System::Console::Write(ex->ToString());
			System::Console::WriteLine("Loader exceptions: ");

			for (int i = 0; i < ex->LoaderExceptions->Length; i++)
			{
				System::Console::Write(ex->LoaderExceptions[i]->ToString());
			}			
		}

		if (types == nullptr)
		{
			return nullptr;
		}

		array<System::Type^>^ constructor_types =  gcnew array<System::Type^>{EasyModApi::typeid};
		array<System::Object^>^ args = gcnew array<System::Object^>{this->client_api};

		System::Type^ script_type = Script::typeid;

		System::Console::WriteLine(types->Length + " types found in assembly " + Assembly->FullName);

		for (int i = 0; i < types->Length; i++)
		{
			array<System::Reflection::ConstructorInfo^>^ constructor_infos = types[i]->GetConstructors();
			bool valid_constructor = true;

			for (int j = 0; j < constructor_infos->Length; j++)
			{
				array<System::Reflection::ParameterInfo^>^ param_info = constructor_infos[j]->GetParameters();

				for (int k = 0; k < param_info->Length; k++)
				{
					if (k >= constructor_types->Length || param_info[k]->ParameterType->FullName != constructor_types[k]->FullName)
					{
						valid_constructor = false;

						break;
					}
					//System::Console::Write(param_info[k]->ParameterType->ToString());
				}

				if (valid_constructor)
				{
					break;
				}

				valid_constructor = true;
				//System::Console::Write("\n");
			}

			bool full_names_match = types[i]->BaseType->FullName == script_type->FullName;
						
			if (types[i]->BaseType->FullName == script_type->FullName &&						// Must have inherited from Script
				valid_constructor) // types[i]->GetConstructor(System::Type::EmptyTypes) != nullptr)					// Must have parameterless constructor
			{
				System::Console::Write("Creating instance... \nassembly full name: " + ScriptAssemblyLocation + "\nFull Name: " + types[i]->FullName + "\n");

				Script^ temp_ret;

				System::Object^ ret_obj;

				try
				{
					//temp_ret = (Script^)scripts_appdomain->CreateInstanceFrom(ScriptAssemblyLocation, types[i]->FullName)->Unwrap();
					// ret_obj = scripts_appdomain->CreateInstanceFrom(ScriptAssemblyLocation, types[i]->FullName)->Unwrap();

					ret_obj = scripts_appdomain->CreateInstanceFrom(ScriptAssemblyLocation, 
																	types[i]->FullName,
																	false,
																	System::Reflection::BindingFlags::CreateInstance,
																	nullptr,
																	args,
																	System::Globalization::CultureInfo::CurrentCulture,
																	nullptr)->Unwrap();
					temp_ret = (Script^)ret_obj;
				}
				catch (System::Exception^ ex)
				{					
					System::String^ script_assembly_codebase = script_type->Assembly->CodeBase;
					System::Console::WriteLine("Real code base: " + script_assembly_codebase);

					// System::String^ dynamic_codebase = ret_obj->GetType()->Assembly->CodeBase;
					// System::Console::WriteLine("Dynamic code base: " + dynamic_codebase);

					System::Console::WriteLine(ex->ToString());
					return nullptr;
				}

				if (temp_ret != nullptr)
				{				

					System::Console::WriteLine("Success!");
					ret->Add(temp_ret);
				}
			}
		}

		return ret;
	}

	System::AppDomain^ ScriptLoader::GetScriptsAppDomain()
	{
		System::Collections::Generic::List<System::AppDomain^>^ all_domains = AppDomainHelper::GetAllAppDomains();

		for (int i = 0; i < all_domains->Count; i++)
		{
			if (all_domains[i]->FriendlyName == this->scripts_appdomain_name)
			{
				return all_domains[i];
			}
		}

		return nullptr;
	}
}
