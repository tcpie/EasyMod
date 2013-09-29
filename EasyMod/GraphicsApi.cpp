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
#include "GraphicsApi.h"

namespace EasyMod
{
	GraphicsApi::GraphicsApi(D3D9Handler^ Handler)
	{
		this->handler = Handler;
		this->fonts = gcnew System::Collections::Generic::List<InternalDrawableFont^>();

		this->depth_drawables = gcnew System::Collections::Generic::List<System::Collections::Generic::List<IVisualizable^>^>();

		for (int i = 0; i < 256; i++) // 256 Depth levels
		{
			this->depth_drawables->Add(gcnew System::Collections::Generic::List<IVisualizable^>());
		}

		this->in_frame = false;
	}

	int GraphicsApi::VisualCompare(IVisualizable^ a, IVisualizable^ b)
	{
		return a->VisInfo->CompareTo(b->VisInfo);
	}

	int GraphicsApi::VisualCompare(DrawableRectangle^ a, DrawableRectangle^ b)
	{
		return a->VisInfo->CompareTo(b->VisInfo);
	}

	void GraphicsApi::clearDrawables()
	{
		for (int i = 0; i < this->depth_drawables->Count; i++)
		{
			this->depth_drawables[i]->Clear();
		}

		for (int i = 0; i < this->fonts->Count; i++)
		{
			this->fonts[i]->D3DFont->Release();
		}
		this->fonts->Clear();
	}

	void GraphicsApi::onEndScene()
	{
		this->in_frame = true;
		
		System::Collections::Generic::List<System::Collections::Generic::List<IVisualizable^>^>^ objs_to_draw = gcnew System::Collections::Generic::List<System::Collections::Generic::List<IVisualizable^>^>();

		int obj_count = 0;

		for (int i = 0; i < 256; i++)
		{
			objs_to_draw->Add(gcnew System::Collections::Generic::List<IVisualizable^>());

			for (int j = 0; j < this->depth_drawables[i]->Count; j++)
			{
				if (!this->depth_drawables[i][j]->VisInfo->IsVisible)
				{
					continue;
				}

				objs_to_draw[i]->Add(this->depth_drawables[i][j]);
				obj_count++;
			}
		}

		if (obj_count == 0)
		{
			this->in_frame = false;
			return;
		}
		else
		{
			// System::Console::WriteLine("Drawing " + objs_to_draw->Count + " objects");
		}

		for (int j = 255; j >= 0; j--)
		{
			for (int i = 0; i < objs_to_draw[j]->Count; i++)
			{
				if (objs_to_draw[j]->Count == 0)
				{
					continue;
				}

				if (DrawableRectangle::typeid->IsAssignableFrom(objs_to_draw[j][i]->GetType()))
				{
					this->DrawRectangle((DrawableRectangle^)objs_to_draw[j][i]);
				}
				else if (DrawableLine::typeid->IsAssignableFrom(objs_to_draw[j][i]->GetType()))
				{
					this->DrawLine((DrawableLine^)objs_to_draw[j][i]);
				}
				else if (DrawableText::typeid->IsAssignableFrom(objs_to_draw[j][i]->GetType()))
				{
					DrawableFont^ f = ((DrawableText^)objs_to_draw[j][i])->Font;

					InternalDrawableFont^ internal_font = this->getFont(f);

					if (internal_font == nullptr)
					{
						// System::Console::WriteLine("Could not find font!");

						continue;
					}

					// System::Console::WriteLine("Font found, continuing drawing...");
					this->DrawText((DrawableText^)objs_to_draw[j][i], internal_font);
				}
				else
				{
					System::Console::WriteLine("ERROR: could not determine drawable type! Type is: " + objs_to_draw->GetType()->ToString());
				}
			}
		}

		this->in_frame = false;
	}

	System::Boolean GraphicsApi::RegisterFont(DrawableFont^ Font)
	{
		TEXTMETRICW font_info;

		System::Console::WriteLine("Creating font: " + Font->FontName);

		LPD3DXFONT d3d_font = this->handler->Drawer->CreateD3DFont(Font->FontName, Font->Size, Font->Italic, &font_info);

		if (d3d_font == NULL)
		{
			return false;
		}
		
		Font->setFontWidth(font_info.tmMaxCharWidth);

		this->fonts->Add(gcnew InternalDrawableFont(Font, d3d_font));

		return true;
	}

	InternalDrawableFont^ GraphicsApi::getFont(DrawableFont^ Font)
	{
		for (int i = 0; i < this->fonts->Count; i++)
		{			
			if (this->fonts[i]->ApiFont == Font)
			{
				return this->fonts[i];
			}
		}

		return nullptr;
	}

	System::Boolean GraphicsApi::RemoveFont(DrawableFont^ Font)
	{
		InternalDrawableFont^ internal_font = this->getFont(Font);

		if (internal_font == nullptr)
		{
			return false;
		}

		return this->fonts->Remove(internal_font);
	}

	void GraphicsApi::RegisterDrawable(DrawableRectangle^ Rectangle)
	{
		this->depth_drawables[Rectangle->VisInfo->DepthValue]->Add(Rectangle);
	}

	void GraphicsApi::RegisterDrawable(DrawableLine^ Line)
	{
		this->depth_drawables[Line->VisInfo->DepthValue]->Add(Line);
	}

	void GraphicsApi::RegisterDrawable(DrawableText^ Text)
	{
		this->depth_drawables[Text->VisInfo->DepthValue]->Add(Text);
	}

	System::Boolean GraphicsApi::RemoveDrawable(DrawableRectangle^ Rectangle)
	{
		return this->depth_drawables[Rectangle->VisInfo->DepthValue]->Remove(Rectangle);
	}

	System::Boolean GraphicsApi::RemoveDrawable(DrawableLine^ Line)
	{
		return this->depth_drawables[Line->VisInfo->DepthValue]->Remove(Line);
	}

	System::Boolean GraphicsApi::RemoveDrawable(DrawableText^ Text)
	{
		return this->depth_drawables[Text->VisInfo->DepthValue]->Remove(Text);
	}

	void GraphicsApi::DrawLine(DrawableLine^ Line)
	{
		this->handler->Drawer->DrawLine((float)Line->StartPos.X, (float)Line->StartPos.Y, (float)Line->EndPos.X, (float)Line->EndPos.Y, Line->Width, this->handler->Drawer->D3DColorFromDrawingColour(Line->Colour), true);
	}

	void GraphicsApi::DrawRectangle(DrawableRectangle^ Rectangle)
	{		
		this->handler->Drawer->DrawRectangle((long)Rectangle->UpperLeftCornerPos.X, (long)Rectangle->UpperLeftCornerPos.Y, Rectangle->Width, Rectangle->Height, this->handler->Drawer->D3DColorFromDrawingColour(Rectangle->Colour));
	}

	int GraphicsApi::DrawText(DrawableText^ Text, InternalDrawableFont^ InternalFont)
	{
		return this->handler->Drawer->DrawText(InternalFont->D3DFont, Text->Text, (long)Text->Position.X, (long)Text->Position.Y, this->handler->Drawer->D3DColorFromDrawingColour(Text->Colour));
	}
}