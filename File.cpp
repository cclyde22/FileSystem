#include "File.h"
using namespace std;

/*	DataNode Methods	*/

/*	Print the data in the data node. 
	Used to print the whole file one data node at a time	*/
void DataNode::Print() {
	for (int i = 0; i < sizeof(DataBlock); i++)
		cout << this->db.data[i];
}

/*	File Methods	*/

/*	Constructor - set pHead to null, call setHeaderLoc with headerLocation as the input	and setDiscID with discIDNum as the input	*/
File::File (blockNum headerLocation, int discIDNum) {
	this->pHead = NULL;
	this->pCurrBlock = NULL;
	this->setHeaderLoc(headerLocation);
	this->setDiscID(discIDNum);
	this->headerBlock.size = 0;
	this->headerBlock.blocksUsed = 0;
	for (int i = 0; i < FILE_DB_ARR_SIZE; i++)
		this->headerBlock.dataBlocks[i] = 0;
	if ((this->Save() == false))
		cout << "Error saving file to disc\n";
}

/*	Set the block number for the next data block in the file.
	Used to extend a file, allows FileSys methods to pass free blocks to the file
	one block at a time to avoid messy pointer arithmetic	*/
void File::SetNextDataBlockNum (blockNum newDBNum) {
	DataNode* pNewNode = new DataNode (newDBNum);
	if (this->pHead == NULL)
		this->pHead = pNewNode;
	else
		this->pHead->pNext = pNewNode;
	this->pCurrBlock = this->pHead;
	this->headerBlock.dataBlocks[this->headerBlock.blocksUsed] = newDBNum;
	this->headerBlock.blocksUsed++;
	writeblock(this->discID, this->headerLoc, &(this->headerBlock));
}

/* 	Set the name of the file, convert a string input to the 
	c-string that needs to be saved to disc and truncate to the right length if necessary */
void File::SetName(string newName)
{	if (newName.length() > 31)
		newName = newName.substr(0,31);
	strcpy(this->headerBlock.name, newName.c_str());}
	
/*	Iterate through data block list printing the contents of each data block in turn	*/
void File::Print () const {
	DataNode* pCurr = this->pHead;
	while (pCurr != NULL) {
		pCurr->Print();
		pCurr = pCurr->pNext;
	}
}

/*	Write the input string to the current data block in memory.
	Calling function handles error handling (Truncating string to 512 bytes).	*/
bool File::WriteToCurrBlock (string data) {
	for (int i = 0; i < sizeof(DataBlock); i++)
		this->pCurrBlock->db.data[i] = data[i];
	writeblock (this->discID, this->pCurrBlock->dataLoc, &(this->pCurrBlock->db));
	this->headerBlock.size = this->headerBlock.size += sizeof(DataBlock);
	writeblock (this->discID, this->headerLoc, &(this->headerBlock));
}

/*	Load the next data block for the file into memory	*/
bool File::LoadNextBlock () {
	if (pCurrBlock->pNext != NULL)
		pCurrBlock = pCurrBlock->pNext;
	else
		cout << "Error loading next block, please assign new block number before loading next block\n";
}

/*	Iterate through data block list saving each to disc in turn.
	Add each block number to FileHeader data block list and save FileHeader to disc	*/
bool File::Save () {
	DataNode* pCurr = this->pHead;
	while (pCurr != NULL) {
		writeblock (this->discID, pCurr->dataLoc, &(pCurr->db));
		pCurr = pCurr->pNext;
	}
	writeblock (this->discID, this->headerLoc, &(this->headerBlock));
}

bool File::Load () {
	readblock (this->discID, this->headerLoc, &(this->headerBlock));
	for (int i = 0; i < this->headerBlock.blocksUsed; i++) {
		DataNode* pCurr = new DataNode (this->headerBlock.dataBlocks[i]);
		readblock (this->discID, pCurr->dataLoc, &(pCurr->db));
	}
}
