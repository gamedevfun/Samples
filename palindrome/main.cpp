#include <iostream>
#include "palindrome.h"

using std::string;

int main()
{
  string text("Tacocat");
  std::cout<<std::boolalpha;
  std::cout<<text<<" is palindrome - "<<IsPalindrome(text)<<std::endl;
  return 0;
}
