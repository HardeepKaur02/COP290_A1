NOTE: 
This implementation uses a screenshot from the video to perform the background subtraction. This background image should be present as "empty_new.png" in the same directory.
Kindly ensure that the video to be processed is also present in the same directory.

BUILD & RUN

1. Using Makefile

To compile the program, type "make" in command line.
It would create the executable file.
Then type "./cam video_name" where video_name is the name of video file WITHOUT extension. It is assumed that video has extension ".mp4".  
This would pop up 3 windows showing the dynamic density, queue density and projected grayscale view of the video.
The values of frame number, queue density and dynamic density would be printed on the console as well as written to a text file "out.txt" in the same directory.
Pressing Esc key would stop the process.

To delete the executable file "cam", type "make clean" in the terminal. 

2. Without using makefile:
 
Type the command : g++ task2.cpp -o cam -std=c++11 `pkg-config --cflags --libs opencv`
It would create the executable file. 
Then type "./cam video_name".
Same actions would follow as discussed above.

