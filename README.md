# Custom single file FileSystem
Implementation of a single file file system using C++

## Commands
```
mkdir path - create directory
rmdir path - delete empty directory
ls path - list directory contents
cd path - change current directory
cp src dest - copy file
rm path - remove file
cat path - output file contents
write path content +append - write content to file  overwrite if exists  append if +append is present
import src dest +append - import file from outer file system to current  append if +append is present - ##not implemented
export src dest - export file from this file system to outer - ##not implemented
cls - clear console
exit - exit program
```
## Usage
Run the .exe file without parameters or with path to already existing file system
## Note
If you want to navigate to root use :rootname/path/to/file where you need to change rootname with the name of your root folder.
