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

const int FILE_DB_ARR_SIZE = 119;

/*	Hardware structures	*/

struct DataBlock 
{ byte data[512]; };

/* Full block of block numbers for data, used for files larger than 119 blocks	*/
struct LocationBlock 
{ blockNum locations[128]; };

struct FileHeader {
	char name[32];
	int size;
	blockNum dataBlocks[119];
};

/*	Software Objects (for when file is in memory)	*/

class DataNode {
	public:
		DataBlock db;
		blockNum dataLoc;
		/*	Constructor	*/
		DataNode (blockNum location) : dataLoc(location) 
			{ for (int i = 0; i < sizeof(DataBlock); i++)
				this->db.data[i] = ' ';}
		void Print();
};
	

class File {
private:
	int discID;
	int blocksInFile;
	int currBlockPos;
	blockNum headerLoc;
	FileHeader headerBlock;
	DataNode* pCurrBlock;
public:
	File (blockNum headerLocation, int discIDNum);
	~File () {}
	string GetName() const {return this->headerBlock.name;}
	int GetSize () const {return this->headerBlock.size;}
	void setDiscID (int idNum) {this->discID = idNum;}
	void setHeaderLoc (blockNum location) {this->headerLoc = location;}
	void setSize (int newSize) {this->headerBlock.size = newSize;}
	blockNum GetHeaderLoc () {return this->headerLoc;}
	blockNum GetCurrBlockLoc () {return this->pCurrBlock->dataLoc;}
	int GetBlocksUsed () {return this->blocksInFile;}
	void SetNextDataBlockNum (blockNum newDBNum);
	void SetName (string newName);
	void Print();
	bool WriteToCurrBlock (string data);
	bool LoadNextBlock ();
	bool LoadFirstBlock ();
	bool Save ();
	bool Load ();
};
#endif
