#pragma once

#include <fstream>

#ifdef LIGHT_EXPORTS
#define CONSOLE_API __declspec(dllexport)
#else
#define CONSOLE_API __declspec(dllimport)
#endif

class CONSOLE_API Console {
public:
	static std::ofstream logFile;
};
