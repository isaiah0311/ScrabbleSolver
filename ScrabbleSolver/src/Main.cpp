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

constexpr int IDM_BUTTON = 101;

 /**
  * Loads the dictionary resource file
  *
  * Handles locating and reading the dictionary resource file. The data is
  * loaded into a vector of strings, each element contains a single word
  * from the dictionary.
  *
  * @param instance is the handle to the program when loaded in memory
  * @return vector containing resource contents
  */
std::vector<std::string> loadDictionary(_In_ HINSTANCE instance) {
	std::vector<std::string> dictionary;

	HRSRC resInfo = FindResourceW(instance, MAKEINTRESOURCEW(ID_DICTIONARY),
		L"TXT");
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
 * Finds words that can be made from a list of letters
 * 
 * Takes in a list of letters and a dictionary. Each word in the dictionary is
 * checked to see if it can be made using the given letters. Blank letters are
 * represented using a question mark. After the entire dictionary has been
 * iterated over, the words that can be made are sorted by point value.
 * 
 * @param dictionary is the word list that will be iterated over
 * @param input is the letters to be used when solving for words
 * @return string containing a list of words that can be made
 */
std::string solve(_In_ std::vector<std::string> dictionary, _In_ char* input) {
	char letters[27] = {};
	for (size_t i = 0; i < strlen(input); ++i) {
		if (input[i] >= 'A' && input[i] <= 'Z')
			++letters[input[i] - 'A'];
		else if (input[i] >= 'a' && input[i] <= 'z')
			++letters[input[i] - 'a'];
		else if (input[i] == '?')
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
					points -= convert(static_cast<char>(i + 'A')) *
						(frequency[i] - letters[i]);
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

	std::sort(words.begin(), words.end(), [](
		_In_ std::pair<std::string, int> a,
		_In_ std::pair<std::string, int> b) {
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

	std::string results;
	if (words.empty())
		results = "No results";
	else {
		for (std::pair<std::string, int> pair : words)
			results.append(pair.first + " (" + std::to_string(pair.second) +
				")\r\n");
	}

	results.pop_back();
	results.pop_back();
	return results;
}

/**
 * Processes messages sent to a window
 * 
 * Window procedure for the application's main window. On window creation,
 * three child windows are created and the dictionary resource file is
 * loaded.
 *
 * @param window is a handle to the window
 * @param msg contains the message value
 * @param wParam contains additional message information
 * @param lParam contains additional message information
 * @return result of the message processing
 */
LRESULT CALLBACK procedure(_In_ HWND window, _In_ unsigned int msg,
	_In_ WPARAM wParam, _In_ LPARAM lParam) {
	static std::vector<std::string> dictionary;
	static HWND input;
	static HWND button;
	static HWND output;

	LRESULT result = 0;

	switch (msg) {
		case WM_CREATE:
		{
			HINSTANCE instance = GetModuleHandleW(nullptr);
			dictionary = loadDictionary(instance);

			input = CreateWindowExW(NULL, L"Edit", nullptr, WS_CHILD |
				WS_VISIBLE | ES_UPPERCASE, 10, 10, 125, 20, window, nullptr,
				instance, nullptr);
			button = CreateWindowExW(NULL, L"Button", L"Solve", WS_CHILD |
				WS_VISIBLE, 145, 10, 80, 20, window,
				reinterpret_cast<HMENU>(IDM_BUTTON), instance, nullptr);
			output = CreateWindowExW(NULL, L"Edit", nullptr, WS_CHILD |
				WS_VISIBLE | WS_VSCROLL | ES_READONLY | ES_UPPERCASE |
				ES_MULTILINE, 10, 40, 565, 310, window, nullptr, instance,
				nullptr);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CLOSE:
			DestroyWindow(window);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDM_BUTTON:
				{
					char text[16] = {};
					if (GetWindowTextA(input, text, 16)) {
						std::string words = solve(dictionary, text);
						SetWindowTextA(output, words.c_str());
					}

					break;
				}
			}
			break;
		default:
			result = DefWindowProcW(window, msg, wParam, lParam);
			break;
	}

	return result;
}

/**
 * Program entry-point
 *
 * Standard Windows entry-point for C and C++ programs. Creates the window and
 * loops over window messages until the window closes, then exits.
 *
 * @param instance is the handle to the program when loaded in memory
 * @param prevInstance is used for backwards compatability with 16-bit Windows
 * @param cmdLine contains command-line arguments as a Unicode string
 * @param cmdShow is a flag used for how the application window is displayed
 * @return exit status
 */
int WINAPI wWinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prevInstance,
	_In_ PWSTR cmdLine, _In_ int cmdShow) {
	WNDCLASSEXW windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEXW);
	windowClass.lpfnWndProc = procedure;
	windowClass.hInstance = instance;
	windowClass.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_BACKGROUND);
	windowClass.lpszClassName = L"Scrabble Solver";
	RegisterClassExW(&windowClass);

	HWND window = CreateWindowExW(NULL, windowClass.lpszClassName,
		L"Scrabble Solver", WS_MINIMIZEBOX | WS_SYSMENU, CW_USEDEFAULT,
		CW_USEDEFAULT, 600, 400, nullptr, nullptr, instance, nullptr);
	ShowWindow(window, cmdShow);

	MSG msg = {};
	while (GetMessageW(&msg, window, NULL, NULL) > 0) {
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	return EXIT_SUCCESS;
}
