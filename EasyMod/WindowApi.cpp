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
#include "WindowApi.h"

namespace EasyMod
{
	WindowApi::WindowApi(System::IntPtr Handle, bool IsConsoleWindow)
	{
		this->handle = Handle;
		this->is_console_window = IsConsoleWindow;
	}

	System::String^ WindowApi::GetCaption()
	{
		int len = GetWindowTextLengthW((HWND)this->handle.ToPointer());
		WCHAR* buf = new WCHAR[len];

		GetWindowTextW((HWND)this->handle.ToPointer(), buf, len);

		return gcnew System::String(buf);
	}

	System::Drawing::Rectangle WindowApi::GetDrawableArea()
	{
		System::Drawing::Rectangle ret_val;
		ret_val.X = 0;
		ret_val.Y = 0;
		ret_val.Width = 0;
		ret_val.Height = 0;

		RECT found_val;

		bool success = GetClientRect((HWND)this->handle.ToPointer(), &found_val) != 0;

		if (!success)
		{
			return ret_val;
		}

		ret_val.X = found_val.left;
		ret_val.Y = found_val.top;
		ret_val.Width = found_val.right - found_val.left;		
		ret_val.Height = found_val.bottom - found_val.top;

		return ret_val;
	}

	System::Drawing::Rectangle WindowApi::GetSizeAndPos()
	{
		System::Drawing::Rectangle ret_val;
		ret_val.X = 0;
		ret_val.Y = 0;
		ret_val.Width = 0;
		ret_val.Height = 0;

		RECT found_val;

		bool success = GetWindowRect((HWND)this->handle.ToPointer(), &found_val) != 0;

		if (!success)
		{
			return ret_val;
		}

		ret_val.X = found_val.left;
		ret_val.Y = found_val.top;
		ret_val.Width = found_val.right - found_val.left;		
		ret_val.Height = found_val.bottom - found_val.top;

		return ret_val;
	}

	void WindowApi::SetSizeAndPos(System::Drawing::Rectangle value)
	{
		SetWindowPos((HWND)this->handle.ToPointer(), NULL, value.X, value.Y, value.Width, value.Height, SWP_ASYNCWINDOWPOS | SWP_SHOWWINDOW | SWP_NOZORDER);
	}
}