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

#ifndef _D3D9DRAWER_H_
#define _D3D9DRAWER_H_

#include <d3d9.h>
#include <d3dx9.h>

namespace EasyMod
{
	ref class D3D9Drawer
	{
	private:
		IDirect3DDevice9* device;
		LPD3DXLINE line;

	internal:
		D3D9Drawer(IDirect3DDevice9* Device);

		void DrawLine(float StartX, float StartY, float EndX, float EndY, float Width, D3DCOLOR Colour, BOOL UseAntiAlias);
		void DrawLine(D3DXVECTOR2 StartPosition, D3DXVECTOR2 EndPosition, float Width, D3DCOLOR Colour, BOOL UseAntiAlias);
		void DrawRectangle(long UpperLeftX, long UpperLeftY, long Width, long Height, D3DCOLOR Colour);
		int DrawText(LPD3DXFONT D3DFont, System::String^ Text, long UpperLeftX, long UpperLeftY, D3DCOLOR FontColour);

		D3DCOLOR D3DColorFromDrawingColour(System::Drawing::Color Colour);
		D3DCOLOR D3DColorFromARGB(int Alpha, int R, int G, int B);
		LPD3DXFONT CreateD3DFont(System::String^ FontName, int Size, bool Italic, TEXTMETRICW* TextInfo);
	};
}

#endif
