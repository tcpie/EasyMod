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

#ifndef _CLRASSEMBLYHELPER_H_
#define _CLRASSEMBLYHELPER_H_

namespace EasyMod {

public ref class ClrAssemblyHelper
{
private:
	System::String^ assemblyPath;
	static ClrAssemblyHelper^ instance;
	static System::Reflection::Assembly^ LoadAssemblyInternal(System::String^ AssemblyPath);
	
public:
	ClrAssemblyHelper();

	static void SetAssemblyPath(System::String^ AssemblyPath);
	static System::String^ GetAssemblyPath();

	static System::Reflection::Assembly^ CurrentDomain_AssemblyResolve(System::Object^ sender, System::ResolveEventArgs^ args);
	static System::Reflection::Assembly^ LoadAssembly(System::String^ AssemblyPath);
	static bool InitializeAssembly(System::Reflection::Assembly^ Assembly);
	
	delegate bool InitializeAssemblyDelegate(System::Reflection::Assembly^ Assembly);
	delegate System::Reflection::Assembly^ LoadAssemblyDelegate(System::String^ AssemblyPath);
	delegate void SetAssemblyPathDelegate(System::String^ AssemblyPath);
};

}

#endif
