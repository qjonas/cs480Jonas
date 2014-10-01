A simple example of matrix use in OpenGL
========================================
This program uses assimp to load an .obj file passes as a command line argument.
It colors the object. 
It also includes a box .obj file in bin for future development.

Commands:

'Left Arrow'--> rotates counter clockwise
'Right Arrow'--> rotates clockwise

's'--> start or stop rotation
'a'--> reverse direction of rotation
'g'--> start or stop revolution
't'--> increase rotation speed
'b'--> decrease rotation speed
'esc'--> exit the program
'Left mouse click'--> switches the rotation of direction 

Menu:
1 Quit --> exit the program
2 Rotation Sub Menu:
  3--> Start rotation
  4--> Stop Rotation
  5--> Reverse Direction
  
******** Code for the hierarchy of menus was directly adapted from 
"Interactive Computer Graphics: A Top-Down Approached with Shader-Based OpenGl" 6th ed. by Edward Angel and David Shreiner

Building This Example
---------------------

*This example requires GLM*
*On ubuntu it can be installed with this command*

>$ sudo apt-get install libglm-dev

*On a Mac you can install GLM with this command(using homebrew)*
>$ brew install glm

To build this example just 

>$ cd build
>$ make

*If you are using a Mac you will need to edit the makefile in the build directory*

The excutable will be put in bin

Additional Notes For OSX Users
------------------------------

Ensure that the latest version of the Developer Tools is installed.
