
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
#include "D3D9Handler.h"

#pragma comment(lib, "WinCore.lib")

namespace EasyMod
{
	MyD3D9Hook::MyD3D9Hook(D3D9Handler^ Owner)
	{
		this->owner = Owner;
	}

	void MyD3D9Hook::OnEndScene(IDirect3DDevice9 *pDevice)
	{
		this->owner->OnEndScene(pDevice);
	}

	D3D9Handler::D3D9Handler()
	{
		this->hooks_set = false;
		this->my_hook = new MyD3D9Hook(this);
	}

	void D3D9Handler::rebuildEndSceneArgs(IDirect3DDevice9* Device)
	{
		this->D3D9Device = Device;
		this->drawer = gcnew D3D9Drawer(this->D3D9Device);
		this->endSceneArgs = gcnew InternalD3D9EventArgs();
	}

	void D3D9Handler::OnEndScene(IDirect3DDevice9* Device)
	{
		if (Device != this->D3D9Device ||
			this->drawer == nullptr ||
			this->endSceneArgs == nullptr)
		{
			this->rebuildEndSceneArgs(Device);
		}

		// System::Console::WriteLine("OnEndScene()");
		
		// this->drawer->DrawRectangle(10, 10, 400, 400, this->drawer->D3DColorFromARGB(200, 255, 0, 0));
		this->EndScene(this, this->endSceneArgs);
	}

	bool D3D9Handler::SetHooks()
	{
		if (this->hooks_set)
		{
			return true;
		}

		bool ret = false;
		
		try
		{
			ret = tcpie::wincore::D3D9Hook::RegisterDetour(this->my_hook);
		}
		catch (...)
		{
			
		}

		if (!ret)
		{
			return false;
		}

		this->hooks_set = true;

		return true;
	}
}
