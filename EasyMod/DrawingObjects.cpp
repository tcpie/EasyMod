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
#include "DrawingObjects.h"
#include "D3D9Handler.h"

namespace EasyMod
{
	DrawableLine::DrawableLine(Vector2 StartPos, Vector2 EndPos, float Width, System::Drawing::Color Colour, VisibilityInfo^ VisibilityInfo)
	{
		this->start_pos = StartPos;
		this->end_pos = EndPos;
		this->width = Width;
		this->colour = Colour;
		this->visibility_info = VisibilityInfo;
	}

	DrawableLine::DrawableLine(Vector2 StartPos, Vector2 EndPos, float Width, System::Drawing::Color Colour)
	{
		this->start_pos = StartPos;
		this->end_pos = EndPos;
		this->width = Width;
		this->colour = Colour;
		this->visibility_info = gcnew VisibilityInfo(100, true);
	}

	DrawableRectangle::DrawableRectangle(Vector2 UpperLeftCornerPos, System::UInt32 Width, System::UInt32 Height, System::Drawing::Color Colour, VisibilityInfo^ VisibilityInfo)
	{
		this->upperLeftCornerPos = UpperLeftCornerPos;
		this->width = Width;
		this->height = Height;
		this->colour = Colour;

		this->visibility_info = VisibilityInfo;
	}

	DrawableRectangle::DrawableRectangle(Vector2 UpperLeftCornerPos, System::UInt32 Width, System::UInt32 Height, System::Drawing::Color Colour)
	{
		this->upperLeftCornerPos = UpperLeftCornerPos;
		this->width = Width;
		this->height = Height;
		this->colour = Colour;

		this->visibility_info = gcnew VisibilityInfo(100, true);
	}
}
