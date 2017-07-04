#ifndef WPF_H
#define WPF_H

///////////////////////////////////////////////////////////////////////
// WPF.h -   Implements GUI using managed code/WPF					 //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Dependency Based Remote Code Repository,             //
//				CSE687 - Object Oriented Design                      //
// Author:      Rahul Maddineni, Syracuse University                 //
// Source:      Jim Fawcett, Syracuse University                     //
///////////////////////////////////////////////////////////////////////

/*
Package Operations:
===================
* This package implement CLR to provide user with a GUI to communicate
* between client and server

Build Process:
==============
Required Files:
*	WPF.cpp, MockChannel.h, MockChannel.cpp

Build commands:
===============
- devenv project4.sln

Maintenance History:
====================
ver 1.0 : 03 May 16
- first release

*/

using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;

#include "../MockChannel/MockChannel.h"
#include <iostream>
#include <string>


namespace WPFCS
{
	ref class WPFDemo : Window
	{
		// MockChannel references

		IMockChannel* pChann_;

		// Main Window Controls

		DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
		Grid^ hGrid = gcnew Grid();
		TabControl^ hTabControl = gcnew TabControl();
		TabItem^ hCheckInTab = gcnew TabItem();         // CheckIn Tab
		TabItem^ hExtractTab = gcnew TabItem();         // Extract Tab
		StatusBar^ hStatusBar = gcnew StatusBar();
		StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
		TextBlock^ hStatus = gcnew TextBlock();

		// Controls for CheckIn Tab View

		Grid^ hCheckInGrid = gcnew Grid();
		ComboBox^ hCheckInFile = gcnew ComboBox();
		TextBlock^ hTextBlock2 = gcnew TextBlock();
		Button^ hSelectFileButton = gcnew Button();
		Button^ hSelectedButton = gcnew Button();
		Button^ hUploadButton = gcnew Button();
		Button^ hGetFilesButton = gcnew Button();
		Button^ hGetSelectedButton = gcnew Button();
		Button^ hConfirmButton = gcnew Button();
		TextBlock^ hTextBlock3 = gcnew TextBlock();
		StackPanel^ hStackPanel2 = gcnew StackPanel();
		StackPanel^ hStackPanel3 = gcnew StackPanel();
		StackPanel^ hStackPanel4 = gcnew StackPanel();
		Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
		Forms::OpenFileDialog^ hOpenFileDialog = gcnew Forms::OpenFileDialog();
		ListBox^ hListBox = gcnew ListBox();
		ListBox^ hListBox1 = gcnew ListBox();

		// Controls for Extract Tab View

		Grid^ hExtractGrid = gcnew Grid();
		ComboBox^ hExtractFile = gcnew ComboBox();
		CheckBox^ hCheckBox1 = gcnew CheckBox();
		Button^ hExtractButton = gcnew Button();
		Button^ hGetFilesExtractButton = gcnew Button();
		StackPanel^ hStackPanel5 = gcnew StackPanel();
		StackPanel^ hStackPanel6 = gcnew StackPanel();
		TextBlock^ hTextBlock4 = gcnew TextBlock();
		TextBlock^ hTextBlock5 = gcnew TextBlock();
		TextBox^ hTextBox1 = gcnew TextBox();
		ListBox^ hListBox2 = gcnew ListBox();

	public:
		WPFDemo();
		~WPFDemo();

		void setUpStatusBar();
		void setUpTabControl();
		void setUpCheckInView();
		void setUpExtractView();

		void extractfiles(Object^ obj, RoutedEventArgs^ args);
		void browseForFile(Object^ sender, RoutedEventArgs^ args);
		void uploadFile(Object^ sender, RoutedEventArgs^ args);
		void confirmDependencies(Object^ sender, RoutedEventArgs^ args);
		void getItemsFromList(Object^ sender, RoutedEventArgs^ args);
		void getItemsFromServer(Object^ sender, RoutedEventArgs^ args);
		void getSelected(Object^ sender, RoutedEventArgs^ args);
		void getSelectedExtract(Object^ sender, RoutedEventArgs^ args);
		void getMessage();

	private:

		void setFileSelectProperties();        // CheckIn Tab
		void setFileDependencies();            // CheckIn Tab
		void setExtractProperties();           // Extract Tab

		std::string toStdString(String^ pStr);       // Converts String^ -> std::string
		String^ toSystemString(std::string& str);    // Converts std::string -> String^
		String^ uploadFiles;
	};
}
#endif