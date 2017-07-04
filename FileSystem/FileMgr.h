#ifndef FILEMGR_H
#define FILEMGR_H

/////////////////////////////////////////////////////////////////////
// FileMngr.h - find files matching specified patterns             //
//             on a specified path                                 //
// ver 1.0                                                         //
// Application:   Parser component, CSE687 - Object Oriented Design//
// Author:		  Teja Thunuguntla, Syracuse University		       //
// Author:      Jim Fawcett, Syracuse University, CST 4-187        //
//              jfawcett@twcny.rr.com                              //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
* Filemanager takes input through command line arguments.
* Path and multiple patterns are inputs. Returns all files
* and directories which the patterns given.

Build Process:
==============
Required files
- FileSystem.h, FileSystem.cpp, DataStore.h

Build commands:
===============
- devenv project2.sln

Maintenance History:
====================
ver 1.0 : 12 Mar 16
- first release

*/

#include "FileSystem.h"
#include "DataStore.h"
#include <iostream>

class FileMgr
{
public:
	using iterator = DataStore::iterator;
	using patterns = std::vector<std::string>;
	std::vector<std::string> vFiles;

	FileMgr(const std::string& path, DataStore& ds) : path_(path), store_(ds)
	{
		patterns_.push_back("*.*");
	}

	// < ------- add all the patterns to a vector -------- >
	void addPattern(const std::string& patt)
	{
		if (patterns_.size() == 1 && patterns_[0] == "*.*")
			patterns_.pop_back();
		patterns_.push_back(patt);
	}

	void search()
	{
		find(path_);
	}

	// < ---------- find all the files and directories in the given path ----------- >
	void find(const std::string& path)
	{
		std::string fpath = FileSystem::Path::getFullFileSpec(path);

		for (auto patt : patterns_)  
		{
			std::vector<std::string> files = FileSystem::Directory::getFiles(fpath, patt);
			for (auto f : files)
			{
				if (f.substr(f.size() - 3, f.size()) != "xml")
				{
					vFiles.push_back(fpath + "\\" + f);
					store_.save(f);
				}
				else
					continue;
			}
		}
		std::vector<std::string> dirs = FileSystem::Directory::getDirectories(fpath);
		for (auto d : dirs)
		{
			if (d == "." || d == "..")
				continue;
			std::string dpath = fpath + "\\" + d;  // here's the fix
			find(dpath);
		}
	}

	// < --------- returns a files vector ----------- >
	std::vector<std::string> getFiles() 
	{
		return vFiles; 
	}
private:
	std::string path_;
	DataStore& store_;
	patterns patterns_;
};

#endif
