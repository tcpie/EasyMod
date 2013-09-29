#include "stdafx.h"

#include <vector>

namespace tcpie { namespace util {

	typedef void (*CALLBACK_FUNC)(void* sender, std::wstring word);

	struct keyword;

	class CKeywordEngine
	{
	private:
		bool keyPressed[0xB8];
		std::vector<keyword*> keywords;
		int keywords_len;

	public:
		CKeywordEngine();
	
		void AddKeyword(std::wstring word, CALLBACK_FUNC callback);
		void HandleLoop();
	};

} }
