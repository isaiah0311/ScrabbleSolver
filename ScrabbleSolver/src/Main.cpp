/**
 * @file
 * @author Isaiah Lateer
 *
 * Scrabble word finder
 */

#include <algorithm>
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
	while (std::getline(stream, word)) {
		word.pop_back();
		dictionary.push_back(word);
	}

	return dictionary;
}

/**
 * Converts a letter into its corresponding point value
 *
 * @param letter that will be converted into a point value
 * @return point value of the given letter
 */
int convert(_In_ char const letter) {
	static int points[26] = { 1, 3, 3, 2, 1, 4, 2, 4, 1, 8, 5, 1, 3, 1, 1, 3,
		10, 1, 1, 1, 1, 4, 4, 8, 4, 10 };
	if (letter >= 'A' && letter <= 'Z')
		return points[static_cast<int>(letter - 'A')];
	else if (letter >= 'a' && letter <= 'z')
		return points[static_cast<int>(letter - 'a')];
	return 0;
}

/**
 * Calculates a word into its total point value
 *
 * @param word is a string containing the word to be calculated
 * @return point total of the given word
 */
int calculate(_In_ std::string word) {
	int points = 0;
	for (char letter : word)
		points += convert(letter);

	return points;
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

	char letters[27] = {};
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
		else if (buffer[i] == '?')
			++letters[26];
	}

	std::vector<std::pair<std::string, int>> words;
	for (std::string word : dictionary) {
		char frequency[26] = {};
		for (size_t i = 0; i < word.length(); ++i) {
			if (word.at(i) >= 'A' && word.at(i) <= 'Z')
				++frequency[word.at(i) - 'A'];
			else if (word.at(i) >= 'a' && word.at(i) <= 'z')
				++frequency[word.at(i) - 'a'];
		}

		bool valid = true;
		int blanks = letters[26];
		int points = calculate(word);
		for (int i = 0; i < 26; ++i) {
			if (frequency[i] > letters[i]) {
				if (blanks >= frequency[i] - letters[i]) {
					blanks -= frequency[i] - letters[i];
					points -= convert(static_cast<char>(i + 'A')) * (frequency[i] - letters[i]);
				} else {
					valid = false;
					break;
				}
			}
		}

		if (valid) {
			words.push_back(make_pair(word, points));
		}
	}

	std::sort(words.begin(), words.end(), [](_In_ std::pair<std::string, int> a, _In_ std::pair<std::string, int> b) {
		if (a.second == b.second) {
			if (a.first.length() == b.first.length()) {
				for (size_t i = 0; i < a.first.length(); ++i) {
					if (a.first.at(i) == b.first.at(i))
						continue;
					return a.first.at(i) < b.first.at(i);
				}
			}

			return a.first.length() < b.first.length();
		}
		
		return a.second < b.second;
	});

	if (words.empty()) {
		if (WriteConsoleA(out, "No results\r\n", 12, nullptr, NULL) == ERROR)
			return EXIT_FAILURE;
	} else {
		for (std::pair<std::string, int> pair : words) {
			std::string word = pair.first + " (" + std::to_string(pair.second) + ")\r\n";
			if (WriteConsoleA(out, word.c_str(), word.length(), nullptr, NULL) == ERROR)
				return EXIT_FAILURE;
		}
	}

	system("pause");

	if (FreeConsole() == ERROR)
		return EXIT_FAILURE;
	return EXIT_SUCCESS;
}
