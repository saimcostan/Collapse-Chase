Our game idea takes inspiration from the game “tag” developed by Jet games. 
The games give options to play as 2 player where one of the players, at random, is picked as that “tagged” and chases the others to tag them. 
This goes on until the map either shrinks, or a time limit is achieved.

The game has gone through a number of stages and amendments as the game development progressed.
First of all Saim Rashid has created a basic SFML window opening code that displays a simple window on the screen. This was followed by adding a CMakeLists.txt file which solved the compatibility issues within VS code and Visual Studio so that all the contributors can be able to push features in the game. After that he added a player character in the form of a rectangle shape which can move up, left and right using W, A and D, all while staying within the boundary of the window. Two new functions named as Update and Draw were created to be called from the main function. 

After that Ammad-uddin created a second player in form of different coloured rectangle that can also move up left and right but with the help of Up, Left and Right arrow keys and restricting it within the boundary of the window. This was done within the Update function.

Later, Narmeen Bajwa implemented the concept of gravity for the movement of both players. This was done by adding a constant value of gravity to the vertical velocity of the player in each frame from the moment the user enters a key to move upward. This ensures that the user can see a smooth jump like a projectile in real world. The player is restricted to jump only while standing on the ground to make it more realistic.  

Then Ammad created a triangle shape that represents that which player is currently “It”. This is based on random selection at the start of the game and the triangle will move to the other player whenever the players intersect each other.

After this, Saim changed most of the structure of the game code by adding the concept of structs to the players so that repeated variables and duplicated code can be avoided. All the characteristics of the player are added under one struct. A MakePlayer function was made so that the variables in player struct can be assigned values according to the player being made in the main function. The update function was altered accordingly to maintain the same player movements. 

Later Ammad created a text file that would load a level which had platforms within it. A platform struct was made and other functions were added like LoadLevel, CreatePlatform and CheckPlatformCollision. The players were now able to land on platforms.

Then Narmeen added a timer that would display at the top and when it runs out the game ends and the player who is lost is displayed at the end. An arial font was added for the timer and end result text. A player freeze concept was also implemented so that the other player has enough time to escape before getting instantly tagged again.

After that, Ammad added the feature of collapsing walls that come closer slowly as the game continues. The Update function was altered so that the players cannot go past the walls. This increased difficulty and made the game even more interesting.
