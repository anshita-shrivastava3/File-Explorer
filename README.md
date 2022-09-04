Terminal based File Explorer.

To run the application:
$ g++ main.cpp
$ ./a.out

Normal mode:
1. The application will start form home of the current user.
2. Arrow keys, backspace, 'h', 'q' have been handled for navigation.

Command mode:
1. File paths have been handles.
2. Permission and owner/group will remain intact in vaiours operations.
3. The directory must have execute permission to be able to open it for various operations.

Assumptions:
1. The system needs to have xdg-open to be able to open file on the file explorer.
2. Resizing is not implemented.
3. Backspace works to delete chacracters in Command mode not for navigation as in Normal mode.
4. Arrow keys are not handled in Command mode.
5. Backspace work till the 2nd character. 1st character is not buffered thus cannot be deleted.
6. Command mode has space separated string thus files having space in their names will not be handled.
7. The user needs to press Esc from command mode before typing any command.
8. Most errors can be handled by error handler with some form of description.
