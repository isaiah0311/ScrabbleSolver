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
 * Standard Windows entry-point for C and C++ programs. Loads the dictionary
 * resource file and then allocates a console. Prompts the user for their
 * letters and then outputs all possible words that can be made.
 *
 * @param instance is the handle to the program when loaded in memory
 * @param prevInstance is used for backwards compatability with 16-bit Windows
 * @param cmdLine contains command-line arguments as a Unicode string
 * @param cmdShow is a flag used for how the application window is displayed
 * @return exit status
 */
int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance,
	_In_ PWSTR cmdLine, _In_ int cmdShow) {
	std::vector<std::string> dictionary = loadDictionary(instance);
	if (dictionary.empty())
		return EXIT_FAILURE;

	if (AllocConsole() == ERROR)
		return EXIT_FAILURE;

	HANDLE in = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
	if (WriteConsoleA(out, "Enter your letters: ", 20, nullptr, NULL) == ERROR)
		return EXIT_FAILURE;

	char buffer[15] = {};
	DWORD chars = 0;
	if (ReadConsoleA(in, buffer, sizeof(buffer), &chars, NULL) == ERROR)
		return EXIT_FAILURE;

	char letters[26] = {};
	for (DWORD i = 0; i < chars; ++i) {
		if (buffer[i] == '\r') {
			buffer[i] = '\0';
			chars = i;
			break;
		}

		if (buffer[i] >= 'A' && buffer[i] <= 'Z')
			++letters[buffer[i] - 'A'];
		else if (buffer[i] >= 'a' && buffer[i] <= 'z')
			++letters[buffer[i] - 'a'];
	}

	std::vector<std::string> words;
	for (std::string word : dictionary) {
		char frequency[26] = {};
		for (size_t i = 0; i < word.length(); ++i) {
			if (word.at(i) >= 'A' && word.at(i) <= 'Z')
				++frequency[word.at(i) - 'A'];
			else if (word.at(i) >= 'a' && word.at(i) <= 'z')
				++frequency[word.at(i) - 'a'];
		}

		bool valid = true;
		for (int i = 0; i < 26; ++i) {
			if (frequency[i] > letters[i]) {
				valid = false;
				break;
			}
		}

		if (valid)
			words.push_back(word);
	}

	for (std::string word : words) {
		word.append("\r\n");
		if (WriteConsoleA(out, word.c_str(), word.length(), nullptr, NULL) == ERROR)
			return EXIT_FAILURE;
	}

	system("pause");

	if (FreeConsole() == ERROR)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
