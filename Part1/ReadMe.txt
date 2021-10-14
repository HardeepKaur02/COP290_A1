NOTE

To get the projected and cropped images of an image "imagename.jpg" , it should be present in the same directory. 

The projected and cropped images are saved in the same directory by the name "imagename_projected.jpg" and "imagename_cropped.jpg" respectively. 

SELECTION OF POINTS

Points are to be selected manually, using right click of mouse button.
Points should be selected in anticlockwise fashion,starting from the top left corner.
Only 4 points should be clicked on.
Clicking on more or less number of points would lead to termination.

BUILD & RUN

1. Using Makefile

To compile the program, type "make" on command line.
It would create the executable file.
Then type "./cam imagename" (excluding quotes). 
This would pop up a window for the manual selection of 4 points on the input image, using right click of mouse button. 
Click on the points in counterclockwise direction starting from top left corner.
Then pressing any key would show the projected image on user's window.
Press another key to see the cropped image. 
Then exit by again pressing any key.

Images would be saved in the same directory.

To delete the executable file "cam", type "make clean". 

2. Without using makefile:
 
Type the command : g++ main.cpp -o testout -std=c++11 `pkg-config --cflags --libs opencv`
It would create the executable file. 
Then type "./cam imagename" where imagename is empty/traffic .
This would ask for selection of points in the similar way as discussed above.

Finally, the projected and cropped images would be saved as imagename_projected.jpg and imagename_cropped.jpg in the same directory.
