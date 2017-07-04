///////////////////////////////////////////////////////////////////////
// WPF.cpp -   Implements GUI using managed code/WPF				 //
// ver 1.0                                                           //
// Language:    C++, Visual Studio 2015                              //
// Application: Dependency Based Remote Code Repository,             //
//				CSE687 - Object Oriented Design                      //
// Author:      Rahul Maddineni, Syracuse University                 //
// Source:      Jim Fawcett, Syracuse University                     //
///////////////////////////////////////////////////////////////////////

#include "WPF.h"

using namespace WPFCS;

WPFDemo::WPFDemo()
{
	// set up channel
	ObjectFactory* pObjFact = new ObjectFactory;
	pChann_ = pObjFact->createMockChannel("localhost", 8081);
	pChann_->start();
	delete pObjFact;

	// set Window properties
	this->Title = "WPF Project 4 Demo";
	this->Width = 800;
	this->Height = 650;

	// Attaching Dock Panel and Status Bar
	this->Content = hDockPanel;
	hDockPanel->Children->Add(hStatusBar);
	hDockPanel->SetDock(hStatusBar, Dock::Bottom);
	hDockPanel->Children->Add(hGrid);

	// Events
	hExtractButton->Click += gcnew RoutedEventHandler(this, &WPFDemo::extractfiles);
	hSelectFileButton->Click += gcnew RoutedEventHandler(this, &WPFDemo::browseForFile);
	hUploadButton->Click += gcnew RoutedEventHandler(this, &WPFDemo::uploadFile);
	hConfirmButton->Click += gcnew RoutedEventHandler(this, &WPFDemo::confirmDependencies);
	hSelectedButton->Click += gcnew RoutedEventHandler(this, &WPFDemo::getItemsFromList);
	hGetFilesButton->Click += gcnew RoutedEventHandler(this, &WPFDemo::getItemsFromServer);
	hGetSelectedButton->Click += gcnew RoutedEventHandler(this, &WPFDemo::getSelected);
	hGetFilesExtractButton->Click += gcnew RoutedEventHandler(this, &WPFDemo::getSelectedExtract);

	// setup Window controls and views
	setUpTabControl();
	setUpStatusBar();
	setUpCheckInView();
	setUpExtractView();
}

// Destructor
WPFDemo::~WPFDemo()
{
	delete pChann_;
}

void WPFDemo::setUpStatusBar()
{
	hStatusBar->Items->Add(hStatusBarItem);
	hStatus->Text = "Connected to Server";
	hStatusBarItem->Content = hStatus;
	hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

void WPFDemo::setUpTabControl()
{
	hGrid->Children->Add(hTabControl);
	hCheckInTab->Header = "Check In";
	hExtractTab->Header = "Extract";
	hTabControl->Items->Add(hCheckInTab);
	hTabControl->Items->Add(hExtractTab);
}

void WPFDemo::setUpCheckInView()
{
	Console::Write("\n  setting up Check In View");
	hCheckInGrid->Margin = Thickness(20);
	hCheckInTab->Content = hCheckInGrid;

	setFileSelectProperties();
	setFileDependencies();
	hListBox1->IsEnabled = false;
	hGetSelectedButton->IsEnabled = false;
	hConfirmButton->IsEnabled = false;
}

void WPFDemo::setUpExtractView()
{
	Console::Write("\n  setting up Extract View");
	hExtractGrid->Margin = Thickness(20);
	hExtractTab->Content = hExtractGrid;

	setExtractProperties();
}

void WPFDemo::setFileSelectProperties() {
	RowDefinition^ hRow6Def = gcnew RowDefinition();
	hCheckInGrid->RowDefinitions->Add(hRow6Def);
	Border^ hBorder4 = gcnew Border();
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hListBox->SelectionMode = SelectionMode::Multiple;
	hBorder4->Child = hListBox;
	hBorder4->Height = 120;
	hCheckInGrid->SetRow(hBorder4, 0);
	hCheckInGrid->Children->Add(hBorder4);
	RowDefinition^ hRow7Def = gcnew RowDefinition();           // Set Buttons: Select File, Get Selected and Upload
	hCheckInGrid->RowDefinitions->Add(hRow7Def);
	hSelectFileButton->Content = "Browse File";
	Border^ hBorder5 = gcnew Border();
	hBorder5->Width = 120; hBorder5->Height = 30;
	hBorder5->BorderThickness = Thickness(1);
	hBorder5->BorderBrush = Brushes::Black;
	hUploadButton->Content = "Upload";
	hUploadButton->IsEnabled = false;
	hBorder5->Child = hSelectFileButton;
	Border^ hBorder6 = gcnew Border();
	hBorder6->Width = 120; hBorder6->Height = 30;
	hBorder6->BorderThickness = Thickness(1);
	hBorder6->BorderBrush = Brushes::Black;
	hBorder6->Child = hUploadButton;
	hStackPanel3->Children->Add(hBorder5);
	hSelectedButton->Content = "Get Selected";
	Border^ hBorder9 = gcnew Border();
	hBorder9->Width = 120;  hBorder9->Height = 30;
	hBorder9->BorderThickness = Thickness(1);
	hBorder9->BorderBrush = Brushes::Black;
	hBorder9->Child = hSelectedButton;
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 20;
	hStackPanel3->Children->Add(hSpacer);
	hStackPanel3->Children->Add(hBorder9);
	TextBlock^ hSpacer3 = gcnew TextBlock();
	hSpacer3->Width = 20;
	hStackPanel3->Children->Add(hSpacer3);
	hStackPanel3->Children->Add(hBorder6);
	hStackPanel3->Orientation = Orientation::Horizontal;
	hStackPanel3->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hCheckInGrid->SetRow(hStackPanel3, 1);
	hCheckInGrid->Children->Add(hStackPanel3);
	hFolderBrowserDialog->ShowNewFolderButton = false;
	hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}

void WPFDemo::setFileDependencies()
{
	// List Box for getting files and adding dependencies
	RowDefinition^ hRow10Def = gcnew RowDefinition();
	hCheckInGrid->RowDefinitions->Add(hRow10Def);
	Border^ hBorder7 = gcnew Border();
	hBorder7->BorderThickness = Thickness(1);
	hBorder7->BorderBrush = Brushes::Black;
	hListBox1->SelectionMode = SelectionMode::Multiple;
	hBorder7->Child = hListBox1;
	hBorder7->Height = 120;
	hCheckInGrid->SetRow(hBorder7, 2);
	hCheckInGrid->Children->Add(hBorder7);

	// Set Buttons: Get Files, Get Selected and Confirm Dependencies
	RowDefinition^ hRow12Def = gcnew RowDefinition();
	hCheckInGrid->RowDefinitions->Add(hRow12Def);
	hGetSelectedButton->Content = "Get Selected";
	Border^ hBorder11 = gcnew Border();
	hBorder11->Width = 120;
	hBorder11->Height = 30;
	hBorder11->BorderThickness = Thickness(1);
	hBorder11->BorderBrush = Brushes::Black;
	hBorder11->Child = hGetSelectedButton;

	hConfirmButton->Content = "Confirm Dependencies";
	Border^ hBorder12 = gcnew Border();
	hBorder12->Width = 140;
	hBorder12->Height = 30;
	hBorder12->BorderThickness = Thickness(1);
	hBorder12->BorderBrush = Brushes::Black;
	hBorder12->Child = hConfirmButton;

	hStackPanel4->Children->Add(hBorder11);
	TextBlock^ hSpacer5 = gcnew TextBlock();
	hSpacer5->Width = 20;
	hStackPanel4->Children->Add(hSpacer5);
	hStackPanel4->Children->Add(hBorder12);
	hStackPanel4->Orientation = Orientation::Horizontal;
	hStackPanel4->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hCheckInGrid->SetRow(hStackPanel4, 3);
	hCheckInGrid->Children->Add(hStackPanel4);
}

void WPFDemo::setExtractProperties() {
	RowDefinition^ hRow13Def = gcnew RowDefinition();         // List Box for Files
	hExtractGrid->RowDefinitions->Add(hRow13Def);
	Border^ hBorder13 = gcnew Border();
	hBorder13->BorderThickness = Thickness(1);
	hBorder13->BorderBrush = Brushes::Black;
	hListBox2->SelectionMode = SelectionMode::Multiple;
	hBorder13->Child = hListBox2;
	hBorder13->Height = 160;
	hExtractGrid->SetRow(hBorder13, 0);
	hExtractGrid->Children->Add(hBorder13);
	RowDefinition^ hRow4Def = gcnew RowDefinition();         // List Box for Files
	hExtractGrid->RowDefinitions->Add(hRow4Def);
	hCheckBox1->IsChecked = false;
	hStackPanel6->Children->Add(hCheckBox1);
	hTextBlock5->Padding = Thickness(15);
	hTextBlock5->Text = "With Dependencies";
	hTextBlock5->FontWeight = FontWeights::Medium;
	hTextBlock5->FontSize = 16; hTextBlock5->Height = 250;
	hStackPanel6->Children->Add(hTextBlock5);
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 30;
	hStackPanel6->Children->Add(hSpacer2);
	hStackPanel6->Orientation = Orientation::Horizontal;
	hStackPanel6->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hExtractGrid->SetRow(hStackPanel6, 1);
	hExtractGrid->Children->Add(hStackPanel6);
	hGetFilesExtractButton->Content = "Get Files";
	Border^ hBorder14 = gcnew Border();
	hBorder14->Width = 120; hBorder14->Height = 30;
	hBorder14->BorderThickness = Thickness(1);
	hBorder14->BorderBrush = Brushes::Black;
	hBorder14->Child = hGetFilesExtractButton;
	hStackPanel5->Children->Add(hBorder14);
	hExtractButton->Content = "Extract";
	Border^ hBorder15 = gcnew Border();
	hBorder15->Width = 140; hBorder15->Height = 30;
	hBorder15->BorderThickness = Thickness(1);
	hBorder15->BorderBrush = Brushes::Black;
	hBorder15->Child = hExtractButton;
	TextBlock^ hSpacer6 = gcnew TextBlock();
	hSpacer6->Width = 20;
	hStackPanel5->Children->Add(hSpacer6);
	hStackPanel5->Children->Add(hBorder15);
	hStackPanel5->Orientation = Orientation::Horizontal;
	hStackPanel5->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hExtractGrid->SetRow(hStackPanel5, 2);
	hExtractGrid->Children->Add(hStackPanel5);
}

// Extract Button Status Bar Update
void WPFDemo::extractfiles(Object^ obj, RoutedEventArgs^ args)
{
	Console::Write("\n  File Extract Request");
	if (hCheckBox1->IsChecked)
		hStatus->Text = "Extracting with Dependencies";
	else
		hStatus->Text = "Extracting without Dependencies";
	int count = hListBox2->SelectedItems->Count;
	StringBuilder^ sbuild = gcnew StringBuilder();

	if (count > 0)
	{
		for each (String^ item in hListBox2->SelectedItems)
		{
			sbuild->Append(item);
			sbuild->Append(",");
		}
	}
	if (hCheckBox1->IsChecked)
		sbuild->Append("|DEPENDENCY");

	pChann_->postMessage("EXTRACT", toStdString(sbuild->ToString()));

}

// Select File Button Browse System Folder
void WPFDemo::browseForFile(Object^ sender, RoutedEventArgs^ args)
{
	std::cout << "\n  Browsing for file";
	hListBox->Items->Clear();
	System::Windows::Forms::DialogResult result;
	result = hFolderBrowserDialog->ShowDialog();
	//result = hOpenFileDialog->ShowDialog();
	if (result == System::Windows::Forms::DialogResult::OK)
	{
		String^ path = hFolderBrowserDialog->SelectedPath;
		//std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
		array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
		if (files->Length > 0)
			hUploadButton->IsEnabled = true;
		for (int i = 0; i < files->Length; ++i)
			hListBox->Items->Add(files[i]);
		array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
		for (int i = 0; i < dirs->Length; ++i)
			hListBox->Items->Add(L"<> " + dirs[i]);
	}
}

// Get Selected Files Button
void WPFDemo::getItemsFromList(Object^ sender, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hListBox->SelectedItems->Count;
	hStatus->Text = "Updated Items";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hListBox->SelectedItems)
		{
			items[index++] = item;
		}
	}

	hListBox->Items->Clear();
	if (count > 0) {
		for each (String^ item in items)
		{
			hListBox->Items->Add(item);
		}
	}
}

// Upload button Status Update
void WPFDemo::uploadFile(Object^ sender, RoutedEventArgs^ args)
{
	int count = hListBox->SelectedItems->Count;
	StringBuilder^ sbuild = gcnew StringBuilder();

	if (count > 0)
	{
		hStatus->Text = "Uploading Files";
		for each (String^ item in hListBox->SelectedItems)
		{
			sbuild->Append(item);
			sbuild->Append(",");
		}
		/*sbuild->Remove((sbuild->Length - 1), 1);*/
	}
	uploadFiles = sbuild->ToString();

	hListBox->IsEnabled = false;
	hUploadButton->IsEnabled = false;
	hSelectFileButton->IsEnabled = false;
	hSelectedButton->IsEnabled = false;
	hListBox1->IsEnabled = true;
	hGetSelectedButton->IsEnabled = true;
	hConfirmButton->IsEnabled = true;

	pChann_->postMessage("GETFILES", "");
	std::string files = pChann_->getMessage();
	if (files != "empty")
	{
		int pos;
		while (pos != std::string::npos)
		{
			pos = files.find(',');
			std::string temp = files.substr(0, pos);
			String^ vFiles = toSystemString(temp);
			hListBox1->Items->Add(vFiles);
			files.erase(0, pos + 1);
		}
	}
}

// Confirm button Status Update
void WPFDemo::confirmDependencies(Object^ sender, RoutedEventArgs^ args)
{
	int count = hListBox1->SelectedItems->Count;
	StringBuilder^ sbuild = gcnew StringBuilder();
	if (count > 0)
	{
		for each (String^ item in hListBox1->SelectedItems)
		{
			sbuild->Append(item);
			sbuild->Append(",");
		}
	}
	else
		sbuild->Append("empty");

	pChann_->postMessage("CHECKIN", toStdString(uploadFiles + "|" + sbuild->ToString()));

	int count1 = hListBox->SelectedItems->Count;
	hStatus->Text = "Uploaded "+count1+" files to server";
	Console::Write("\n  Uploading File");
	Console::Write("\n  Dependencies Updated");

	hListBox->IsEnabled = true;
	hUploadButton->IsEnabled = true;
	hSelectFileButton->IsEnabled = true;
	hSelectedButton->IsEnabled = true;

	hListBox1->Items->Clear();
	hGetSelectedButton->IsEnabled = false;
	hConfirmButton->IsEnabled = false;

	hListBox2->Items->Clear();
	pChann_->postMessage("GETFILES", "");
	std::string files = pChann_->getMessage();
	int pos;
	while (pos != std::string::npos)
	{
		pos = files.find(',');
		std::string temp = files.substr(0, pos);
		String^ vFiles = toSystemString(temp);
		hListBox2->Items->Add(vFiles);
		files.erase(0, pos + 1);
	}
}

// Get Files button 
void WPFDemo::getItemsFromServer(Object^ sender, RoutedEventArgs^ args)
{
}

// Get Selected button
void WPFDemo::getSelected(Object^ sender, RoutedEventArgs^ args)
{
	int index = 0;
	int count = hListBox1->SelectedItems->Count;
	hStatus->Text = "Updated Items";
	array<System::String^>^ items = gcnew array<String^>(count);
	if (count > 0) {
		for each (String^ item in hListBox1->SelectedItems)
		{
			items[index++] = item;
		}
	}

	hListBox1->Items->Clear();
	if (count > 0) {
		for each (String^ item in items)
		{
			hListBox1->Items->Add(item);
		}
	}
}

// Get Selected button in Extract
void WPFDemo::getSelectedExtract(Object^ sender, RoutedEventArgs^ args)
{
	hListBox2->Items->Clear();
	pChann_->postMessage("GETFILES", "");
	std::string files = pChann_->getMessage();
	int pos = 0;
	while (pos != std::string::npos)
	{
		pos = files.find(',');
		std::string temp = files.substr(0, pos);
		String^ vFiles = toSystemString(temp);
		hListBox2->Items->Add(vFiles);
		files.erase(0, pos + 1);
	}
	hStatus->Text = "Received Files from Server";
}

// Get Files from Server
void WPFDemo::getMessage()
{
	// recvThread runs this function

	while (true)
	{
		std::cout << "\n  receive thread calling getMessage()";
		array<String^>^ args = gcnew array<String^>(100);
		while (true)
		{
			int i = 0;
			std::string msg = pChann_->getMessage();
			int pos;
			while ((pos = msg.find(',')) != std::string::npos)
			{
				std::string mSg = ((msg.substr(0, pos)));
				String^ sMsg = toSystemString(msg);
				args[i] = sMsg;
				i++;
				msg.erase(0, pos + 1);
				hListBox1->Items->Add(toSystemString(msg));
			}
		}
	}
}

// Converts System^ string to std::string
std::string WPFDemo::toStdString(String^ pStr)
{
	std::string dst;
	for (int i = 0; i < pStr->Length; ++i)
		dst += (char)pStr[i];
	return dst;
}

// Converts std::string to System^ string
String^ WPFDemo::toSystemString(std::string& str)
{
	StringBuilder^ pStr = gcnew StringBuilder();
	for (size_t i = 0; i < str.size(); ++i)
		pStr->Append((Char)str[i]);
	return pStr->ToString();
}

[STAThread]
int main(array<System::String^>^ args)
{
	Console::WriteLine(L"\n Starting WPFDemo");
	Application^ app = gcnew Application();
	app->Run(gcnew WPFDemo());
	Console::WriteLine(L"\n\n");
}
