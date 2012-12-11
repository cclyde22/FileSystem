#include "FileSys.h"
using namespace std;
/*	FileSys Methods		*/

/*	Set New disc name with input string		*/
void FileSys::SetDiscName (string newName) 
{	if (newName.length() > 31)
		newName = newName.substr(0,31);
	strcpy(this->FSHeader.discName, newName.c_str());}

/*	Read Super Block from disc and update FileSys members in memory	*/
void FileSys::GetSBInfo () {
	int success = readblock(this->discID, SB_ADDRESS, &(this->FSHeader));
	if (success == 0)
		cout << "Error reading superblock\n";
}

/*	Save Super Block to disc	*/
void FileSys::UpdateSuperBlock () {
	int success = writeblock(this->discID, SB_ADDRESS, &(this->FSHeader));
	if (success == 0)
		cout << "Error saving superblock to disc\n";
}


/*	Prompt user for disc details and create disc, then initialize for use 	*/ 
void FileSys::CreateDisc () {
	string newDiscName;
	cout << "Please enter a name for your new emulated disc (Max Length of 31 characters)\n";
	cin >> newDiscName;
	this->SetDiscName(newDiscName);
	cout << "Please enter the size (in blocks) for your new disc\n";
	cin >> this->FSHeader.totalBlocks;
	int created = createdisc(this->FSHeader.discName, this->FSHeader.totalBlocks);
	if (created) {
		int mounted = mountdisc(this->FSHeader.discName);
		if (mounted != -1) {
			this->discID = mounted;
			this->fbq.setDiscID(this->discID);
			this->fbq.Init(this->FSHeader.totalBlocks);
			bool FBQSaved = this->fbq.SaveFBQ();
			this->root.setDiscID(this->discID);
			cout << "Disc successfully created and ready for use.\n";
		}
		else
			cout << "Something went wrong, disc not mounted for use after creation\n";
	}
	else
		cout << "Something went wrong, disc not successfully created\n";
	
}

/*	Prompt user for name of disc to reformat. 
	If disc doesn't exist output error message, otherwise prompt for size and reformat disc */
void FileSys::ReformatDisc () {
	string discToReformat;
	cout << "Please enter the name of the existing disc that you would like to format\n";
	cin >> discToReformat;
	int exists = mountdisc(discToReformat.c_str());
	if (exists != -1) {
		int	mountedDiscID = exists;
		int currentSize = discsize(mountedDiscID);
		int newSize;
		closedisc(mountedDiscID);
		cout << "The current disc named " << discToReformat << " is " << currentSize << " blocks\n";
		cout << "Please enter the new disc size (in blocks)\n";
		cin >> newSize;
		int reformat = createdisc(discToReformat.c_str(), newSize);
		if (reformat == 1) {
			this->SetDiscName(discToReformat);
			this->FSHeader.totalBlocks = newSize;
			int mounted = mountdisc(this->FSHeader.discName);
			if (mounted != -1) {
				this->discID = mounted;
				this->fbq.setDiscID(this->discID);
				this->fbq.Init(this->FSHeader.totalBlocks);
				//possibly empty root as well
				this->root.setDiscID(this->discID);
				cout << "Disc successfully reformatted and ready for use.\n";
			}
			else
				cout << "Something went wrong, disc not mounted for use after reformat\n";
		}
		else
			cout << "Something went wrong, disc not successfully reformatted\n";
	}
	else
		cout << "A disc with this name doesn't exist, please use create to create a new disc\n";

}

/*	Prompt user for name of disc to mount. If disc doesn't exist output error msg
	Otherwise mount disc and initialize for use 	*/
void FileSys::MountDisc () {
	string discToMount;
	cout << "Please enter the name of the disc that you would like to mount for use\n";
	cin >> discToMount;
	int mounted = mountdisc(discToMount.c_str());
	if (mounted != -1) {
		this->discID = mounted;
		this->SetDiscName(discToMount);
		GetSBInfo();
		this->fbq.setDiscID(this->discID);
		bool FBQLoaded = this->fbq.LoadFBQ();
		if (FBQLoaded == false) {
			cout << "Error, free block queue not loaded correctly.\n";
			return;
		}
		else {
			this->root.setDiscID(this->discID);
			bool RootDirLoaded = this->root.LoadRoot();
			if (RootDirLoaded == false) {
				cout << "Error, root directory not loaded correctly.\n";
				return;
			}
			else
				cout << "Disc successfully mounted and ready for use with disc id = " << this->discID << "\n";
		}
	}
	else
		cout << "Something went wrong, disc not mounted for use after reformat\n";
	
}

/*	Read file header from disc and create file in memory, return pointer to the file	*/
File* FileSys::OpenFile (blockNum fileHeaderLoc) {
	File* openedFile = new File (fileHeaderLoc, this->discID);
	bool fileLoaded = openedFile->Load();
	if (fileLoaded == false) {
		cout << "Error loading file data from disc\n";
		return NULL;
	}
	else
		return openedFile;
}

/*	Save the file header, data blocks to disc.
	Then delete the file from memory	*/
void FileSys::CloseFile (File* FileToClose) {
	bool fileSaved = FileToClose->Save();
	if (fileSaved == false) {
		cout << "Error saving file to disc\n";
	}
	else
		delete FileToClose;
}

/*	Prompt user for file name to print, then use Find method to get block number for file. 
	Output error message if the file doesn't exist otherwise open the file and
	use the file's print method to print the contents. Close the file afterwards	*/  
void FileSys::PrintFile () {
	string fileToPrint;
	cout << "Please enter the name of the file that you want to output to the screen\n";
	cin >> fileToPrint;
	blockNum fileLoc = this->root.FindFile(fileToPrint);
	if (fileLoc != -1) {
		File* OpenedFile = this->OpenFile(fileLoc);
		if (OpenedFile == NULL) {
			cout << "Error: File not opened correctly\n";
			return;
		}
		else {
			OpenedFile->Print();
			this->CloseFile(OpenedFile);
		} 
	}
	else
		cout << "Error: File with name " << fileToPrint << " not found in root directory\n";
}

/*	Prompt user for initial file info, make sure that file with the same name doesn't already exist,
	then create new file and add to root directory. Close file afterwards and save root directory, fbq, and superblock to disc 	*/
void FileSys::CreateFile () {
	string newFileName;
	cout << "Enter a name for the new file\n";
	cin >> newFileName;
	blockNum FileExists = this->root.FindFile(newFileName);
	if (FileExists != -1)
		cout << "Error: A file with this name already exists\n";
	else {
		blockNum newFileHeaderLoc = this->fbq.GetFreeBlock();
		cout << "Got free block for file header = " << newFileHeaderLoc << "\n";
		File* newFile = new File (newFileHeaderLoc, this->discID);
		newFile->SetName(newFileName);
		string content;
		cout << "Enter the initial contents for the new file\n";
		cin >> content;
		int i, j;
		string contentArr[FILE_DB_ARR_SIZE];
		for (i = 0, j = 0; i < content.length() && j < FILE_DB_ARR_SIZE; i+=sizeof(DataBlock), j++)
			contentArr[j] = content.substr(i, (i+511));
		while (i % 512 != 0) {
			//space fill
			contentArr[j][i] = ' ';
			i++;
		}
		for (i = 0; i < j; i++) {
			blockNum freeBlock = this->fbq.GetFreeBlock();
			newFile->SetNextDataBlockNum(freeBlock);
			newFile->LoadNextBlock();
			newFile->WriteToCurrBlock(contentArr[i]);
		}
		newFile->Save();
		bool FileAdded = this->root.AddFile(newFileName, newFileHeaderLoc);
		if (FileAdded == false)
			cout << "Error adding file to root directory\n";
		else {
			bool RootSaved = this->root.SaveRoot();
			if (RootSaved == false)
				cout << "Error saving root directory to disc\n";
			else {
				bool FBQSaved = this->fbq.SaveFBQ();
				if (FBQSaved == false)
					cout << "Error saving fbq to disc\n";
				else {
					this->FSHeader.blocksFree = this->fbq.GetCount();
					this->UpdateSuperBlock();
				}
			}
		}
	}
}

/*	Prompt for name of file to update, then use find method to find the file.
	Output an error message if the file does not exist. Otherwise open the file and 
	prompt user for new contents. Resize file if necessary and update contents.
	Close the file afterwards	*/
void FileSys::UpdateFile () {
	string fileNameToUpdate;
	cout << "Enter the name of the file that you would like to update\n";
	cin >> fileNameToUpdate;
	blockNum FileExists = this->root.FindFile(fileNameToUpdate);
	if (FileExists == -1)
		cout << "Error: A file with this name does not exists\n";
	else {
		File* fileToUpdate = this->OpenFile(FileExists);
		cout << "The current contents of " << fileNameToUpdate << " are: \n";
		fileToUpdate->Print();
		string newContent;
		cout << "\nEnter the new content for this file\n";
		cin >> newContent;
		int i, j;
		string contentArr[FILE_DB_ARR_SIZE];
		for (i = 0, j = 0; i < newContent.length() && j < FILE_DB_ARR_SIZE; i+=sizeof(DataBlock), j++)
			contentArr[j] = newContent.substr(i, (i+511));
		while (i % 512 != 0) {
			contentArr[j][i] = 0;
			i++;
		}
		for (i = 0; i < j; i++) {
			if (i > fileToUpdate->GetBlocksUsed()) {
				blockNum freeBlock = this->fbq.GetFreeBlock();
				fileToUpdate->SetNextDataBlockNum(freeBlock);
			}
			fileToUpdate->WriteToCurrBlock(contentArr[i]);
			fileToUpdate->LoadNextBlock();
		}
		fileToUpdate->Save();
		bool FBQSaved = this->fbq.SaveFBQ();
		if (FBQSaved == false)
			cout << "Error saving fbq to disc\n";
		else {
			this->FSHeader.blocksFree = this->fbq.GetCount();
			this->UpdateSuperBlock();
			cout << "File successfully updated\n";
		}
	}	
}

/*	Prompt for file name to delete, then use the find method to find the file.
	Output an error message if the file does not exist. Otherwise open the file and
	free the blocks used for the contents. Then remove the file from the root directory,
	free the block used by the file and close the file 	*/
void FileSys::DeleteFile () {
	string fileNameToDelete;
	cout << "Please enter name of the file that you want to delete\n";
	cin >> fileNameToDelete;
	blockNum FileExists = this->root.FindFile(fileNameToDelete);
	if (FileExists == -1)
		cout << "Error: A file with this name does not exists\n";
	else {
		File* FileToDelete = this->OpenFile(FileExists);
		if (FileToDelete == NULL)
			cout << "Error loading file into memory\n";
		else {
			DataNode* pCurrDB = FileToDelete->GetFirstDataNode();
			while (pCurrDB != NULL) {
				this->fbq.FreeBlock(pCurrDB->dataLoc);
				pCurrDB = pCurrDB->pNext;
			}
			this->fbq.FreeBlock(FileToDelete->GetHeaderLoc());
			delete FileToDelete;
			bool fileRemoved = this->root.RemoveFile(fileNameToDelete);
			if (fileRemoved == false)
				cout << "Error removing file from root directory\n";
			else {
				bool RootSaved = this->root.SaveRoot();
				bool FBQSaved = this->fbq.SaveFBQ();
				if (RootSaved == false || FBQSaved == false)
					cout << "Error saving root or fbq to disc\n";
				else {
					this->FSHeader.blocksFree = this->fbq.GetCount();
					this->UpdateSuperBlock();
					cout << "File successfully deleted\n";
				}
			}
		}
	}
}

/*	Save superblock, root header, and fbq header to disc	*/
void FileSys::Exit () {
	this->UpdateSuperBlock();
	bool RootDirSaved = this->root.SaveRoot();
	if (RootDirSaved == false)
		cout << "Error saving root directory to disc\n";
	else {
		bool FbqSaved = this->fbq.SaveFBQ();
		if (FbqSaved == false)
			cout << "Error saving free block queue to disc\n";
		else 
			cout << "Clean exit successful. Have a nice day\n";
	}
} 	
