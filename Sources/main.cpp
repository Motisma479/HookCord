#include "pch.hpp"

#if defined(_MSC_VER) && defined(_DEBUG)
#include <crtdbg.h>
#endif

int main(int argc, char** argv)
{
#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// this row is used to stop at new/malloc N 284
	//_CrtSetBreakAlloc(284);
#endif

	std::cout << "Hello World!\n";

	return 0;
}
