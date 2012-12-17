#include "FBQ.h"
using namespace std;

/*	FreeBlockQueue Methods	*/

/*	Used to make sure the same block isn't freed twice,
	Iterate through the queue using block number as the 
	search key. Return true if found, otherwise false.	*/
bool FreeBlockQueue::Find (blockNum searchKey) const {
	if ( (this->isEmpty()) == true )
		return false;
	else {
		FBQNode* pCurr = this->pHead;
		while (pCurr->freeBlockNumber != searchKey) {
			if (pCurr->pNext == NULL)
				return false;
			else
				pCurr = pCurr->pNext;
		}
		return true;
	}
}

/*	Used to make sure the Queue is empty before any block numbers are added to it.
	Deletes each node in the queue and subtracts 1 from count each time.
	Zero fill FBQ hardware block and save to disc.		*/	
void FreeBlockQueue::EmptyFBQ () {
	FBQNode* pCurr = this->pHead;
	while (pCurr != NULL) {
		FBQNode* pTemp = pCurr;
		pCurr = pCurr->pNext;
		delete pTemp;
		this->count--;
	}
	for (int i = 0; i < FBQ_HEADER_SIZE; i++)
		this->fbqHeader.freeBlockLocs[i] = 0;
	bool saved = this->SaveFBQ();
	if (saved == false)
		cout << "Error saving FBQ to disc\n";
}

/*	Initialize free block queue in memory. Free all block numbers except 0, 1, and 2.
	Also fill fbqHeader with newly freed blocks and save to disc.
	Used in CreateDisc and ReformatDisc 	*/
void FreeBlockQueue::Init (short int totalBlocks) {
	int i, j;
	this->EmptyFBQ();
	for (i = 3, j = 0; i < totalBlocks && j < FBQ_HEADER_SIZE; i++, j++) {
		this->FreeBlock(i);
		this->fbqHeader.freeBlockLocs[j] = i;
	}
	while (j < FBQ_HEADER_SIZE) {
		this->fbqHeader.freeBlockLocs[j] = 0;
		j++;
	}
	bool saved = this->SaveFBQ();
	if (saved == false)
		cout << "Error saving FBQ to disc\n";
}

/*	Save the FBQ Header to disc	*/
bool FreeBlockQueue::SaveFBQ () {
	int success = writeblock(this->discID, FBQ_ADDRESS, &(this->fbqHeader));
	if (success == 1)
		return true;
	else
		return false;
}

/*	Read FBQ Header from disc and add all blocks to the queue.	*/
bool FreeBlockQueue::LoadFBQ () {
	int success = readblock(this->discID, FBQ_ADDRESS, &(this->fbqHeader));
	if (success) {
		blockNum tempFBQArr[FBQ_HEADER_SIZE];
		for (int i = 0; i < FBQ_HEADER_SIZE; i++)
			tempFBQArr[i] = this->fbqHeader.freeBlockLocs[i];
		this->EmptyFBQ();
		for (int i = 0; i < FBQ_HEADER_SIZE; i++)
			this->FreeBlock(tempFBQArr[i]);
		bool saved = this->SaveFBQ();
		if (saved == false)
			cout << "Error saving FBQ to disc\n";
	} 
}

/*	Add a block number to the free block queue (both software and hardware versions,
	remember to increase count and save the fbq header to disc afterwards	*/
void FreeBlockQueue::FreeBlock(blockNum blockToFree) {
	if ( (this->Find(blockToFree)) != true ) {
		FBQNode* pNewNode = new FBQNode (blockToFree);
		if (this->isEmpty())
			this->pHead = pNewNode;
		else
			this->pTail->pNext = pNewNode;
		this->pTail = pNewNode;
		this->count++;
		this->fbqHeader.freeBlockLocs[count] = blockToFree;
		bool saved = this->SaveFBQ();
		if (saved == false)
			cout << "Error saving FBQ to disc\n";
	}
	else {
		if (blockToFree > 3)
			cout << "Block number is already in free block queue\n";
	}
} 

/*	Get a free block for use from the queue,
	remember to decrease count and save the fbq header to disc afterwards	*/	
blockNum FreeBlockQueue::GetFreeBlock () {
	if ( (this->isEmpty()) != true ) {
		FBQNode* pTemp = this->pHead;
		blockNum freeBlock = this->pHead->freeBlockNumber;
		if (this->pHead->pNext == NULL)
			this->pTail = NULL;
		this->pHead = this->pHead->pNext;
		delete pTemp;
		int i;
		for (i = 0; i < this->count; i++) 
			if (freeBlock == this->fbqHeader.freeBlockLocs[i])
				break;
		if (i == this->count) {
			cout << "Error removing from root header, block location not found.\n";
			return -1;
		}
		else {
			for (int j = i; j < this->count; j++)
				this->fbqHeader.freeBlockLocs[j] = this->fbqHeader.freeBlockLocs[j+1];
			this->count--;
			bool saved = this->SaveFBQ();
			if (saved == false) {
				cout << "Error saving FBQ to disc\n";
				return -1;
			}
			return freeBlock;
		}
	}
	else {
		cout << "No more free blocks\n";
		return -1;
	}
} 
