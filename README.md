-To build an executable from my code, you will need to use Visual Studio. In a new Visual Studio C++ project, first make sure that you have properly installed Allegro5 for UI viewability.

-After this, ensure all of the header and .cpp files (that I have provided) are in the "Header" and "Source" folders of that same C++ project, respectively. 

-Once you are sure Allegro5 is properly installed and you have all the files in the correct place, use CTRL+SHIFT+B to build an executable file. On my laptop, this executable file appears in "/Users/crjac/source/repos/proj-part3/x64/Debug", but this might be different for you depending on where you have your Visual Studio projects saved. Essentially, you will just need to find the location of a .exe file called "proj-part3.exe".

-If you do not have the provided test files downloaded in .txt format, be sure to do so. Whatever files you download, make sure you save them to the same spot as "proj-part3.exe" (in my case, I have them saved to "/Users/crjac/source/repos/proj-part3/x64/Debug").

-This part is where we will run the code. You can either use the terminal provided in Visual Studio or open terminal manually on your laptop. In either case, navigate to the folder where you have "proj-part3.exe" saved (along with the test files) using the "cd" command.

-To finally run your code with whatever test file you want, type ".\proj-part3.exe <input-file>" into the terminal, and the UI should pop up and run the provided test file.


Working Features That I Implemented:

-The space bar can be used to put the simulation into a pause state, where nothing will change and time will not increment

-I have a timer on the right hand side of the UI window to display the current time

-I have passengers labeled with their destination floor number to make it easier to show who is going where and easy to see who still needs to be dropped off in the cabin

-I have passengers wait outside the floor they requested the elevator at, and they move into the elevator when the cabin stops at their floor

-The floor lights light up depending on what direction a person is going when they are waiting on a floor

-I have animations for the elevator moving and for the passengers moving in and out of the cabin
