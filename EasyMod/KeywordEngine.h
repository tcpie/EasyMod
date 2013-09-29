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

#ifndef _KEYWORDENGINE_H_
#define _KEYWORDENGINE_H_

#include "Events.h"
#include "Serializable.h"

namespace EasyMod
{
	ref struct Keyword;

	ref class KeywordEngine
	{
	private:
		System::Collections::Generic::List<Keyword^>^ keywords;
		void raiseKeywordEntered(KeywordEventArgs^ e);

	internal:
		KeywordEngine();

		void keyDown(System::Object^ sender, SerializableKeyEventArgs^ e);
		event KeywordEventHandler^ KeywordEntered;
		void AddKeyword(System::String^ Word);
	};
}

#endif
