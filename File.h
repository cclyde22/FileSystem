//include guard
#ifndef _FILE_H_INCLUDED_
#define _FILE_H_INCLUDED_
#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <string>
#include <cstring>
#include <cmath>
#include "fakedisc.h"
using namespace std;

//declared for clarity
typedef int blockNum; 

//used to make things easier to keep track of
const blockNum SB_ADDRESS = 0;		//block number for persistant SuperBlock
const blockNum FBQ_ADDRESS = 1; 	//block number of persistant FBQ
const blockNum ROOT_ADDRESS = 2; 	//block number of persistant rootDir

const int FILE_DB_ARR_SIZE = 118;

/*	Hardware structures	*/

struct DataBlock 
{ byte data[512]; };

/* Full block of block numbers for data, used for files larger than 119 blocks	*/
struct LocationBlock 
{ blockNum locations[128]; };

struct FileHeader {
	char name[32];
	/* Size in bytes */
	int size;
	int blocksUsed; //aka block count (helps manage the dataBlock array)
	/*	Array of locations of blocks for data (-1 filled) */
	blockNum dataBlocks[118];
};

/*	Software Objects (for when file is in memory)	*/

class DataNode {
	public:
		DataBlock db;
		blockNum dataLoc;
		DataNode *pNext;
		/*	Constructor	*/
		DataNode (blockNum location) : dataLoc(location), pNext(NULL) 
			{ for (int i = 0; i < sizeof(DataBlock); i++)
				this->db.data[i] = 0;}
		void Print();
};
	

class File {
private:
	int discID;
	blockNum headerLoc;
	FileHeader headerBlock;
	DataNode* pCurrBlock;
	DataNode* pHead;
public:
	File (blockNum headerLocation, int discIDNum);
	~File () {}
	string GetName() const {return this->headerBlock.name;}
	/*	Used to delete the file, calling function iterates through list deleting each in turn	*/
	DataNode* GetFirstDataNode () const {return this->pHead;}	
	void setDiscID (int idNum) {this->discID = idNum;}
	void setHeaderLoc (blockNum location) {this->headerLoc = location;}
	blockNum GetHeaderLoc () {return this->headerLoc;}
	int GetBlocksUsed () {return this->headerBlock.blocksUsed;}	
	void SetNextDataBlockNum (blockNum newDBNum);
	void SetName (string newName);
	void Print () const;	
	bool WriteToCurrBlock (string data);
	bool LoadNextBlock ();
	bool Save ();
	bool Load ();
};
#endif
