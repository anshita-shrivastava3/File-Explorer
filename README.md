Terminal based File Explorer.

To run the application:
$ g++ main.cpp
$ ./a.out

Normal mode:
Normal mode is the default mode of your application. It should have the following functionalities -
1. Display a list of directories and files in the current folder
  a. Every file in the directory should be displayed on a new line with the following
attributes for each file -
    i. File Name
    
    ii. File Size
    iii. Ownership (user and group) and Permissions
    iv. Last modified
  b. The file explorer should show entries “.” and “..” for current and parent directory
respectively
  c. The file explorer should handle scrolling using the up and down arrow keys.
  d. User should be able to navigate up and down in the file list using the corresponding up
and down arrow keys. The up and down arrow keys should also handle scrolling during
vertical overflow.
2. Open directories and files When enter key is pressed -
  a.  Directory - Clear the screen and navigate into the directory and show
the directory contents as specified in point 1
  b.  File - Open the file in vi editor
3. Traversal
  a. Go back - Left arrow key should take the user to the previously visited directory
  b. Go forward - Right arrow key should take the user to the next directory
  c. Up one level - Backspace key should take the user up one level
  d. Home – h key should take the user to the home folder

4. The application will start form home of the current user.
5. Arrow keys, backspace, 'h', 'q' have been handled for navigation.

Command mode:
The application should enter the Command button whenever “:” (colon) key is pressed. In the command
mode, the user should be able to enter different commands. All commands appear in the status bar at the
bottom.
1. Copy –
  ‘$ copy <source_file(s)> <destination_directory>’
  Move –
  ‘$ move <source_file(s)> <destination_directory>’
  Rename –
  ‘$ rename <old_filename> <new_filename>’
  a. Eg –
    ‘$ copy foo.txt bar.txt baz.mp4 ~/foobar’
    ‘$ move foo.txt bar.txt baz.mp4 ~/foobar’
    ‘$ rename foo.txt bar.txt ’
  b. Assume that the destination directory exists, and you have write permissions.
  c. Copying/Moving directories should also be implemented
  d. The file ownership and permissions should remain intact
2. Create File –
  ‘$ create_file <file_name> <destination_path>’
  Create Directory –
  ‘$ create_dir <dir_name> <destination_path>’
  a. Eg – ‘$ create_file foo.txt ~/foobar create_file foo.txt’.
  ‘$ create_dir foo ~/foobar’
3. Delete File –
  ‘$ delete_file <file_path>’
  Delete Directory –
  ‘$ delete_dir <dir_path>’
  a. On deleting directory, you must recursively delete all content present inside it.
4. Goto –
  ‘$ goto <location>’
  a. Eg – ‘$ goto <directory_path>’
5. Search –
  ‘$ search <file_name>’
  or
  ‘$ search <directory_name>’
  a. Search for a given file or folder under the current directory recursively.
  b. Output should be True or False depending on whether the file or folder exists
6. On pressing ESC key, the application should go back to Normal Mode
7. On pressing q key in normal mode, the application should close. Similarly, entering the ‘quit’
command in command mode should also close the application.
8. File paths have been handled.
9. Permission and owner/group will remain intact in vaiours operations.
10. The directory must have execute permission to be able to open it for various operations.

Assumptions:
1. The system needs to have xdg-open to be able to open file on the file explorer.
2. Resizing is not implemented.
3. Backspace works to delete chacracters in Command mode not for navigation as in Normal mode.
4. Arrow keys are not handled in Command mode.
5. Backspace work till the 2nd character. 1st character is not buffered thus cannot be deleted.
6. Command mode has space separated string thus files having space in their names will not be handled.
7. The user needs to press Esc from command mode before typing any command.
8. Most errors can be handled by error handler with some form of description.
