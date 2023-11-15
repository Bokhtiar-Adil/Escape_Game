# ESCAPE GAME

Escape game is a chaser/collector type game built with OpenGL 3.3 (core profile). It's a simple, short and basic game inspired by the widely popular game 'Temple Run'. Player will have to control a robot chased by another big robot and escape to safety.

![image](https://github.com/Bokhtiar-Adil/Escape_game_v2/assets/103052177/045632e6-8971-4477-bb03-a4669a0dd119)

![image](https://github.com/Bokhtiar-Adil/Escape_game_v2/assets/103052177/743b09b3-0c05-47e1-a9b1-5f5fc7090c47)

## Installation
You need to install the following things -
1. Visual Studio
2. CMake
3. GLFW
4. GLAD
If you don't have them installed, follow the guidelines in this pdf file to install them - [glfw glad installation guidelines.pdf](https://github.com/Bokhtiar-Adil/Escape_game_v2/files/13363734/glfw.glad.installation.guidelines.pdf)

Now, copy the contents of the 'OpenGL' folder in the repository and paste and overwrite the contents in the 'opengl' folder you created while installation. After that, 
1. Open the project with visual studio (you can double click the .sln file or open it from visual studio too).
2. Just like in the pdf, update the 'Include' folder path and the 'Library' folder path
3. Add "glfw3.lib", "opengl32.lib" and freetype.lib" in Linker -> Additional Dependency.
4. Click apply and ok.
5. Delete the glad.c file from the 'Source files' folder, then add the glad.c file from your opengl folder as demonstrated in the pdf.
You are good to go. Now, run the game. You should face no error.

## Rules
To win this game, player will have to survive for 1 minute. Player will lose if -
1. Robot doesn't move forward for too long
2. Fuel runs out

Player will have to collect the game items spreading in front of him. There are five types of game items -
1. Green item	: Boosts fuel and coin bonus twofold
2. Gold item	: Gives coin
3. Blue item	: Gives fuel
4. Red item	  : Slows down fuel and coin bonus, like a reverse boost
5. Black item	: Switches off all the lights except the torch

Fuel will automatically run out in each frame. Collecting the blue item refuels it. If the fuel gets too low, coins will be automatically spent to refuel it until it reaches zero. Boosting doubles fuel and coin bonus.

There are thre indicator bars over the robot's head. They show corresponding resource status in color gradient. Color changes from red to green if the corresponding resource increases and from green to red if the corresponding resource decreases. 
1. Left gradient bar	  : Shows how much coin you have
2. Middle gradient bar	: Shows how much fuel you have
3. Right gradient bar	  : Shows remaining boost duration
Beware of the reds!

## Controls
Enter		    : Starts the game
Escape		  : Closes the game
W, S, A, D	: Forward, Backward, Left, Right (player can't move backwards during gameplay, only allowed in ghost mode)
Space		    : Jump
E, R		    : Camera up and down
X, Y, Z		  : Pitch, Yaw, Roll
N			      : Forced nightmode
B			      : Camera wide view
H			      : Camera reset
T			      : Toggle torch
1, 2, 3, 4	: Robot rotates around axis x, y, z and resets respectively
7           : Ghost mode, pauses game to move the camera as you want without moving the robot, H resets it
Command prompt will always show these directives during the gameplay.

![image](https://github.com/Bokhtiar-Adil/Escape_game_v2/assets/103052177/4534a459-3468-49c6-a914-85092bdc7c40)

![image](https://github.com/Bokhtiar-Adil/Escape_game_v2/assets/103052177/c65a0f4d-29c4-4ba2-9cfd-4c9a99d4da38)








