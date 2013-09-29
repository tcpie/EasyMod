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

#ifndef _D3D9HANDLER_H_
#define _D3D9HANDLER_H_

#include <WinCore\D3D9Hook.h>
#include "D3D9Drawer.h"

#include <d3d9.h>
#include <vcclr.h>

namespace EasyMod
{
	ref class InternalD3D9EventArgs;
	ref class D3D9Handler;

	public delegate void InternalD3D9EventHandler(System::Object^ sender, InternalD3D9EventArgs^ e);
	
	ref class InternalD3D9EventArgs : System::EventArgs
	{
	private:

	internal:
		InternalD3D9EventArgs()
		{
		}
	};

	class MyD3D9Hook : public tcpie::wincore::ID3D9CallbackClass
	{
	protected:
		gcroot<D3D9Handler^> owner;
		
	public:
		MyD3D9Hook(D3D9Handler^ Owner);

		virtual void OnDeviceReset(IDirect3DDevice9 *pDevice, D3DPRESENT_PARAMETERS *pD3DPRESENT_PARAMETERS) { }
		virtual void OnDeviceLost(IDirect3DDevice9 *pDevice) { }
		virtual void OnDeviceReleased(IDirect3DDevice9 *pDevice) { } // We don't need this
		virtual void OnEndScene(IDirect3DDevice9 *pDevice);
	};

	ref class D3D9Handler
	{
	private:
		bool hooks_set;
		IDirect3DDevice9* D3D9Device;
		D3D9Drawer^ drawer;
		InternalD3D9EventArgs^ endSceneArgs;

		MyD3D9Hook* my_hook;

		void rebuildEndSceneArgs(IDirect3DDevice9* Device);

	internal:

		void OnEndScene(IDirect3DDevice9* Device);

		D3D9Handler();

		event InternalD3D9EventHandler^ EndScene;

		bool SetHooks();

		property D3D9Drawer^ Drawer
		{
			D3D9Drawer^ get()
			{
				return this->drawer;
			}
		}
	};
}

#endif
