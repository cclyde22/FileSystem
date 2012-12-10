#include "File.h"
#include "RootDir.h"
#include "FBQ.h"
using namespace std;

/* 	Hardware representation - used to save to disc.
	Stores info about the filesystem	*/
struct SuperBlock {
	char discName[32];
	int totalBlocks;
	int blocksFree;
	byte padding[472];
};

/*	Software representation for when FileSys is in memory	*/
class FileSys {
	private:
		short int discID;
		SuperBlock FSHeader;	  
		FreeBlockQueue fbq; 
		RootDir root;
		void GetSBInfo ();
		void UpdateSuperBlock ();
		File* OpenFile (blockNum fileHeaderLoc);
		void CloseFile (File* FileToClose);
	public:
		FileSys () {this->discID = 0;}
		~FileSys () {this->discID = 0;}
		void SetDiscName (string newName);
		int GetDiscID () const {return this->discID;}
		void CreateDisc ();
		void ReformatDisc ();
		void MountDisc ();
		void ListRoot () const {this->root.Print();}	
		void CreateFile ();
		void PrintFile ();
		void UpdateFile ();
		void DeleteFile ();
		void Exit (); 	
};
