#include "stdafx.h"
#include "CKeywordEngine.h"

using namespace std;

namespace tcpie { namespace util {

	struct keyword
	{
		wstring keyWord;
		unsigned int counter;
		CALLBACK_FUNC functionToCall;

		keyword(wstring word, CALLBACK_FUNC functionToCall)
		{
			this->counter = 0;
			this->keyWord = word;
			this->functionToCall = functionToCall;
		}
	};

	WCHAR GetCharFromKeyCode(unsigned int code)
	{
		WCHAR buf;
		BYTE kbState[256];

		if (!GetKeyboardState(kbState))
		{
			return NULL;
		}

		int retVal = ToUnicode(code, 0, kbState, &buf, 1, 0);

		if (retVal != 1)
		{
			return NULL;
		}

		return buf;
	}

	CKeywordEngine::CKeywordEngine()
	{
		for (int i = 0; i < 0xB8; i++)
		{
			this->keyPressed[i] = false;
		}
	}

	void CKeywordEngine::AddKeyword(std::wstring word, CALLBACK_FUNC callback)
	{
		this->keywords.push_back(new keyword(word, callback));
	}

	void CKeywordEngine::HandleLoop()
	{
		for (int i = 0; i < 0xB8; i++)
		{
			int vkey = i + 0x09;

			if (GetAsyncKeyState(vkey) != 0)
			{
				// Key down
				if (!this->keyPressed[i])
				{
					this->keyPressed[i] = true;

					WCHAR c = GetCharFromKeyCode(vkey);

					for (unsigned int j = 0; j < this->keywords.size(); j++)
					{
						WCHAR compChar = this->keywords[j]->keyWord.c_str()[keywords[j]->counter];
						std::wstring word = this->keywords[j]->keyWord;

						// wcout << L"comparing " << c << L" to " << compChar << L" , char " << keywords[j]->counter << L" of << " << word << endl;
					
						if(compChar == c)
						{
							this->keywords[j]->counter++;

							if (this->keywords[j]->counter >= this->keywords[j]->keyWord.length())
							{
								// we are done
								this->keywords[j]->counter = 0;
								this->keywords[j]->functionToCall(this, this->keywords[j]->keyWord);
							}
						}
						else
						{
							this->keywords[j]->counter = 0;
							compChar = this->keywords[j]->keyWord.c_str()[keywords[j]->counter];

							if (compChar == c)
							{
								this->keywords[j]->counter++;

								if (this->keywords[j]->counter >= this->keywords[j]->keyWord.length())
								{
									// we are done
									this->keywords[j]->counter = 0;
									this->keywords[j]->functionToCall(this, this->keywords[j]->keyWord);
								}
							}
						}
					}
				}
			}
			else
			{
				this->keyPressed[i] = false;
			}
		}
	}

} }
