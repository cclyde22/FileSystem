#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>
#include "fakedisc.h"

int createdisc(const char *name, int number_of_blocks)
{ block b;
  int discnum, x;
  for (int i=0; i<512; i+=1)
    b[i]=0;
  discnum=open(name, O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0600);
  if (discnum<0)
  { fprintf(stderr, "Create failed for \"%s\"\n", name);
    return 0; }
  for (int i=0; i<number_of_blocks; i+=1)
  { x=write(discnum, b, 512);
    if (x<512)
    { fprintf(stderr, "Failed to create block %d of \"%s\"\n", i, name);
      return 0; } }
  close(discnum);
  return 1; }

int mountdisc(const char *name)
{ int discnum;
  discnum=open(name, O_RDWR | O_BINARY);
  if (discnum<0)
  { fprintf(stderr, "Failed to open \"%s\"\n", name);
    return -1; }
  return(discnum); }

int opendisc(const char *name)
{ return mountdisc(name); }
  
int discsize(int discnum)
{ int sz=lseek(discnum, 0, SEEK_END);
  return sz/512; }

void dismountdisc(int discnum)
{ close(discnum); }

void closedisc(int discnum)
{ close(discnum); }

int writeblock(int discnum, int blocknum, void *b)
{ int x=lseek(discnum, blocknum*512, SEEK_SET);
  if (x<0)
    return 0;
  x=write(discnum, b, 512);
  if (x<512)
    return 0;
  return 1; }
  
int readblock(int discnum, int blocknum, void *b)
{ int x=lseek(discnum, blocknum*512, SEEK_SET);
  if (x<0)
    return 0;
  x=read(discnum, b, 512);
  if (x<512)
    return 0;
  return 1; }

