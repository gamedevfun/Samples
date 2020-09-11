## Samples
Some code samples from taken courses and coding interview tasks
  
### Coding interview test: 
It using test framework [Catch](https://github.com/catchorg/Catch2) https://github.com/catchorg/Catch2


1. Write a function that takes an unsigned integer as input, and returns true if all the digits in the
base 10 representation of that number are unique.
bool AllDigitsUnique(unsigned int value)  
Example:  
AllDigitsUnique(48778584) -> false
AllDigitsUnique(17308459) -> true

2. Write a function that modifies an input string, sorting the letters according to a sort order
defined by a second string. You may assume that every character in the input string also appears
somewhere in the sort order string. Make your function as fast as possible for long input strings.  
*void SortLetters(char* input, char* sortOrder)*  
Example:    
*char\* inputStr = strdup(“trion world network”);*
*SortLetters(inputStr, “ oinewkrtdl”);*  
would modify the input string to “
oooinnewwkrrrttdl”.

3. Imagine a maze of interconnected rooms:
    - Each room has a unique name, and can be connected to between zero and four other
rooms through doors named north, south, east, and west.
    - Rooms aren’t necessarily spatially coherent. If A is north of B, B might not be south
of A.
    - Doors aren’t necessarily bidirectional. If A can be reached from B, B might not be
reachable from A.
    - Rooms might connect to themselves.    
a. Write a class declaration that shows the structure of a Room. 
b. Write a function to connect a new Room to an existing Room.  
c. Write a function to determine if any path exists between a starting room and an ending room
with a given name. You may add additional members and methods to your Room class or struct
as needed. You don’t need to worry about finding the shortest path. It’s only necessary to
determine if any path exists.
Use this prototype:
bool Room::PathExistsTo(const char* endingRoomName);

4. Design an elevator controller for a building of ten floors (including the ground floor).
Your ElevatorController class should implement the interface in ElevatorControllerBase and be
constructed with a reference to an ElevatorMotor object.
Design your controller to achive the overall-most-efficient system. The definition of overall-
most-efficient is up to you.
```cpp
        class ElevatorMotor
	    {
		public:
	       	enum Direction { Up = 1, None = 0, Down = -1 };
	       int getCurrentFloor();
	       Direction getCurrentDirection();
	       void goToFloor(int floor);
	    }
    	class ElevatorControllerBase
    	{
    	public:
    	// called when an up or down button was pushed on a floor
    	virtual void summonButtonPushed(int summoningFloor,
    	ElevatorMotor::Direction direction) = 0;
    	// called when a button for a floor is pushed inside the car
    	virtual void floorButtonPushed(int destinationFloor) = 0;
    	// called when the car has reached a particular floor
    	virtual void reachedFloor(int floor) = 0;
    	}
```

### Other samples: 
1. **Hex game** - another implementation of game for terminal using C++17 for more info about game [see](https://en.wikipedia.org/wiki/Hex_(board_game))
2. **Palindrome** - determine if word is palindrome.
