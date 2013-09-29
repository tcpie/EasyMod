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
#include "ClrAssemblyHelper.h"
#include "AppDomainHelper.h"

namespace EasyMod {

ClrAssemblyHelper::ClrAssemblyHelper()
{
	this->assemblyPath = System::String::Empty;
	ClrAssemblyHelper::instance = this;
	System::AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(ClrAssemblyHelper::CurrentDomain_AssemblyResolve);
}

void ClrAssemblyHelper::SetAssemblyPath(System::String^ AssemblyPath)
{
	if (ClrAssemblyHelper::instance == nullptr)
	{
		gcnew ClrAssemblyHelper();
	}

	ClrAssemblyHelper::instance->assemblyPath = AssemblyPath;
}

System::String^ ClrAssemblyHelper::GetAssemblyPath()
{
	if (ClrAssemblyHelper::instance == nullptr)
	{
		return nullptr;
	}

	return ClrAssemblyHelper::instance->assemblyPath;
}

System::Reflection::Assembly^ ClrAssemblyHelper::CurrentDomain_AssemblyResolve(System::Object^ sender, System::ResolveEventArgs^ args)
{
	ClrAssemblyHelper^ instance = ClrAssemblyHelper::instance;
		
	if (instance == nullptr)
	{
		instance = gcnew ClrAssemblyHelper();
	}

	array<System::String^>^ Parts = args->Name->Split(',');
	System::String^ assembly_name =	Parts[0]->Trim();

	array<System::Reflection::Assembly^>^ all_assemblies_in_current_domain = System::AppDomain::CurrentDomain->GetAssemblies();

	// We check if the given assembly is already loaded. If so, we return the already-loaded assembly
	for (int i = 0; i < all_assemblies_in_current_domain->Length; i++)
	{
		System::String^ comp_string = all_assemblies_in_current_domain[i]->FullName->Split(',')[0]->Trim();		

		if (comp_string == assembly_name)
		{
			//System::Windows::Forms::MessageBox::Show("Requested assembly is already loaded. Returning loaded assembly :-)");
			return all_assemblies_in_current_domain[i];
		}
	}
	
	System::String^ File                 = "";

	// We now check various locations, to see if the file exists there

	if (System::IO::File::Exists(assembly_name))
	{
		File = assembly_name;
	}
	else if (System::IO::File::Exists(assembly_name->Replace("\\\\", "\\")))
	{
		File = assembly_name->Replace("\\\\", "\\");
	}
	else if (System::IO::File::Exists("." + "\\" + assembly_name + ".dll"))
	{
		File = "." + "\\" + assembly_name + ".dll";
	}
	else if (System::IO::File::Exists("." + "\\" + assembly_name + ".net.dll"))
	{
		File = "." + "\\" + assembly_name + ".net.dll";
	}
	else if (System::IO::File::Exists("." + "\\scripts\\" + assembly_name + ".dll"))
	{
		File = "." + "\\scripts\\" + assembly_name + ".dll";
	}
	else if (System::IO::File::Exists("." + "\\scripts\\" + assembly_name + ".net.dll"))
	{
		File = "." + "\\scripts\\" + assembly_name + ".net.dll";
	}
	else if (System::IO::File::Exists("." + "\\plugins\\" + assembly_name + ".dll"))
	{
		File = "." + "\\plugins\\" + assembly_name + ".dll";
	}
	else if (System::IO::File::Exists("." + "\\plugins\\" + assembly_name + ".net.dll"))
	{
		File = "." + "\\plugins\\" + assembly_name + ".net.dll";
	}
	else if (System::IO::File::Exists(System::IO::Path::GetDirectoryName(System::Reflection::Assembly::GetExecutingAssembly()->Location) + "\\" + assembly_name + ".dll"))
	{
		File = System::IO::Path::GetDirectoryName(System::Reflection::Assembly::GetExecutingAssembly()->Location) + "\\" + assembly_name + ".dll";
	}
	else if (System::IO::File::Exists(System::IO::Path::GetDirectoryName(System::Reflection::Assembly::GetExecutingAssembly()->Location) + "\\" + assembly_name + ".net.dll"))
	{
		File = System::IO::Path::GetDirectoryName(System::Reflection::Assembly::GetExecutingAssembly()->Location) + "\\" + assembly_name + ".net.dll";
	}

	//System::Windows::Forms::MessageBox::Show("Seeking file: " + File);

	System::Reflection::Assembly^ result = nullptr;

	try
	{
		// First try to load through the custom way
		result = ClrAssemblyHelper::LoadAssemblyInternal(File);
	}
	catch (...)
	{
		//System::Windows::Forms::MessageBox::Show("Custom assembly load failed with exception: " + ex->ToString());
		// Now try to load through the default way
		result = System::Reflection::Assembly::LoadFrom(File);
	}

	return result;
}

bool ClrAssemblyHelper::InitializeAssembly(System::Reflection::Assembly^ Assembly)
{
	array<System::Type^>^ types = Assembly->GetTypes();

	for (int i = 0; i < types->Length; i++)
	{
		// Check if there is a DllMain class, which is not static
		// static classes are both abstract and sealed :O. See http://dotneteers.net/blogs/divedeeper/archive/2008/08/04/QueryingStaticClasses.aspx
				
		if (types[i]->Name == "DllMain" &&										// Must be DllMain
			!(types[i]->IsAbstract && types[i]->IsSealed) &&					// Must not be static
			types[i]->GetConstructor(System::Type::EmptyTypes) != nullptr)		// Must have parameterless constructor
		{
			System::Reflection::MethodInfo^ method_info = types[i]->GetMethod("DllAttached");

			if (method_info != nullptr &&
				method_info->GetParameters()->Length == 0)
			{
				try
				{
					method_info->Invoke(nullptr, nullptr);
				}
				catch (...)
				{
					return false;
				}

				return true;
			}
		}
	}

	return false;
}

System::Reflection::Assembly^ ClrAssemblyHelper::LoadAssemblyInternal(System::String^ AssemblyPath)
{
	System::IO::FileStream^ file_stream = gcnew System::IO::FileStream(AssemblyPath, System::IO::FileMode::Open, System::IO::FileAccess::Read, System::IO::FileShare::Read);

	array<System::Byte>^ data = gcnew array<System::Byte>((int)file_stream->Length);
	file_stream->Read(data, 0, (int)file_stream->Length);
	file_stream->Close();

	System::Reflection::Assembly^ assembly = System::Reflection::Assembly::Load(data);

	return assembly;
}

System::Reflection::Assembly^ ClrAssemblyHelper::LoadAssembly(System::String^ AssemblyPath)
{
	ClrAssemblyHelper::SetAssemblyPath(AssemblyPath);

	array<System::Object^>^ parameters = gcnew array<System::Object^>(1);
	parameters[0] = (System::Object^)AssemblyPath;
	AppDomainHelper::ExecuteMethodInAppDomain(AppDomainHelper::GetDefaultDomain(), gcnew ClrAssemblyHelper::SetAssemblyPathDelegate(ClrAssemblyHelper::SetAssemblyPath), parameters);
	System::Reflection::Assembly^ assembly = ClrAssemblyHelper::LoadAssemblyInternal(AssemblyPath);

	return assembly;
}

}
