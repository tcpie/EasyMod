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

#ifndef _GRAPHICSAPI_H_
#define _GRAPHICSAPI_H_

#include "D3D9Handler.h"

#include "ApiObjects.h"
#include "DrawingObjects.h"

namespace EasyMod
{
	public ref class GraphicsApi : System::MarshalByRefObject
	{
	private:
		System::Boolean in_frame;

		D3D9Handler^ handler;

		System::Collections::Generic::List<InternalDrawableFont^>^ fonts;

		System::Collections::Generic::List<System::Collections::Generic::List<IVisualizable^>^>^ depth_drawables;

		void DrawLine(DrawableLine^ Line);
		void DrawRectangle(DrawableRectangle^ Rectangle);
		int DrawText(DrawableText^ Text, InternalDrawableFont^ InternalFont);

		int VisualCompare(IVisualizable^ a, IVisualizable^ b);
		int VisualCompare(DrawableRectangle^ a, DrawableRectangle^ b);

		InternalDrawableFont^ getFont(DrawableFont^ Font);

	internal:
		GraphicsApi(D3D9Handler^ Handler);

		void onEndScene();
		void clearDrawables();

		property System::Boolean IsInFrame
		{
			System::Boolean get() { return this->in_frame; }
		}

	public:
		System::Boolean RegisterFont(DrawableFont^ Font);
		System::Boolean RemoveFont(DrawableFont^ Font);

		void RegisterDrawable(DrawableRectangle^ Rectangle);
		void RegisterDrawable(DrawableLine^ Line);
		void RegisterDrawable(DrawableText^ Text);

		System::Boolean RemoveDrawable(DrawableRectangle^ Rectangle);
		System::Boolean RemoveDrawable(DrawableLine^ Line);
		System::Boolean RemoveDrawable(DrawableText^ Text);
	};

}

#endif
