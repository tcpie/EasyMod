/*
* Copyright (c) 2009-2011 Hazard (hazard_x@gmx.net / twitter.com/HazardX)
* 
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
*/

#include "stdafx.h"

#include "KeyWatchDog.h"
#include "Events.h"

#pragma managed

namespace GTA {

	KeyWatchDog::KeyWatchDog() 
	{
		this->keystate = gcnew array<bool>(256);
		this->bShift = false;
		this->bCtrl = false;
		this->bAlt = false;
		this->pModifier = System::Windows::Forms::Keys::None;

		this->keystate[0] = false;
		this->keystate[3] = false;
	}

	KeyWatchDog::~KeyWatchDog()
	{

	}

	System::Windows::Forms::Keys KeyWatchDog::RemoveRedundantKeyModifiers(System::Windows::Forms::Keys key) 
	{
		switch (key & System::Windows::Forms::Keys::KeyCode) 
		{
			case System::Windows::Forms::Keys::ShiftKey:
			case System::Windows::Forms::Keys::LShiftKey:
			case System::Windows::Forms::Keys::RShiftKey:
				return key & ~System::Windows::Forms::Keys::Shift;

			case System::Windows::Forms::Keys::ControlKey:
			case System::Windows::Forms::Keys::LControlKey:
			case System::Windows::Forms::Keys::RControlKey:
				return key & ~System::Windows::Forms::Keys::Control;

			case System::Windows::Forms::Keys::Menu:
			case System::Windows::Forms::Keys::LMenu:
			case System::Windows::Forms::Keys::RMenu:
				return key & ~System::Windows::Forms::Keys::Alt;
		}
		return key;
	}

	bool KeyWatchDog::isKeyPressedAsync(System::Windows::Forms::Keys key) 
	{
		return ((GetAsyncKeyState((int)key) & 0x8000) != 0);
	}

	void KeyWatchDog::CheckKeyAsync(System::Windows::Forms::Keys Key) 
	{
		bool pressed = this->isKeyPressedAsync(Key);

		if (pressed == keystate[(int)Key]) 
		{
			return;
		}

		this->keystate[(int)Key] = pressed;

		if (pressed)
		{
			this->OnKeyDown(gcnew EasyMod::SerializableKeyEventArgs(Key | Modifier));
		}
		else
		{
			this->OnKeyUp(gcnew EasyMod::SerializableKeyEventArgs(Key | Modifier));
		}
	}

	void KeyWatchDog::Check() 
	{
		bool stat;
		System::Windows::Forms::Keys key;		

		for (int i = 7; i < 255; i++) 
		{ // yes, 255 is NOT valid! // 0 undefined, 3 VK_CANCEL can be ignored, 1 2 4 5 6 mouse keys
			stat = GetAsyncKeyState(i) != 0; // ((this->ckeystate[i] & 0x80) != 0);
			
			if (stat != this->keystate[i]) 
			{
				if (i == VK_SHIFT)
				{
					this->bShift = stat;
				}
				else if (i == VK_CONTROL)
				{
					this->bCtrl = stat;
				}
				else if (i == VK_MENU)
				{
					this->bAlt = stat;
				}

				this->pModifier = (bShift ? System::Windows::Forms::Keys::Shift : System::Windows::Forms::Keys::None) | 
					  			  (bCtrl ? System::Windows::Forms::Keys::Control : System::Windows::Forms::Keys::None) | 
								  (bAlt ? System::Windows::Forms::Keys::Alt : System::Windows::Forms::Keys::None);

				key = this->RemoveRedundantKeyModifiers(System::Windows::Forms::Keys(i) | Modifier);

				if (stat)
				{
					this->OnKeyDown(gcnew EasyMod::SerializableKeyEventArgs(key));
				}
				else
				{
					this->OnKeyUp(gcnew EasyMod::SerializableKeyEventArgs(key));
				}

				keystate[i] = stat;
			}
		}

		this->CheckKeyAsync(System::Windows::Forms::Keys::LButton);
		this->CheckKeyAsync(System::Windows::Forms::Keys::RButton);
		this->CheckKeyAsync(System::Windows::Forms::Keys::MButton);
		this->CheckKeyAsync(System::Windows::Forms::Keys::XButton1);
		this->CheckKeyAsync(System::Windows::Forms::Keys::XButton2);
	}

	void KeyWatchDog::OnKeyDown(EasyMod::SerializableKeyEventArgs^ e) 
	{
		this->KeyDown(this, e);
		this->AnonymousKeyDown(e);
	}

	void KeyWatchDog::OnKeyUp(EasyMod::SerializableKeyEventArgs^ e) 
	{
		this->KeyUp(this, e);
		this->AnonymousKeyUp(e);
	}

}