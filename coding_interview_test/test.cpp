#define CATCH_CONFIG_MAIN  // This tells Catch to provide the main()
#define CATCH_CONFIG_ENABLE_BENCHMARKING
#include "catch.hpp"  //! unit test library

#include <bitset>
#include <cstring>
#include <unordered_map>
#include <utility>
#include <unordered_set>
#include <queue>
#include <array>

//! 1. Write a function that takes an unsigned integer as input, and returns true if all the digits in the
//! base 10 representation of that number are unique.

bool AllDigitsUnique(unsigned int digit)
{
    std::bitset<10> bb;

    do
    {
        auto index = digit%10;
        if(bb[index])
            return false;
        bb.set(index);
        digit /= 10;
    } while (digit > 0);

    return true;
}

TEST_CASE("All digit should to be unique in base of 10")
{
    REQUIRE(AllDigitsUnique(48778584) == false);
    REQUIRE(AllDigitsUnique(17308459));
    REQUIRE(AllDigitsUnique(12345678));
    REQUIRE(AllDigitsUnique(98765432));
    REQUIRE(AllDigitsUnique(0));
    REQUIRE(AllDigitsUnique(4353464) == false);
}

//! --------------------------------------------------------------------------

//! 2. Write a function that modifies an input string, sorting the letters according to a sort order
//! defined by a second string. You may assume that every character in the input string also appears
//! somewhere in the sort order string. Make your function as fast as possible for long input strings.

//! fast and simple, works for extended ASCII encoding table
//! if it can assume that it will not use extended ASCII table can decreased in size
void SortLetters(char* input, const char* sortOrder)
{
    int table [std::numeric_limits<unsigned char>::max()] = {0};

    for(const char* ptr = input; *ptr; ++ptr)
    {
        auto symbol = *ptr;
        table[symbol]++;
    }

    char* head = input;
    for(const char* ptr = sortOrder; *ptr; ++ptr)
    {
        auto symbol = *ptr;
        auto symbolCount = table[symbol];
        for(int i = 0; i < symbolCount; ++i)
        {
            *head=symbol;
            head++;
        }
    }
}

//! more slower than the first one, but can be extended to use with wide character set
void SortLetters2(char* input, const char* sortOrder)
{
    auto size = std::strlen(sortOrder);
    std::unordered_map<char, int> table(size*2);

    for(const char* ptr = input; *ptr; ++ptr)
    {
        auto symbol = *ptr;
        table[symbol]++;
    }

    char* head = input;
    for(const char* ptr = sortOrder; *ptr; ++ptr)
    {
        auto symbol = *ptr;
        auto symbolCount = table[symbol];
        for(int i = 0; i < symbolCount; ++i)
        {
            *head=symbol;
            head++;
        }
    }
}

char* GetLongString(const char* pattern)
{
    std::default_random_engine rGen;
    auto size = std::strlen(pattern);
    std::uniform_int_distribution<int> rand(0, size-1);
    const int stringSize = 4096;
    char* result = new char[stringSize+1];
    result[stringSize] = 0;
    for(int i = 0; i < stringSize; ++i)
    {
        result[i] = pattern[rand(rGen)];
    }
    return result;
}

TEST_CASE("SortLetters")
{
    char* inputStr = strdup("trion world network");
    SortLetters(inputStr, " oinewkrtdl");
    REQUIRE(std::strcmp(inputStr, "  oooinnewwkrrrttdl") == 0);
}

TEST_CASE("SortLetters2")
{
    char* inputStr = strdup("trion world network");
    SortLetters2(inputStr, " oinewkrtdl");
    REQUIRE(std::strcmp(inputStr, "  oooinnewwkrrrttdl") == 0);

    const char* pattern = "zaswdefrgthbnm; ";
    auto long_str1 = GetLongString(pattern);
    auto long_str2 = strdup(long_str1);

    BENCHMARK("SortLetters")
    {
        return SortLetters(long_str1, pattern);
    };

    BENCHMARK("SortLetters2")
    {
        return SortLetters2(long_str2, pattern);
    };
}

//! -----------------------------------------------------------------

//! 3. Imagine a maze of interconnected rooms:
//! - Each room has a unique name, and can be connected to between zero and four other
//! rooms through doors named north, south, east, and west.
//! - Rooms aren’t necessarily spatially coherent. If A is north of B, B might not be south
//! of A.
//! - Doors aren’t necessarily bidirectional. If A can be reached from B, B might not be
//! reachable from A.
//! - Rooms might connect to themselves.
//! 3a. Write a class or struct declaration that shows the structure of a Room. Include a constructor
//! (for classes) or initialization function (for structs).
//! 3b. Write a function to connect a new Room to an existing Room.
//! 3c. Write a function to determine if any path exists between a starting room and an ending room
//! with a given name. You may add additional members and methods to your Room class or struct
//! as needed. You don’t need to worry about finding the shortest path. It’s only necessary to
//! determine if any path exists.

class Room
{
public:
    explicit Room(std::string name, Room* north = nullptr, Room* south = nullptr, Room* east = nullptr, Room* west = nullptr)
    : name(std::move(name))
    , north(north)
    , south(south)
    , east(east)
    , west(west)
    {
    }

    void ConnectToNorth(Room* room) { north = room; }
    void ConnectToSouth(Room* room) { south = room; }
    void ConnectToEast(Room* room)  { east  = room; }
    void ConnectToWest(Room* room)  { west  = room; }

    bool PathExistsTo(const char* endingRoomName)
    {
        std::unordered_set<Room*> visited;
        // load factor for more perfomance 
        visited.max_load_factor(0.6); 
        std::queue<Room*> open_set;
        open_set.push(this);

        while (!open_set.empty())
        {
            auto next = open_set.front();
            open_set.pop();

            visited.insert(next);
            // if found destination room - stop search
            if(next->GetName() == endingRoomName)
                return true;

            auto neighbors = next->GetNeighbors();
            for (Room* room : neighbors)
            {
                if(room == nullptr || visited.count(room))
                    continue;
                open_set.push(room);
            }
        }

        return false;
    }

    std::array<Room*, 4> GetNeighbors() const
    {
        return std::array<Room*, 4> {north, south, east, west};
    }

    std::string GetName() const { return name; }

private:
    std::string name;
    Room* north = nullptr;
    Room* south = nullptr;
    Room* east  = nullptr;
    Room* west  = nullptr;
};

TEST_CASE("PathToRoom")
{
    Room D("D"), E("E"), F("F"), G("G"), C("C");
    Room B("B");
    B.ConnectToWest(&B);
    Room A("A");
    A.ConnectToEast(&B);
    A.ConnectToNorth(&E);
    B.ConnectToNorth(&C);
    B.ConnectToSouth(&A);
    C.ConnectToEast(&D);
    C.ConnectToWest(&A);
    D.ConnectToSouth(&E);
    E.ConnectToEast(&G);
    F.ConnectToSouth(&G);
    G.ConnectToSouth(&F);
    E.ConnectToNorth(&D);
    E.ConnectToWest(&B);

    REQUIRE(A.PathExistsTo("G"));
    REQUIRE(!G.PathExistsTo("A"));
    REQUIRE(!F.PathExistsTo("A"));
    REQUIRE(E.PathExistsTo("A"));
    REQUIRE(D.PathExistsTo("A"));
}

//! -------------------------------------------------------------------
