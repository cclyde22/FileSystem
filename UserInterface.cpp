#include "UserInterface.h"
using namespace std;

short int UserInterface::GetChoice () {
	short int currentChoice;
	cout << "Please enter the number corresponding to your desired operation (0 to Display Menu): ";
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cin >> currentChoice;
	while (!cin || ((currentChoice > 9) || (currentChoice < 0)) ) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Invalid input. Try again: ";
		cin >> currentChoice;
	}
	return currentChoice;
}

bool UserInterface::ExecChoice (FileSys *fs)  {
	if (this->choice == Print_Menu) {
		PrintMenu();
		return true;
	}
	else if (this->choice == Create_Disc) {
		fs->CreateDisc();
		return true;
	}
	else if (this->choice == Format_Disc) {
		if (fs->GetDiscID() > 0)
			fs->ReformatDisc();
		else
			cout << "Error, disc not mounted correctly. Sorry for the inconvenience.\n";
		return true;
	}
	else if (this->choice == Mount_Disc) {
		if (fs->GetDiscID() < 0)
			fs->MountDisc();
		else
			cout << "An error has occured, disc is already mounted. Sorry for the inconvenience.\n";
		return true;
	}
	else if (this->choice == List_Root) {
		if (fs->GetDiscID() > 0)
			fs->ListRoot();
		else 
			cout << "Error, disc not mounted correctly. Sorry for the inconvenience.\n";
		return true;	
	}
	else if (this->choice == Create_File) {
		if (fs->GetDiscID() > 0)
			fs->CreateFile();
		else
			cout << "Error, disc not mounted correctly. Sorry for the inconvenience.\n";
		return true;
	}
	else if (this->choice == Read_File) {
		if (fs->GetDiscID() > 0)
			fs->PrintFile();
		else
			cout << "Error, disc not mounted correctly. Sorry for the inconvenience.\n";
		return true;
	}
	else if (this->choice == Update_File) {
		if (fs->GetDiscID() > 0)
			fs->UpdateFile();
		else
			cout << "Error, disc not mounted correctly. Sorry for the inconvenience.\n";
		return true;	
	}
	else if (this->choice == Delete_File) {
		if (fs->GetDiscID() > 0)
			fs->DeleteFile();
		else
			cout << "Error, disc not mounted correctly. Sorry for the inconvenience\n";
		return true;
	}
	else if (this->choice == Exit) {
		fs->Exit();
		return false;
	}
	else {
		cout << "Invalid choice please make another selection\n";
		return true;
	}
}

void UserInterface::PrintMenu () const {

	cout << setfill('*') << setw(80) << "\n";
	cout << "Menu:\n";
	cout << setfill('*')<<setw(80)<<"\n";
	cout << "(0): Display menu\n";
	cout << "(1): Create new emulated disc\n";
	cout << "(2): Format existing emulated disc\n";
	cout << "(3): Mount existing emulated disc\n";
	cout << "(4): List the root directory\n";
	cout << "(5): Create new file\n";
	cout << "(6): Update existing file\n";
	cout << "(7): Read existing file (Print to screen)\n";
	cout << "(8): Delete existing file from disc\n";
	cout << "(9): Exit\n";
}


void UserInterface::run (FileSys *fs) {
		PrintMenu();
		while (true) {
			this->choice = GetChoice();
			bool Continue = ExecChoice(fs);
			if (Continue == false)
				break;
		}
}



//testing size of structs to make sure that they will write correctly to disc
/*
void main () {
	cout << "Size of File Header block is: " << sizeof(FileHeader) << " bytes\n";
	cout << "Size of FBQ Header is: " << sizeof(FBQHeaderBlock) << " bytes\n";
	cout << "Size of Root Header is: " << sizeof(RootHeaderBlock) << " bytes\n";
	cout << "Size of FileSys Header block is: " << sizeof(SuperBlock) << " bytes\n";
}
*/

void main () {
	UserInterface UI;
	FileSys *FileSystem = new FileSys();
	UI.run(FileSystem);
}
