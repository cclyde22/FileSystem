#include "RootDir.h"
using namespace std;
/*	RootDir Methods		*/

/*	The print method for an individual file, used in the list root method
	to print each node of the linked list individually	*/
void FileNode::Print() {
	cout << "Name: " << this->fileName << "\tSize: " << this->fileSize << "\tLocation: " << this->fileLoc << "\n";
}

/* RootDir Constructor	*/
RootDir::RootDir () {
	this->pHead = NULL; 
	this->count = 0;
	/*	Zero fill rootHeader	*/
	for (int i = 0; i < ROOT_HEADER_SIZE; i++)
		this->rootHeader.fileBlockLocs[i] = 0;
}

/*	Add file information to linked list representation of the root directory
	also add new file block number to hardware version of the root directory
	return false on error	*/
bool RootDir::AddFile (string name, int size, blockNum location) {
	if (FindFile(name) == -1) {
		FileNode* pNewFile = new FileNode (name, size, location);
		pNewFile->pNext = pHead;
		this->pHead = pNewFile;
		/*	Add to rootHeader (hardware version of root)	*/
		this->rootHeader.fileBlockLocs[this->count] = location;
		this->count++;
		return true;
	}
	else
		return false;
}

/*	Add file information to linked list representation of the root directory
	also add new file block number to hardware version of the root directory
	return false on error	*/
bool RootDir::AddFile (blockNum location) {
	if (FindFile(location) == -1) {
		File* pFile = new File(location, this->discID);
		pFile->Load();
		FileNode* pNewFile = new FileNode (pFile->GetName(), pFile->GetSize(), pFile->GetHeaderLoc());
		pNewFile->pNext = pHead;
		this->pHead = pNewFile;
		/*	Add to rootHeader (hardware version of root)	*/
		this->rootHeader.fileBlockLocs[this->count] = location;
		this->count++;
		return true;
	}
	else
		return false;
}

/*	Iterate through the linked list and remove the node matching file name
	Also iterate through rootHeader and remove the entry for file location	*/
bool RootDir::RemoveFile (string fileToRemove) {
	if (this->count == 0)
		return false;
	else {
		FileNode* pCurr = this->pHead;
		FileNode* pPrev = this->pHead;
		while (pCurr->fileName != fileToRemove) {
			if (pCurr->pNext == NULL)
				return false;
			else {
				pPrev = pCurr;
				pCurr = pCurr->pNext;
			}
		}
		if (pCurr == this->pHead)
			this->pHead = this->pHead->pNext;
		else
			pPrev->pNext = pCurr->pNext;
		/*	 Removed from list, now remove from rootHeader	*/
		int i;
		int value = pCurr->fileLoc;
		delete pCurr;
		for (i = 0; i < this->count; i++) 
			if (value == this->rootHeader.fileBlockLocs[i])
				break;
		if (i == this->count) {
			cout << "Error removing from root header, block location not found.\n";
			return false;
		}
		else {
			for (int j = i; j < this->count; j++)
				this->rootHeader.fileBlockLocs[j] = this->rootHeader.fileBlockLocs[j+1];
			this->count--;
			return true;
		}
	}
}
/*	Save the hardware version of the root directory to disc using writeblock function from return false on error */
bool RootDir::SaveRoot () {
	int success = writeblock(this->discID, ROOT_ADDRESS, &(this->rootHeader));
	if (success == 1)
		return true;
	else
		return false;
}
/*	Load the hardware version of the root directory into memory,
	and add each file to the linked list representation of the root directory	*/
bool RootDir::LoadRoot () {
	int success = readblock(this->discID, ROOT_ADDRESS, &(this->rootHeader));
	if (success) {
		for (int i = 0; i < ROOT_HEADER_SIZE; i++) {
			if (this->rootHeader.fileBlockLocs[i] > 2)
				this->AddFile(this->rootHeader.fileBlockLocs[i]);
		}
		bool saved = this->SaveRoot();
		if (saved == false)
			cout << "Error saving FBQ to disc\n";
	} 
}
/*	Iterate through the linked list representation of the root directory,
	printing the information from each node until the end of the list	*/
void RootDir::Print() const {
	cout << "Root Directory contains " << this->count << " entries\n";
	cout << "----------------------------------------\n";
	FileNode* pCurr = this->pHead;
	while (pCurr != NULL) {
		pCurr->Print();
		pCurr = pCurr->pNext;
	}
}

/*	Iterate through the linked list and find the file using the file name 
	as the search key. Return the block number for the file if found, Otherwise
	return -1	*/ 
blockNum RootDir::FindFile(string searchKey) const {
	if (this->count == 0)
		return -1;
	else {
		FileNode* pCurr = this->pHead;
		while ( pCurr->fileName != searchKey) {
			if (pCurr->pNext == NULL)
				return -1;
			else
				pCurr = pCurr->pNext;
		}
		return pCurr->fileLoc;
	}
}

/*	Iterate through the linked list and find the file using the file name 
	as the search key. Return the block number for the file if found, Otherwise
	return -1	*/ 
blockNum RootDir::FindFile(blockNum searchKey) const {
	if (this->count == 0)
		return -1;
	else {
		FileNode* pCurr = this->pHead;
		while (pCurr->fileLoc != searchKey) {
			if (pCurr->pNext == NULL)
				return -1;
			else
				pCurr = pCurr->pNext;
		}
		return pCurr->fileLoc;
	}
}

