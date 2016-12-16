#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include "fs.h"
#include "stat.h"

// Indirect address per block
#define IDPB BSIZE/4

const char* memBlock;
uint size, nblocks, ninodes;	
uint dataStart, dataEnd;

char* getAddr(int block) {
	return (char*) &memBlock[block*BSIZE];
}

int checkBitmap(int block) {
	int bitmapBlock = 2 + ninodes/IPB;
	char* bitmap = getAddr(bitmapBlock);
	int index = block / 8;
	int pos = block % 8;
	if(bitmap[index] & (0x1<<pos)) {
		return -1;
	}
	return 0;
}

int checkIndirect(int block) {
	int i;
	uint* indirect;
	
	if((block < dataStart) | (block > dataEnd)) {  // Check if valid data block
		return -1;
	}
	indirect = (uint*)getAddr(block);  // Get Address of block
	
	for(i = 0; i < IDPB; i++) {  // Go through indirect block checking for bad address
		int iAddr = indirect[i];
		
		if(iAddr == 0) {  // Check if unallocated
			continue;
		}	
		if((iAddr < dataStart) | (iAddr > dataEnd)) {  // Check if indirect is 
			return -1;
		}
		
		if(checkBitmap(iAddr) < 0) { // Check if allocated in bitmap
			return -1;
		}
	}
	return 0;
}



int badInode(struct dinode* head) {
	int i;
	for(i = 0; i < ninodes; i++) {
		if((head[i].type > 3) | (head[i].type < 0)) {
			return -1;
		}
	}
	return 0;
}

int badAddress(struct dinode* head) {
	int i, j;
	
	uint* indirect;
	
	for(i = 0; i < ninodes; i++) {
		int curAddr;
		for(j = 0; j < NDIRECT; j++) {  // Check Direct Addrs
			curAddr = head[i].addrs[j];
			if(curAddr == 0) {  // Check if unallocated
				continue;
			}
			
			if((curAddr < dataStart) | (curAddr > dataEnd)) {
				// printf("Bad Address1: %d\n", curAddr);
				return -1;
			}
		}
		
		if((curAddr = head[i].addrs[NDIRECT]) != 0) {  // Check Indirect Addrs
			if((curAddr < dataStart) | (curAddr > dataEnd)) {  // Check if valid data block
				return -1;
			}
			indirect = (uint*)getAddr(curAddr);  // Get Address of block
			for(j = 0; j < IDPB; j++) {  // Go through indirect block checking for bad address
				int iAddr = indirect[j];
				
				if(iAddr == 0) {  // Check if unallocated
					continue;
				}	
				if((iAddr < dataStart) | (iAddr > dataEnd)) {
					// printf("Bad Address2: %d\n", iAddr);
					return -1;
				}
			}
		}
	}
	return 0;
}

int badRoot(struct dinode* head) {
	struct dirent *directory;
	
	if(head[0].type != 0) {  // Inode 0 is not unalloacted
		//printf("Root1\n");
		return -1;
	}
	
	if(head[1].type != 1) {  // Not Directory
		//printf("Root2\n");
		return -1;
	}

	directory = (struct dirent *)getAddr(head[1].addrs[0]);
	
	if(directory[0].inum != 1) {  // Inode number is not itself
		//printf("Root3\n");
		return -1;
	}
	
	if(directory[1].inum != 1) {  // Has parent?
		//printf("Name: %s, %d\n", directory[1].name, directory[1].inum);
		//printf("Root4\n");
		return -1;
	}
	
	return 0;
}

int main(int argc, char *argv[]) {

	int fd;
	struct stat st;
	
	if(argc != 2) {
	  printf("Usage: %s file_system_image\n", argv[0]);
	  exit(1);
	}
	
	if((fd = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "image not found.\n");
		exit(1);
	}
	
	fstat(fd, &st);  // Get stats on opened file
	
	memBlock = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);  // Map file into filespace
	if (memBlock == MAP_FAILED) {
		fprintf(stderr, "image not found.\n");
		exit(1);
	}

	struct superblock *sb;
	sb = (struct superblock*) &memBlock[BSIZE];	
	
	size = sb->size;
	nblocks = sb->nblocks;
	ninodes = sb->ninodes;
	
	dataStart = 2 + ninodes/IPB + ceil(((double)nblocks)/((double)BPB));
	dataEnd = dataStart + nblocks;
	
	// printf("Size: %d, nblocks: %d, ninodes: %d\n",size, nblocks, ninodes);
	
	int i;
	for(i = 0; i < ninodes; i++) {
		// printf("%d - Type: %hu, Major: %hu, Minor: %hu, Nlink: %hu, Size: %d\n", i, inode[i].type, inode[i].major, inode[i].minor, inode[i].nlink, inode[i].size);
	}
	
	int ret;
	if((ret = badInode((struct dinode*)getAddr(2))) < 0) {
		fprintf(stderr, "ERROR: bad inode.\n");
		exit(1);
	}
	
	if((ret = badAddress((struct dinode*)getAddr(2))) < 0) {
		fprintf(stderr, "ERROR: bad address in inode.\n");
		exit(1);
	}
	
	if((ret = badRoot((struct dinode*)getAddr(2))) < 0) {
		fprintf(stderr, "ERROR: root directory does not exist.\n");
		exit(1);
	}
	
	return 0;
}
