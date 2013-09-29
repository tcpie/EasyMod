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

#ifndef _APIOBJECTS_H_
#define _APIOBJECTS_H_

namespace EasyMod
{
	[System::SerializableAttribute]
	public value struct Vector2
	{
	public:
		System::Double X;
		System::Double Y;

		Vector2(System::Double X, System::Double Y) { this->X = X; this->Y = Y; }
	};

	[System::SerializableAttribute]
	public value struct Vector3
	{
	public:
		System::Double X;
		System::Double Y;
		System::Double Z;

		Vector3(System::Double X, System::Double Y, System::Double Z) { this->X = X; this->Y = Y; this->Z = Z; }
	};
}

#endif
