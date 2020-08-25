#define CATCH_CONFIG_MAIN  // This tells Catch to provide the main()
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"  //! unit test library

#include "palindrome.h"

TEST_CASE("All letter should make palindrome - first half of word should be equal to another one")
{
  REQUIRE(IsPalindrome("48778584") == false); // not palindrome
  REQUIRE(IsPalindrome("Tacocat"));           // palindrome with first capital letter
  REQUIRE(IsPalindrome("aibohphobia"));
  REQUIRE(IsPalindrome("ailihphilia"));
  REQUIRE(IsPalindrome("DeTartRatED"));       // palindrome with mixed letters
  REQUIRE(IsPalindrome("rotor"));
  REQUIRE(IsPalindrome("madam"));
  REQUIRE(IsPalindrome("maqam"));
  REQUIRE(IsPalindrome("melem"));
  REQUIRE(IsPalindrome("minim"));
  REQUIRE(IsPalindrome("mmhmm"));
  REQUIRE(IsPalindrome("neven"));
  REQUIRE(IsPalindrome("nisin"));
  REQUIRE(IsPalindrome("radar"));
  REQUIRE(IsPalindrome("refer"));
  REQUIRE(IsPalindrome("rever"));
}