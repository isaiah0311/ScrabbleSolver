/**
 * @file
 * @author Isaiah Lateer
 *
 * Scrabble word finder
 */

#include <sstream>
#include <string>
#include <vector>

#include <windows.h>

#include "Resources.h"

 /**
  * Loads the dictionary resource file
  *
  * Handles locating and reading the dictionary resource file. The data is
  * loaded into a vector of strings, each element contains a single word
  * from the dictionary. If the dictionary is empty, an error occurred.
  *
  * @param instance is the handle to the program when loaded in memory
  * @return vector containing dictionary contents
  */
std::vector<std::string> loadDictionary(_In_ HINSTANCE instance) {
	std::vector<std::string> dictionary;

	HRSRC resInfo = FindResourceW(instance, MAKEINTRESOURCEW(ID_DICTIONARY), L"TXT");
	if (!resInfo)
		return {};

	HGLOBAL resData = LoadResource(instance, resInfo);
	if (!resData)
		return {};

	LPVOID res = LockResource(resData);
	if (!res)
		return {};

	DWORD size = SizeofResource(instance, resInfo);

	std::string data;
	data.assign(static_cast<char*>(res), size);
	std::stringstream stream(data);

	std::string word;
	while (std::getline(stream, word))
		dictionary.push_back(word);
	return dictionary;
}

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

	std::vector<std::string> dictionary = loadDictionary(instance);
	if (dictionary.empty())
		return EXIT_FAILURE;

	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	for (size_t i = 0; i < dictionary.size(); ++i) {
		WriteConsoleA(out, dictionary.at(i).c_str(), dictionary.at(i).size(), nullptr, nullptr);
		WriteConsoleA(out, "\r\n", 2, nullptr, nullptr);
	}

	system("pause");

	FreeConsole();
	return EXIT_SUCCESS;
}
