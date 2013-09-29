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
#include "AppDomainHelper.h"
#include "ClrAssemblyHelper.h"

namespace EasyMod {

[System::Serializable]
ref class AppDomainHelperBuddy : System::MarshalByRefObject
{
public:
	AppDomainHelperBuddy() 
	{ 
		System::AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(ClrAssemblyHelper::CurrentDomain_AssemblyResolve);
	}

	static AppDomainHelperBuddy^ Get(System::AppDomain^ Domain)
	{
		System::AppDomain::CurrentDomain->AssemblyResolve += gcnew System::ResolveEventHandler(ClrAssemblyHelper::CurrentDomain_AssemblyResolve);

		return  (AppDomainHelperBuddy^)Domain->CreateInstanceFrom(System::Reflection::Assembly::GetExecutingAssembly()->Location, AppDomainHelperBuddy::typeid->FullName)->Unwrap();
	}

	System::Object^ ExecuteMethod(System::Delegate^ Method, array<System::Object^>^ Parameters)
	{
		array<System::Reflection::ParameterInfo^>^ parameter_info = Method->Method->GetParameters();
		
		if (parameter_info->Length != 0 && Parameters == nullptr || parameter_info->Length != Parameters->Length)
		{
			throw gcnew System::Exception("Parameter count does not match parameter count of method! " + "(expected " + parameter_info->Length.ToString() + ", got " + Parameters->Length.ToString() + ")");
			
			return nullptr;
		}

		return Method->DynamicInvoke(Parameters);
	}
};

AppDomainHelper::AppDomainHelper()
{

}

System::AppDomain^ AppDomainHelper::GetDefaultDomain()
{
	mscoree::ICorRuntimeHost^ host = gcnew mscoree::CorRuntimeHost();
	System::Object^ domain;
	host->GetDefaultDomain(domain);

	System::Runtime::InteropServices::Marshal::ReleaseComObject(host);
	return (System::AppDomain^)domain;
}

System::Collections::Generic::List<System::AppDomain^>^ AppDomainHelper::GetAllAppDomains()
{
	System::IntPtr enumHandle = System::IntPtr::Zero;
	mscoree::ICorRuntimeHost^ host = gcnew mscoree::CorRuntimeHost();
	System::Collections::Generic::List<System::AppDomain^>^ domains = gcnew System::Collections::Generic::List<System::AppDomain^>();
	
	try
	{
		host->EnumDomains(enumHandle);

		System::Object^ domain;

		while (true)
        {
            host->NextDomain(enumHandle, domain);

            if (domain == nullptr) 
			{
				break;
			}

			domains->Add((System::AppDomain^)domain);
        } 
    }
    catch (...)
	{ 
		return domains; 
	}
    finally
    {
        host->CloseEnum(enumHandle);
		System::Runtime::InteropServices::Marshal::ReleaseComObject(host);
    }

	return domains;
}

System::Object^ AppDomainHelper::ExecuteMethodInAppDomain(System::AppDomain^ Domain, System::Delegate^ Method, array<System::Object^>^ Parameters)
{
	AppDomainHelperBuddy^ buddy = AppDomainHelperBuddy::Get(Domain);	
	
	return buddy->ExecuteMethod(Method, Parameters);
}

}
