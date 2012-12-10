#include "File.h"
using namespace std;

const int ROOT_HEADER_SIZE = 128;
/* Hardware representation of the root directory, used to write to disc. */
struct RootHeaderBlock 
{ blockNum fileBlockLocs[ROOT_HEADER_SIZE];};

/* Software Declarations for when the root directory is in memory	*/

class FileNode {
	public:
		string fileName;
		blockNum fileLoc;
		FileNode *pNext;
		/*	Constructor	*/
		FileNode (string name, blockNum location) : fileName(name), fileLoc(location), pNext(NULL) 
			{ }
		void Print();
};

class RootDir {
	private:
		FileNode *pHead;
		int discID;
		RootHeaderBlock rootHeader;
		unsigned short count;
	public:
		RootDir ();
		~RootDir () {}
		void setDiscID (int idNum) {this->discID = idNum;}
		unsigned short GetCount() const {return this->count;}
		bool AddFile (string name, blockNum location);
		bool RemoveFile (string fileToRemove);
		bool SaveRoot ();
		bool LoadRoot ();
		void Print() const;
		blockNum FindFile(string searchKey) const;	
};

