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

#ifndef _DRAWINGOBJECTS_H_
#define _DRAWINGOBJECTS_H_

#include "ApiObjects.h"

#include <d3dx9.h>

namespace EasyMod
{
	ref class D3D9Handler;
	
	[System::SerializableAttribute]
	public ref class VisibilityInfo : public System::IComparable
	{
	public:
		System::Boolean IsVisible;
		System::Byte DepthValue;

		VisibilityInfo(System::Byte DepthValue, System::Boolean IsVisible) { this->DepthValue = DepthValue; this->IsVisible = IsVisible; }

		virtual int CompareTo(System::Object^ obj)
		{
			VisibilityInfo^ d = (VisibilityInfo^)obj;

			if (this->DepthValue == d->DepthValue)
			{
				return 0;
			}
			else if (this->DepthValue < d->DepthValue)
			{
				return 1;
			}
			else if (this->DepthValue > d->DepthValue)
			{
				return -1;
			}

			return 0;
		}
	};

	private interface class IVisualizable
	{
		virtual property VisibilityInfo^ VisInfo;
	};

	public ref class DrawableFont : public System::MarshalByRefObject
	{
	private:
		System::String^ font_name;
		int size;
		bool italic;
		int width; 

	internal:
		// NOTE: we can't just set width to "internal", this sets us up for a complicated problem:
		//	while the code that will try to access width is "in" this assembly (GraphicsApi), at
		//  run-time it is in another AppDomain, thus it cannot access the internal variable! :O
		// DrawableFont(System::String^ FontName, int Size, bool Italic, int Width) { this->font_name = FontName; this->size = Size; this->italic = Italic; this->width = Width; }

		void setFontWidth(int Width)
		{
			this->width = Width;
		}

	public:
		DrawableFont(System::String^ FontName, int Size, bool Italic) { this->font_name = FontName; this->size = Size; this->italic = Italic; this->width = 0; }

		property System::String^ FontName { System::String^ get() { return this->font_name; } }

		property int Size { int get() { return this->size; } }

		property int LineHeight { int get() { return (int)((float)this->size * 1.1); } }

		property int CharWidth { int get() { return (int)((float)this->width * 1.1); } }

		property bool Italic { bool get() { return this->italic; } }
	};

	private ref class InternalDrawableFont
	{
	private:
		DrawableFont^ api_font;
		ID3DXFont* d3d_font;

	internal:
		InternalDrawableFont(DrawableFont^ ApiFont, ID3DXFont* D3DFont) { this->api_font = ApiFont; this->d3d_font = D3DFont; }

		property DrawableFont^ ApiFont { DrawableFont^ get() { return this->api_font; } }

		property ID3DXFont* D3DFont { ID3DXFont* get() { return this->d3d_font; } }
	};

	public ref class DrawableText : public System::MarshalByRefObject, public IVisualizable
	{
	private:
		System::String^ text;
		DrawableFont^ font;
		VisibilityInfo^ visibility_info;
		Vector2 position;
		System::Drawing::Color colour;

	public:
		DrawableText(System::String^ Text, DrawableFont^ Font, Vector2 Position, System::Drawing::Color Colour) 
		{ 
			this->text = Text; 
			this->font = Font; 
			this->visibility_info = gcnew VisibilityInfo(100, true);
			this->position = Position;
			this->colour = Colour;
		}

		virtual property VisibilityInfo^ VisInfo
		{
			VisibilityInfo^ get() { return this->visibility_info; }
			void set(VisibilityInfo^ value) { this->visibility_info = value; }
		}

		property bool IsVisible
		{
			bool get() { return this->VisInfo->IsVisible; }
			void set(bool value) { this->VisInfo->IsVisible = value; }
		}

		property Vector2 Position
		{
			Vector2 get() { return this->position; }
			void set(Vector2 value) { this->position = value; }
		}

		property System::String^ Text
		{
			System::String^ get() { return this->text; }
			void set(System::String^ value) { this->text = value; }
		}

		property DrawableFont^ Font
		{
			DrawableFont^ get() { return this->font; }
			void set(DrawableFont^ value) { this->font = value; }
		}
		
		property System::Drawing::Color Colour
		{
			System::Drawing::Color get() { return this->colour; }
			void set(System::Drawing::Color value) { this->colour = value; }
		}
	};	

	public ref class DrawableLine : public System::MarshalByRefObject, public IVisualizable
	{
	private:
		Vector2 start_pos;
		Vector2 end_pos;
		System::Drawing::Color colour;
		float width;
		VisibilityInfo^ visibility_info;

	public:
		DrawableLine(Vector2 StartPos, Vector2 EndPos, float Width, System::Drawing::Color Colour, VisibilityInfo^ VisibilityInfo);

		DrawableLine(Vector2 StartPos, Vector2 EndPos, float Width, System::Drawing::Color Colour);

		property Vector2 StartPos
		{
			Vector2 get() { return this->start_pos; }
			void set(Vector2 value) { this->start_pos = value; }
		}

		property Vector2 EndPos
		{
			Vector2 get() { return this->end_pos; }
			void set(Vector2 value) { this->end_pos = value; }
		}

		virtual property VisibilityInfo^ VisInfo
		{
			VisibilityInfo^ get() { return this->visibility_info; }
			void set(VisibilityInfo^ value) { this->visibility_info = value; }
		}

		property bool IsVisible
		{
			bool get() { return this->VisInfo->IsVisible; }
			void set(bool value) { this->VisInfo->IsVisible = value; }
		}

		property System::Drawing::Color Colour
		{
			System::Drawing::Color get() { return this->colour; }
			void set(System::Drawing::Color value) { this->colour = value; }
		}

		property float Width
		{
			float get() { return this->width; }
			void set(float value) { this->width = value; }
		}
	};

	public ref class DrawableRectangle : public System::MarshalByRefObject, public IVisualizable
	{
	private:
		Vector2 upperLeftCornerPos;		
		System::Drawing::Color colour;
		System::UInt32 width;
		System::UInt32 height;
		VisibilityInfo^ visibility_info;

	public:
		DrawableRectangle(Vector2 UpperLeftCornerPos, System::UInt32 Width, System::UInt32 Height, System::Drawing::Color Colour, VisibilityInfo^ VisibilityInfo);

		DrawableRectangle(Vector2 UpperLeftCornerPos, System::UInt32 Width, System::UInt32 Height, System::Drawing::Color Colour);

		virtual property VisibilityInfo^ VisInfo
		{
			VisibilityInfo^ get() { return this->visibility_info; }
			void set(VisibilityInfo^ value) { this->visibility_info = value; }
		}

		property bool IsVisible
		{
			bool get() { return this->VisInfo->IsVisible; }
			void set(bool value) { this->VisInfo->IsVisible = value; }
		}

		property System::Double X
		{
			System::Double get() { return this->upperLeftCornerPos.X; }
			void set(System::Double value) { this->upperLeftCornerPos.X = value; }
		}

		property System::Double Y
		{
			System::Double get() { return this->upperLeftCornerPos.Y; }
			void set(System::Double value) { this->upperLeftCornerPos.Y = value; }
		}

		property Vector2 UpperLeftCornerPos
		{
			Vector2 get() { return this->upperLeftCornerPos; }
			void set(Vector2 value) { this->upperLeftCornerPos = value; }
		}

		property System::Drawing::Color Colour
		{
			System::Drawing::Color get() { return this->colour; }
			void set(System::Drawing::Color value) { this->colour = value; }
		}

		property System::UInt32 Width
		{
			System::UInt32 get() { return this->width; }
			void set(System::UInt32 value) { this->width = value; }
		}

		property System::UInt32 Height
		{
			System::UInt32 get() { return this->height; }
			void set(System::UInt32 value) { this->height = value; }
		}
	};
}

#endif
