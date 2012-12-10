/* Once a disc is in use (after createdisc or opendisc) it is referred to 
   through a single integer, which those functions return. When creating a
   disc, its size in specified in multiples of 512 byte blocks */

typedef unsigned char byte;
typedef byte block[512];

int createdisc(const char *name, int number_of_blocks); // create a new disc
int mountdisc(const char *name);                        // open an existing disc
int opendisc(const char *name);                         // alternate name
void dismountdisc(int discnum);                         // close
void closedisc(int discnum);                            // alternate name

/* when you access a disc, all you can do is read or write a whole block.
   You must specify which disc, the number of the block to be read or
   written (blocks are numbered from 0 to numblocks-1), and of course
   provide the the 512 byte array for the data. These two functions
   return 1 to indicate success, and 0 to indicate failure.
   The blocks are declared as void* because C is peculiar. */

int discsize(int discnum);                        // returns number of blocks
int writeblock(int discnum, int blocknum, void *b);
int readblock(int discnum, int blocknum, void *b);

