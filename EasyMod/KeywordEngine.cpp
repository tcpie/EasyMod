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
#include "KeywordEngine.h"

#include "Serializable.h"

#pragma managed

namespace EasyMod
{
	ref struct Keyword
	{
	public:
		System::String^ Word;
		int Count;

		Keyword(System::String^ Word)
		{
			this->Word = Word;
			this->Count = 0;
		}
	};

	KeywordEngine::KeywordEngine()
	{
		this->keywords = gcnew System::Collections::Generic::List<Keyword^>();
	}

	void KeywordEngine::raiseKeywordEntered(KeywordEventArgs^ e)
	{
		this->KeywordEntered(this, e);
	}

	void KeywordEngine::AddKeyword(System::String^ Word)
	{
		// Check if we already have word
		for (int i = 0; i < this->keywords->Count; i++)
		{
			if (this->keywords[i]->Word == Word)
			{
				return;
			}
		}

		this->keywords->Add(gcnew Keyword(Word));
	}

	void KeywordEngine::keyDown(System::Object^ sender, SerializableKeyEventArgs^ e)
	{
		System::Char keychar = e->KeyChar;

		for (int i = 0; i < this->keywords->Count; i++)
		{
			System::Char compchar = this->keywords[i]->Word[this->keywords[i]->Count];
			//System::Console::WriteLine("Comparing '" + compchar + "' to '" + keychar + "' AppDomain: " + System::AppDomain::CurrentDomain->FriendlyName);

			if (compchar == keychar)
			{
				this->keywords[i]->Count++;

				if (this->keywords[i]->Count >= this->keywords[i]->Word->Length)
				{
					//System::Console::WriteLine("Keyword \"" + this->keywords[i]->Word + "\" complete.");

					// We are done
					this->keywords[i]->Count = 0;
					this->raiseKeywordEntered(gcnew KeywordEventArgs(this->keywords[i]->Word));
				}
			}
			else
			{
				this->keywords[i]->Count = 0;
				compchar = this->keywords[i]->Word[this->keywords[i]->Count];

				if (compchar == keychar)
				{
					this->keywords[i]->Count++;

					if (this->keywords[i]->Count >= this->keywords[i]->Word->Length)
					{
						//System::Console::WriteLine("Keyword \"" + this->keywords[i]->Word + "\" complete.");

						// We are done
						this->keywords[i]->Count = 0;
						this->raiseKeywordEntered(gcnew KeywordEventArgs(this->keywords[i]->Word));
					}
				}
			}
		}
	}
}