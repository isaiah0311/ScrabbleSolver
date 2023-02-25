/**
 * @file
 * @author Isaiah Lateer
 *
 * Scrabble word finder
 */

#include <string>

#include <windows.h>

#include "Resources.h"

/**
 * Program entry-point
 *
 * Standard Windows entry-point for C and C++ programs. Allocates a console
 * and then loads the dictionary resource file. Prints the entire Scrabble
 * dictionary and then pauses before exiting.
 *
 * @param instance is the handle to the program when loaded in memory
 * @param prevInstance is used for backwards compatability with 16-bit Windows
 * @param cmdLine contains command-line arguments as a Unicode string
 * @param cmdShow is a flag used for how the application window is displayed
 * @return exit status
 */
int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance,
	_In_ PWSTR cmdLine, _In_ int cmdShow) {
	AllocConsole();

	HRSRC resInfo = FindResourceW(instance, MAKEINTRESOURCEW(ID_DICTIONARY), L"TXT");
	if (!resInfo)
		return EXIT_FAILURE;

	HGLOBAL resData = LoadResource(instance, resInfo);
	if (!resData)
		return EXIT_FAILURE;

	LPVOID res = LockResource(resData);
	if (!res)
		return EXIT_FAILURE;

	DWORD size = SizeofResource(instance, resInfo);

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	std::string dictionary = static_cast<char*>(res);
	WriteConsoleA(out, dictionary.c_str(), size, nullptr, nullptr);
	WriteConsoleA(out, "\r\n", 2, nullptr, nullptr);
	system("pause");

	FreeConsole();
	return EXIT_SUCCESS;
}
