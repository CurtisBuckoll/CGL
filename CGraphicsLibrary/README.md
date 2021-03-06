CMPT361 Assignment 3
Curtis Buckoll
301291952

This submission was built in Qt Creator. There should (hopefully) not be any problems in the code currently- I've tested everything
well and have attempted to implement everything.

This folder contains the qtqt.pro file, and you can use that to open the project in Qt Creator to view and run the code. I've 
also included an executable that was compiled in visual studio (using the same code) in the folder "a3_executable" (I used VS as 
I could not get the executable compiled in Qt to work, even when including the correct .dll's). To get this .exe to run in a windows 
environment, you must drop the following three files into "a3_executable", along with any .simp or .obj files that you wish to 
render:

Qt5Cored.dll
Qt5Guid.dll
Qt5Widgetsd.dll

This files are too large for me to upload within the same project. Alternatively, you can do this with the .exe compiled by Qt
Creator if you have that working on your end. The .exe will render a .simp file that you specify on command line in this form:

.\CMPT361.exe filename.simp

I think that's it, I know this must be difficult on your end so I'm doing my best to make this easy.


---------------------------------------------------------------------------------------------------




CMPT361 Assignment 2
Curtis Buckoll
301291952

If NOT downloading from canvas and instead running in VS2015, start by opening "CMPT361.sln".

Please note that my Y coord is flipped before drawing, so if pages 6-8 are upside-down, this is by design of the renderer!

client.cpp manages the viewed pages, and SimpWriter.cpp contains the class to read from simp files. Everything should be set up
to drop test1.simp, test2.simp, and test3.simp into the root directory overwriting the existing files.

I tried my best to implement everything in detail, so hopefully all goes well!



---------------------------------------------------------------------------------------------------



CMPT361 Assignment 1
Curtis Buckoll
301291952

I did my best to implement all of the requirements, and  so there is nothing I have not 
attempted. I included a sixth page where I implemented bi-linear interpolation of a single
static triangle in the first panel, and 5 random triangles with random colours in the second 
panel. I was pretty happy with my antialiased lines, and so I included them in third
panel of page six.

A quick break down of the file/class organisation is as follows: 
	
	Point.h contains useful vertex data structures used amongst the other files.
	Line.h contains the three line rendering algorithms, DDA, Bresenham, and Antialiased.
	Polygon.h contains the functions to create filled polygons.
	DrawableCustom.h manages coordinates for drawing to specific parts of the screen (ie. each panel).

Both Polygon and Line are static classes. Please also note that for page four, I flip my 
y coordinate right before rendering, so my y coordinates go from bottom to top with respect to 
the screen. Therefore, the diagonals in each squares go from bottom left (min(x), min(y)) to 
top right (max(x), max(y)).