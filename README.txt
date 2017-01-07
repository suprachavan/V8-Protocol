• Program name
	Project_Part1.cpp

• Your Name (authors)
	Supra P Chavan
	
• Class number & Section 
	CPSC 558 : Advanced Computer Networks Section : 01

• Project ID
	Project Part 1

• Introduction
	Implement the F-8 protocol over a small network to exchange message in F8 protocol frame. The host and link implementation of the Simple Data Link lab assignment is used. 

• External Requirements 
	Requirement to have visual studio community edition 2015 installed on Windows Operating System

• Build, Installation, and Setup
	If not installed already, install visual studio community edition 2015 from microsoft official site.
	Either create new c++ console application project- by going to file->new->project-select c++ in language tab->win32 console application.
	The source code can be found in the folder- 558-P1-CNS->Project_Part1, in the file Project_Part1 which is a c++ source file
	The existing code from this file can be copied in the newly created solution and run. 
	OR Open the solution in Visual Studio (if already installed) . 
	To build the solution, go to build tab on the top menu bar, click on it, and select "build the solution" from drop down list. (or shortcut: ctrl+shift+b)
	To run the program, go to debug tab on the top menu bar, click on it, and select "start without debugging" the console output screen will appear.

Note: attached in the folder- visual studio solution folder "Project_Part1"
	Project_Part1.cpp - visual c++ source code file.

	output screenshot -3 screenshots
	1st screenshot->entire 57 byte host message sent in a frame
	2nd screenshot->sending of fragmented frames of 30 bytes each
	3rd screenshot->sending of fragmented frames in reverse order

	sample1.txt->entire 57 byte host message sent in a frame
	sample2.txt->sending of fragmented frames of 30 bytes each
	sample3.txt->sending of fragmented frames in reverse order

	README.txt

• Usage 
	As mentioned in the introduction, this program implements the F-8 protocol over a tiny network and achieves exchange of a text message wrapped in an F8-frame.
	step 1. Implementation of star topology with nodes A, B, C, D and links L1, L2, L3 by calling attach() functions
	step 2. Construction of host map for each node
	step 3. construction of F8 frame
	step 4. Calculating header checksum and adding that to F8 frame
	step 5. Conversion of string message into bytes, adding it to F8 frame.
	step 6. Calculating message checksum and adding it to F8 frame
	step 7. send frame using snd_f8() function of node, which calls link's xmt() which in turn calls rcv() function of destination node.
	step 8. check if frame is F8 in rcv() and if yes call rcv_f8() function of destination node
	step 9.	In rcv_f8(), extract msg from frame and display, also check whether its destination node or not, if not, forward the message using node's host map.	
	

• Extra Features
	Implements a snd_f8r() function which duplicates snd_f8() and sends fragments in reverse.
	each frame is of 30 bytes.
	For fragmentation, DLEN field of F8 frame is set to 13 bytes. 
	F-ident field of F8 frame is set to >32 (as mentioned in the pdf) and 
	seq numbers are assigned to each frame in ascending order begining from 32. i.e. 32, 33, 34, 35, 36.
	In rcv_f8() function the fragments are re-assembled using F_seq field and message is extracted in order.

• Bugs
	The code may result in some errors if run in a different environment e.g. linux.
	Try these remedies- Comment the first line- #include "stdafx.h" (as this is a windows specific header file.)
	Other errors may ocuur depending on c++ standard in use. (e.g. "for each" statement not supported)
	Note: these errors are NOT observed in the visual studio environment and the program runs smoothly to produce the desired output.

