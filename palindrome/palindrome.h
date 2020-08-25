#ifndef PALINDROME_PALINDROME_H
#define PALINDROME_PALINDROME_H

#include <string>

char to_lower(char letter)
{
  return static_cast<char>(std::tolower(static_cast<unsigned char>(letter)));
}

bool IsPalindrome(const std::string & word)
{
  auto word_length = word.length();
  for (int i = 0; i < (word_length-1)/2; ++i)
  {
    auto letter1 = to_lower(word[i]);
    auto letter2 = to_lower(word[word_length-1-i]);
    if(letter1 != letter2)
      return false;
  }
  return true;
}


#endif //PALINDROME_PALINDROME_H
