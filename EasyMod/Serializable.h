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

#ifndef _SERIALIZABLE_H_
#define _SERIALIZABLE_H_

namespace EasyMod
{
	[System::SerializableAttribute]
	public enum class EventType
	{
		KeyUpEvent,
		KeyDownEvent,
		KeywordEvent,
		ConsoleCmdEvent,
	};
	
	[System::SerializableAttribute]
	public enum class ConsoleInputType
    {
        Command,
        Variable,
    };

	[System::SerializableAttribute]
	public ref class SerializableEventArgs : System::EventArgs 
	{
	public:
		SerializableEventArgs() { }
	};

	[System::SerializableAttribute]
	public ref class SerializableGraphicsEventArgs : SerializableEventArgs
	{

	};

	[System::SerializableAttribute]
	public ref class SerializableConsoleCmdEventArgs : SerializableEventArgs
	{
	private:
		System::String^ cmd_name;
		System::Collections::Generic::List<System::String^>^ args;
		ConsoleInputType input_type;

	public:
		SerializableConsoleCmdEventArgs(System::String^ CmdName, ConsoleInputType InputType, array<System::String^>^ Args) 
		{ 
			this->cmd_name = CmdName; 
			this->args = gcnew System::Collections::Generic::List<System::String^>(); 
			this->args->AddRange(Args);
			this->input_type = InputType;
		}

		property ConsoleInputType InputType { ConsoleInputType get() { return this->input_type; } }
		property System::String^ CmdName { System::String^ get() { return this->cmd_name; } }

		property System::Collections::Generic::List<System::String^>^ Arguments { System::Collections::Generic::List<System::String^>^ get() { return this->args; } }
	};

	[System::SerializableAttribute]
	public ref class KeywordEventArgs : SerializableEventArgs
	{
	private:
		System::String^ keyword;

	public:
		KeywordEventArgs(System::String^ Keyword) { this->keyword = Keyword; };

		property System::String^ Keyword
		{
			System::String^ get()
			{
				return this->keyword;
			}
		}
	};

	// Most of this class taken from HazardX's GTA::KeyEventsArgs class.
	[System::SerializableAttribute]
	public ref class SerializableKeyEventArgs : SerializableEventArgs
	{
	private:
		System::Windows::Forms::Keys winforms_key;
		System::Char key_char;

	public:
		SerializableKeyEventArgs(System::Windows::Forms::Keys Key) 
		{ 
			this->winforms_key = Key;

			
			BYTE keyboardstate[256];

			keyboardstate[VK_SHIFT] = (this->Shift) ? 0xFF : 0x00;
			keyboardstate[VK_CONTROL] = (this->Alt) ? 0xFF : 0x00;
			keyboardstate[VK_MENU] = (this->Alt) ? 0xFF : 0x00;

			WCHAR res[2];
			ToUnicode((UINT)Key, 0, keyboardstate, res, 2, 0);
			this->key_char = System::Char(res[0]);
		}

		/// <summary>
		/// Returns the key without any modifier information.
		/// </summary>
		property System::Windows::Forms::Keys Key 
		{
			System::Windows::Forms::Keys get() 
			{
				return (this->winforms_key & System::Windows::Forms::Keys::KeyCode);
			}
		}

		property System::Char KeyChar
		{
			System::Char get()
			{
				return this->key_char;
				/*
				if (this->Shift)
				{
					return (System::Char)this->Key;
				}
				else
				{
					return System::Char::ToLower((System::Char)this->Key);
				}
				*/
			}
		}

		/// <summary>
		/// Returns the key combined with modifier (Shift, Control, Alt) information.
		/// </summary>
		property System::Windows::Forms::Keys KeyWithModifiers 
		{
			System::Windows::Forms::Keys get() 
			{
				return this->winforms_key;
			}
		}

		/// <summary>
		/// Returns if the key was pressed in combination with Shift.
		/// </summary>
		property bool Shift 
		{
			bool get()
			{
				return ((this->winforms_key & System::Windows::Forms::Keys::Shift) != System::Windows::Forms::Keys::None);
			}
		}

		/// <summary>
		/// Returns if the key was pressed in combination with Control.
		/// </summary>
		property bool Control 
		{
			bool get()
			{
				return ((this->winforms_key & System::Windows::Forms::Keys::Control) != System::Windows::Forms::Keys::None);
			}
		}
		/// <summary>
		/// Returns if the key was pressed in combination with Alt.
		/// </summary>
		property bool Alt 
		{
			bool get() 
			{
				return ((this->winforms_key & System::Windows::Forms::Keys::Alt) != System::Windows::Forms::Keys::None);
			}
		}

		property bool IsModifier 
		{
			bool get() 
			{
				switch (this->Key) 
				{
					case System::Windows::Forms::Keys::ShiftKey: return true;
					case System::Windows::Forms::Keys::LShiftKey: return true;
					case System::Windows::Forms::Keys::RShiftKey: return true;
					case System::Windows::Forms::Keys::ControlKey: return true;
					case System::Windows::Forms::Keys::LControlKey: return true;
					case System::Windows::Forms::Keys::RControlKey: return true;
					case System::Windows::Forms::Keys::Menu: return true;
					case System::Windows::Forms::Keys::LMenu: return true;
					case System::Windows::Forms::Keys::RMenu: return true;
				}
				return false;
			}
		}

		property System::Windows::Forms::MouseButtons MouseButton
		{
			System::Windows::Forms::MouseButtons get()
			{
				switch (this->Key)
				{
					case System::Windows::Forms::Keys::LButton: return System::Windows::Forms::MouseButtons::Left;
					case System::Windows::Forms::Keys::RButton: return System::Windows::Forms::MouseButtons::Right;
					case System::Windows::Forms::Keys::MButton: return System::Windows::Forms::MouseButtons::Middle;
					case System::Windows::Forms::Keys::XButton1: return System::Windows::Forms::MouseButtons::XButton1;
					case System::Windows::Forms::Keys::XButton2: return System::Windows::Forms::MouseButtons::XButton2;
				}

				return System::Windows::Forms::MouseButtons::None;
			}
		}

		property bool IsMouseButton
		{
			bool get() 
			{
				return (this->MouseButton != System::Windows::Forms::MouseButtons::None);
			}
		}
	};

	[System::SerializableAttribute]
	public ref class ScriptEvent
	{
	private:
		SerializableEventArgs^ args;
		EventType type;

	public:
		ScriptEvent(SerializableEventArgs^ e, EventType TypeOfEvent)
		{
			this->args = e;
			this->type = TypeOfEvent;
		}

		property SerializableEventArgs^ Args
		{
			SerializableEventArgs^ get() { return this->args; }
		}

		property EventType TypeOfEvent
		{
			EventType get() { return this->type; }
		}
	};
}

#endif
