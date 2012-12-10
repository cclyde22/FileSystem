#include "File.h"
using namespace std;

const int FBQ_HEADER_SIZE = 128;

/* FBQ hardware declaration used to write FBQ to disc. 	*/
struct FBQHeaderBlock
{ blockNum freeBlockLocs[FBQ_HEADER_SIZE];};

/* FBQ Software declarations for use when in memory		*/
class FBQNode {
	public:
		blockNum freeBlockNumber;
		FBQNode *pNext;
		FBQNode (blockNum fbn) : freeBlockNumber(fbn), pNext(NULL) { }
};
		
class FreeBlockQueue {
	private:
		FBQHeaderBlock fbqHeader;
		FBQNode *pHead;
		FBQNode *pTail;
		int discID;
		short int count;
		bool Find (blockNum searchKey) const;
		bool isEmpty () const {return this->count == 0 ? true : false; }
		void EmptyFBQ ();
	public:
		FreeBlockQueue () :	pHead(NULL), pTail(NULL), count(0) { }
		~FreeBlockQueue () {}
		void setDiscID (int idNum) {this->discID = idNum;}
		void Init(short int totalBlocks);
		bool SaveFBQ ();
		bool LoadFBQ ();
		void FreeBlock(blockNum blockToFree);
		short int GetCount () {return this->count;}
		blockNum GetFreeBlock ();		
};	
