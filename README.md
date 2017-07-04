# Remote Code Repository

The Remote Code Repository will:

* Support package checkin, where a module is a named small set of closely related packages, usually from the same namespace.
* Store files in folders based on package name and checkin date.
* Track dependency relationships between packages.
* Support extraction of all the files along with their dependencies

## Commands:
- compile.bat - devenv CommPrototype.sln /rebuild debug

- run.bat - 
	:run.bat
	cd Debug
	start WPF.exe
	start MsgServer.exe
	cd..

### Folders:

- Server stores files in "ServerFiles" folder.
- Extracted files from the server are stored in "ExtractedFiles" folder.

### Note:

- Only one client and a server is implemented.
- Can upload only files, won't work on packages.

## Flow :

### CheckIn Tab:

- Start by pressing "Browse File" button to browse files.

- "Get Selected" button to clear the list and only show selected files
   from the list of files

- Press "Upload" button to upload the file/files to the server.

- List of files already in the server are populated in the second list box

- Select files from the list box and press "Confirm Dependencies" button. The
  selected files from second list box will be added as dependencies to the 
  highlighted files in first list box.

- Files are stored with metadata and timestamp in "ServerFiles" Folder.


### Extract Tab:

- After uploading files in checkin tab, server files are automatically 
  displayed or press "Get Files" button to display the lastest server files.

- Select files from the listbox which you need to extract.

- "Dependencies" check box defines whether you want download the selected file/files
   with/without dependency.

- Press "Extract" button to extract the selected files to "ExtractedFiles" Folder.

## Screenshots

### StartUp
![Starting Up](https://github.com/rahulmaddineni/Remote-Code-Repository/blob/master/Screenshots/Startup.PNG)

### Upload Files with Dependencies
![Uploading](https://github.com/rahulmaddineni/Remote-Code-Repository/blob/master/Screenshots/Uplading.PNG)

### Extract Files with Dependencies
![Extracting](https://github.com/rahulmaddineni/Remote-Code-Repository/blob/master/Screenshots/Extracting.PNG)
