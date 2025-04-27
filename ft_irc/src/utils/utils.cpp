#include "utils/utils.hpp"

#include <algorithm>
#include <iostream>

#include "utils/color.hpp"

void printError(const std::string& message) {
  std::cerr << RED "Error: " << message << RESET << std::endl;
}

std::string toIrcCharacters(const std::string& str) {
  std::string result;
  for (size_t i = 0; i < str.size(); ++i) {
    if (str[i] == '{') {
      result += '[';
    } else if (str[i] == '}') {
      result += ']';
    } else if (str[i] == '|') {
      result += '\\';
    } else if (str[i] == '^') {
      result += '~';
    } else {
      result += str[i];
    }
  }
  return result;
}

static bool matchHelper(const std::string& text, size_t i,
                        const std::string& pattern, size_t j) {
  while (j < pattern.size()) {
    char p = pattern[j];

    switch (p) {
      case '\\': {
        // エスケープ処理
        if (j + 1 >= pattern.size())
          return false;
        if (i >= text.size() || text[i] != pattern[j + 1])
          return false;
        i++;
        j += 2;
        break;
      }
      case '*': {
        // '*' は 0 文字以上にマッチ。すべてのスキップ長を試す
        while (j + 1 < pattern.size() && pattern[j + 1] == '*')
          j++;  // '**' → '*'
        for (size_t skip = 0; i + skip <= text.size(); ++skip) {
          if (matchHelper(text, i + skip, pattern, j + 1))
            return true;
        }
        return false;
      }
      case '?': {
        if (i >= text.size())
          return false;
        i++;
        j++;
        break;
      }
      default: {
        if (i >= text.size() || text[i] != p)
          return false;
        i++;
        j++;
        break;
      }
    }
  }
  return i == text.size();
}

/**
 * * @brief Compare two strings for equality, with an option for
 * case-insensitive comparison.
 * * @param str1 The first string to compare.
 * * @param str2 The second string to compare.
 * * @param insensitive If true, perform a case-insensitive comparison.
 * * @return true if the strings are equal, false otherwise.
 * * @details Wildcards are supported in the comparison. The following
 * *          wildcards are supported:
 * *         - '*' matches any sequence of characters
 * *         - '?' matches any single character
 */
bool ircEqual(const std::string& text, const std::string& pattern,
              bool insensitive) {
  std::string textStr = text;
  std::string patternStr = pattern;
  if (insensitive) {
    std::transform(textStr.begin(), textStr.end(), textStr.begin(), ::tolower);
    std::transform(patternStr.begin(), patternStr.end(), patternStr.begin(),
                   ::tolower);
  }
  return matchHelper(textStr, 0, patternStr, 0);
}
