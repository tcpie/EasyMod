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
#include "PluginManager.h"
#include "ClrAssemblyHelper.h"
#include "AppDomainHelper.h"

namespace EasyMod {

PluginManager::PluginManager()
{
	this->plugins = gcnew System::Collections::Generic::List<Plugin^>();

	this->resetPluginAppDomain();
}

void PluginManager::resetPluginAppDomain()
{
	if (this->pluginAppDomain == nullptr)
	{
		this->pluginAppDomain = System::AppDomain::CreateDomain("tcpie_pluginmanager");
		this->pluginAppDomain->InitializeLifetimeService();

		return;
	}

	System::AppDomain::Unload(this->pluginAppDomain);

	this->pluginAppDomain = System::AppDomain::CreateDomain("tcpie_pluginmanager");
	this->pluginAppDomain->InitializeLifetimeService();

	return;
}

PluginManager^ PluginManager::Get()
{
	if (PluginManager::instance == nullptr)
	{
		PluginManager::instance = gcnew PluginManager();
	}

	return PluginManager::instance;
}

array<Plugin^>^ PluginManager::GetPlugins()
{
	return this->plugins->ToArray();
}

void PluginManager::UnloadAllPlugins()
{
	for (int i = 0; i < this->plugins->Count; i++)
	{
		if (this->plugins[i]->GetAppDomain() != this->pluginAppDomain)
		{
			System::AppDomain::Unload(this->plugins[i]->GetAppDomain());
		}
	}

	this->plugins->Clear();

	this->resetPluginAppDomain();
}

bool PluginManager::UnloadPlugin(System::String^ PluginFileName)
{
	// Let's find the plugin
	Plugin^ plugin = nullptr;

	for (int i = 0; i < this->plugins->Count; i++)
	{
		if (this->plugins[i]->GetFileName() == System::IO::Path::GetFileNameWithoutExtension(PluginFileName))
		{
			plugin = this->plugins[i];

			break;
		}
	}

	if (plugin == nullptr)
	{
		return false;
	}

	if (this->pluginAppDomain == plugin->GetAppDomain())
	{
		this->plugins->Clear();
		this->resetPluginAppDomain();
	}
	else
	{
		this->plugins->Remove(plugin);
		System::AppDomain::Unload(plugin->GetAppDomain());
	}

	

	return true;
}

System::Reflection::Assembly^ PluginManager::LoadPlugin(System::String^ PluginLocation, System::String^ AppDomainName, bool InitializeAssembly)
{
	System::AppDomain^ domain = nullptr;

	System::Collections::Generic::List<System::AppDomain^>^ all_domains = AppDomainHelper::GetAllAppDomains();

	for (int i = 0; i < all_domains->Count; i++)
	{
		if (all_domains[i]->FriendlyName == AppDomainName)
		{
			domain = all_domains[i];

			break;
		}
	}

	if (domain == nullptr)
	{
		domain = System::AppDomain::CreateDomain(AppDomainName);
		domain->InitializeLifetimeService();
	}

	array<System::Object^>^ parameters = gcnew array<System::Object^>(1);
	parameters[0] = (System::Object^)PluginLocation;

	System::Reflection::Assembly^ assembly = nullptr;
	
	try
	{
		assembly = (System::Reflection::Assembly^)AppDomainHelper::ExecuteMethodInAppDomain(domain, gcnew ClrAssemblyHelper::LoadAssemblyDelegate(ClrAssemblyHelper::LoadAssembly), parameters);
	}
	catch (...)
	{
		// We probably can't load using our custom method, so let's use the default method:
		assembly =  (System::Reflection::Assembly^)AppDomainHelper::ExecuteMethodInAppDomain(domain, gcnew ClrAssemblyHelper::LoadAssemblyDelegate(System::Reflection::Assembly::Load), parameters);
	}

	if (assembly == nullptr)
	{
		if (AppDomainName != this->pluginAppDomain->FriendlyName)
		{
			System::AppDomain::Unload(domain);
		}

		return nullptr;
	}

	if (InitializeAssembly)
	{
		if ((System::Boolean)AppDomainHelper::ExecuteMethodInAppDomain(domain, gcnew ClrAssemblyHelper::InitializeAssemblyDelegate(ClrAssemblyHelper::InitializeAssembly), gcnew array<System::Object^> { (System::Object^)assembly }) == false)
		{
			if (AppDomainName != this->pluginAppDomain->FriendlyName)
			{
				System::AppDomain::Unload(domain);
			}

			return nullptr;
		}
	}

	Plugin^ plugin = gcnew Plugin(assembly, domain);

	this->plugins->Add(plugin);

	return assembly;
}

System::Reflection::Assembly^ PluginManager::LoadPlugin(System::String^ PluginLocation, bool LoadInSeperateAppDomain, bool InitializeAssembly)
{
	System::AppDomain^ domain = this->pluginAppDomain;

	System::String^ domain_name;

	if (LoadInSeperateAppDomain)
	{		
		domain_name = this->plugins->Count.ToString() + "_" + System::IO::Path::GetFileNameWithoutExtension(PluginLocation);
	}
	else 
	{
		if (domain == nullptr)
		{
			this->resetPluginAppDomain();
		}

		domain_name = this->pluginAppDomain->FriendlyName;
	}

	return this->LoadPlugin(PluginLocation, domain_name, InitializeAssembly);
}

} 
