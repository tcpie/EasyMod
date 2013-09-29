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
#include "D3D9Drawer.h"

#include <vcclr.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

namespace EasyMod
{
	D3D9Drawer::D3D9Drawer(IDirect3DDevice9* Device)
	{
		this->device = Device;
		
		pin_ptr<LPD3DXLINE> p = &this->line;

		D3DXCreateLine(this->device, p);
	}

	D3DCOLOR D3D9Drawer::D3DColorFromDrawingColour(System::Drawing::Color Colour)
	{
		return this->D3DColorFromARGB(Colour.A, Colour.R, Colour.G, Colour.B);
	}

	D3DCOLOR D3D9Drawer::D3DColorFromARGB(int Alpha, int R, int G, int B)
	{
		return D3DCOLOR_ARGB(Alpha, R, G, B);
	}

	void D3D9Drawer::DrawLine(float StartX, float StartY, float EndX, float EndY, float Width, D3DCOLOR Colour, BOOL UseAntiAlias)
	{
		D3DXVECTOR2 vLine[ 2 ]; // Two points
		this->line->SetAntialias(UseAntiAlias); // To smooth edges

		this->line->SetWidth(Width); // Width of the line
		this->line->Begin();

		vLine[ 0 ][ 0 ] = StartX; // Set points into array
		vLine[ 0 ][ 1 ] = StartY;
		vLine[ 1 ][ 0 ] = EndX;
		vLine[ 1 ][ 1 ] = EndY;

		this->line->Draw(vLine, 2, Colour); // Draw with Line, number of lines, and color
		this->line->End(); // finish
	}

	void D3D9Drawer::DrawLine(D3DXVECTOR2 StartPosition, D3DXVECTOR2 EndPosition, float Width, D3DCOLOR Colour, BOOL UseAntiAlias)
	{
		D3DXVECTOR2 vLine[ 2 ]; // Two points
		this->line->SetAntialias(UseAntiAlias); // To smooth edges

		this->line->SetWidth(Width); // Width of the line
		this->line->Begin();

		vLine[ 0 ][ 0 ] = StartPosition.x; // Set points into array
		vLine[ 0 ][ 1 ] = StartPosition.y;
		vLine[ 1 ][ 0 ] = EndPosition.x;
		vLine[ 1 ][ 1 ] = EndPosition.y;

		this->line->Draw(vLine, 2, Colour); // Draw with Line, number of lines, and color
		this->line->End(); // finish
	}

	void D3D9Drawer::DrawRectangle(long UpperLeftX, long UpperLeftY, long Width, long Height, D3DCOLOR Colour)
	{
		D3DXVECTOR2 vLine[ 2 ];
		this->line->SetWidth((FLOAT)Width);
		this->line->SetAntialias( 0 );
		this->line->SetGLLines( 1 );

		this->line->Begin();

		vLine[ 0 ][ 0 ] = (FLOAT)(UpperLeftX + Width/2);
		vLine[ 0 ][ 1 ] = (FLOAT)(UpperLeftY);
		vLine[ 1 ][ 0 ] = (FLOAT)(UpperLeftX + Width/2);
		vLine[ 1 ][ 1 ] = (FLOAT)(UpperLeftY + Height);

		this->line->Draw(vLine, 2, Colour);
		this->line->End();
	}

	int D3D9Drawer::DrawText(LPD3DXFONT D3DFont, System::String^ Text, long UpperLeftX, long UpperLeftY, D3DCOLOR FontColour)
	{
		RECT r;
		r.left = UpperLeftX;
		r.right = UpperLeftX + 3000;
		r.top = UpperLeftY;
		r.bottom = UpperLeftY + 2000;

		pin_ptr<const wchar_t> wch = PtrToStringChars(Text);

		return D3DFont->DrawTextW(NULL, wch, -1, &r, DT_LEFT | DT_TOP, FontColour);
	}

	LPD3DXFONT D3D9Drawer::CreateD3DFont(System::String^ FontName, int Size, bool Italic, TEXTMETRICW* TextInfo)
	{
		LPD3DXFONT ret_val = NULL;

		pin_ptr<const wchar_t> wch = PtrToStringChars(FontName);

		HRESULT ret = D3DXCreateFontW(this->device,					// D3D device
									 Size,							// Height
									 0,								// Width
									 FW_NORMAL,						// Weigth
									 1,								// Miplevels
									 Italic,						// Italic (y/n)
									 DEFAULT_CHARSET,				// Charset (ANSI, ....)
									 OUT_DEFAULT_PRECIS,			// Output precision
									 ANTIALIASED_QUALITY,			// Quality
									 DEFAULT_PITCH | FF_DONTCARE,	// Pitch and family
									 wch,							// Face name
									 &ret_val);						// Output: font

		if (ret_val != NULL && TextInfo != NULL)
		{
			ret_val->GetTextMetricsW(TextInfo);
		}

		return ret_val;
	}
}
