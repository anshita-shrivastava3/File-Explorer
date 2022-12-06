# Linux Terminal Based File Explorer

### Prerequisites
1. G++ compiler
   * ```sudo apt-get install g++```

### Compile and run the application
1. go to project directory and run following command
   * ```g++ main.cpp```
   * ```./a.out```
   
### Functionality Terminal File Explorer 
File explorer is work in two modes. 
1. Normal Mode: Normal mode is the default mode of your application. It should have the following functionalities.
2. Command Mode: Swicth from Normal to Command Mode using ":"
### 1. Normal Mode: 
1.1 **Read and display list of files and directories in the current folder**
* File explorer show each file in the directory (one entry per line). The following attributes are visible for each file
    * File Name
    * File size
    * Ownership (User & Group) & Permissions
    * Last modified

* The file explorer also handles scrolling (vertical overflow) in case the directory has a lot of files.
* The file explorer also shows the entries “.” & “..” for current and parent directory respectively.
* User is able to navigate up & down the file list using corresponding arrow keys.

1.2 **Open files & directories**
* When enter is pressed
    * Directory - It will Clear the screen and Navigate into the directory and shows the files & directories inside it as specified in point 1
    * Files - It will open files using the corresponding default application.


### 2. Command Mode:
The application should enter the Command button whenever “:” (colon) key is pressed. In the command
mode, the user should be able to enter different commands. All commands appear in the status bar at the
bottom.

**2.1 copy, move and rename** 
```
copy <source_file(s)> <destination_directory>
move <source_file(s)> <destination_directory>
```

```
Eg:
copy foo.txt bar.txt baz.mp4 ~/foobar
move foo.txt bar.txt baz.mp4 ~/foobar
rename foo.txt bar.txt
```
Copying / Moving of directories is also be implemented

**2.2 create files and directories** 
```
create_file <file_name> <destination_path>
create_dir <dir_name> <destination_path>
```

```
Eg:
create_file foo.txt ~/foobar
create_file foo.txt .
create_dir folder_name ~/foobar
```

**2.3 delete files and directories** 
```
delete_file <file_path>
delete_dir <directory_path>
```

```
Eg:
delete_file ~/foobar/foo.txt.
delete_dir ~/foobar/folder_name
```

**2.4 goto** 
```
goto <directory_path>
```

```
Eg:
goto /home/user/
goto ~
```

**2.5 Search a file or folder given fullname.** 
```
search <filename>
```

```
Eg:
search foo.txt
```
Search for the given filename under the current directory recursively


**2.6 Snapshotting the filesystem and dump into a file** 
```
snapshot <folder> <dumpfile>​
```

```
Eg:
snapshot ~/foobar/ dumpimg
```
Given a base directory this command recursively crawl the directory and store the output in dumpfile.

**2.7 On pressing ‘ESC’ key the application should go to Normal Mode**
