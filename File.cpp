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
	this->pCurrBlock = NULL;
	this->setHeaderLoc(headerLocation);
	this->setDiscID(discIDNum);
}

/*	Set the block number for the next data block in the file.
	Used to extend a file, allows FileSys methods to pass free blocks to the file
	one block at a time to avoid messy pointer arithmetic	*/
void File::SetNextDataBlockNum (blockNum newDBNum) {
	if (this->pCurrBlock == NULL) {
		DataNode* pTemp = new DataNode (newDBNum);
		this->pCurrBlock = pTemp;
	}
	this->headerBlock.dataBlocks[this->blocksInFile] = newDBNum;
	this->blocksInFile++;
	writeblock(this->discID, this->headerLoc, &(this->headerBlock));
}

/* 	Set the name of the file, convert a string input to the 
	c-string that needs to be saved to disc and truncate to the right length if necessary */
void File::SetName(string newName)
{	if (newName.length() > 31)
		newName = newName.substr(0,31);
	strcpy(this->headerBlock.name, newName.c_str());}
	

/*	Write the input Data Block to the current data block in memory.
	Calling function handles error handling (Truncating string to 512 bytes).	*/
bool File::WriteToCurrBlock (string data) {
	for (int i = 0; i < sizeof(DataBlock); i++)
		this->pCurrBlock->db.data[i] = data[i];
	writeblock (this->discID, this->pCurrBlock->dataLoc, &(this->pCurrBlock->db));
}

/*	Load the next data block for the file into memory	*/
bool File::LoadNextBlock () {
	if ( (writeblock(this->discID, this->pCurrBlock->dataLoc, &(this->pCurrBlock->db))) == 0)
		return false;
	if (this->headerBlock.dataBlocks[++currBlockPos] != 0) {
		if ( (readblock (this->discID, this->headerBlock.dataBlocks[currBlockPos], &(this->pCurrBlock->db)) ) == 0)
			return false;
		this->pCurrBlock->dataLoc = this->headerBlock.dataBlocks[currBlockPos];
	}
	else 
		return false;
}

bool File::LoadFirstBlock () {
	if (this->pCurrBlock != NULL) {
		if ( (writeblock(this->discID, this->pCurrBlock->dataLoc, &(this->pCurrBlock->db))) == 0)
			return false;
	}
	else {
		DataNode* pTemp = new DataNode (0);
		this->pCurrBlock = pTemp;
	}
	if (this->headerBlock.dataBlocks[0] != 0) {
		if ( (readblock (this->discID, this->headerBlock.dataBlocks[0], &(this->pCurrBlock->db)) ) == 0)
			return false;
		this->pCurrBlock->dataLoc = this->headerBlock.dataBlocks[0];
	}
	else
		return false;
}

/*	*/
bool File::Save () {
	if ( (writeblock (this->discID, this->headerLoc, &(this->headerBlock))) == 0)
		return false;
	else
		return true;
}

bool File::Load () {
	if ( (readblock (this->discID, this->headerLoc, &(this->headerBlock))) == 0)
		return false;
	else
		return true;
}

void File::Print() {
	(this->pCurrBlock)->Print();
	while ( (this->LoadNextBlock()) != false )
		(this->pCurrBlock)->Print();
}
