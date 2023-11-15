# ESCAPE GAME

Escape game is a chaser/collector type game built with OpenGL 3.3 (core profile). It's a simple, short and basic game inspired by the widely popular game 'Temple Run'. Player will have to control a robot chased by another big robot and escape to safety. <br/>

![Screenshot (211)](https://github.com/Bokhtiar-Adil/Escape_Game/assets/103052177/25b55ea0-a580-4b2e-948d-8c98d0518a3d)

![Screenshot (220)](https://github.com/Bokhtiar-Adil/Escape_Game/assets/103052177/9bee29a0-9387-4cc4-8784-92b28022dbea)

## Installation
You need to install the following things -
1. Visual Studio
2. CMake
3. GLFW
4. GLAD
<br />
If you don't have them installed, follow the guidelines in this pdf file to install them - <br /> [glfw glad installation guidelines.pdf](https://github.com/Bokhtiar-Adil/Escape_game_v2/files/13363734/glfw.glad.installation.guidelines.pdf) <br/>

Now, copy the contents of the 'OpenGL' folder in the repository and paste and overwrite the contents in the 'opengl' folder you created while installation. After that, 
1. Open the project with visual studio (you can double click the .sln file or open it from visual studio too).
2. Just like in the pdf, update the 'Include' folder path and the 'Library' folder path
3. Add "glfw3.lib", "opengl32.lib" and "freetype.lib" in Linker -> Additional Dependency.
4. Click apply and ok.
5. Delete the glad.c file from the 'Source files' folder, then add the glad.c file from your opengl folder as demonstrated in the pdf.
<br />
You are good to go. Now, run the game. You should face no error.

## Rules
To win this game, player will have to survive for 1 minute. Player will lose if - <br/>
1. Robot doesn't move forward for too long <br/>
2. Fuel runs out <br/>

Player will have to collect the game items spreading in front of him. There are five types of game items - <br/>
1. Green item	: Boosts fuel and coin bonus twofold <br/>
2. Gold item	: Gives coin <br/>
3. Blue item	: Gives fuel <br/>
4. Red item	  : Slows down fuel and coin bonus, like a reverse boost <br/>
5. Black item	: Switches off all the lights except the torch <br/>

Fuel will automatically run out in each frame. Collecting the blue item refuels it. If the fuel gets too low, coins will be automatically spent to refuel it until it reaches zero. Boosting doubles fuel and coin bonus. <br/>

There are thre indicator bars over the robot's head. They show corresponding resource status in color gradient. Color changes from red to green if the corresponding resource increases and from green to red if the corresponding resource decreases. <br/>
1. Left gradient bar	  : Shows how much coin you have
2. Middle gradient bar	: Shows how much fuel you have
3. Right gradient bar	  : Shows remaining boost duration 


## Controls
Command prompt wiil show all the necessary directives.
1. Enter : Starts the game
2. Escape : Closes the game
3. W, S, A, D	: Forward, Backward, Left, Right (player can't move backwards during gameplay, only allowed in ghost mode)
4. Space : Jump
5. E, R : Camera up and down
6. X, Y, Z : Pitch, Yaw, Roll
7. N : Forced nightmode
8. B : Camera wide view
9. H : Camera reset
10. T : Toggle torch
11. 1, 2, 3, 4 : Robot rotates around axis x, y, z and resets respectively
12. 7 : Ghost mode, pauses game to move the camera as you want without moving the robot, H resets it









