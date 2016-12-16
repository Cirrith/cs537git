#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include "fs.h"
#include "stat.h"

const char* memBlock;
uint size, nblocks, ninodes;	
uint dataStart, dataEnd;

char* getAddr(int block) {
	return (char*) &memBlock[block*BSIZE];
}

int checkBitmap(int block) {
	int bitmapBlock = 2 + ninodes/IPB + 1;
	char* bitmap = getAddr(bitmapBlock);
	int index = block / 8;
	int pos = block % 8;
	if((bitmap[index]>>pos) & 0x1) {
		return 0;
	}
	return -1;
}

int checkIndirect(int block) {
	int i;
	uint* indirect;
	
	if((block < dataStart) | (block > dataEnd)) {  // Check if valid data block
		return -1;
	}
	indirect = (uint*)getAddr(block);  // Get Address of block
	
	for(i = 0; i < NINDIRECT; i++) {  // Go through indirect block checking for bad address
		int iAddr = indirect[i];
		
		if(iAddr == 0) {  // Check if unallocated
			continue;
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
			for(j = 0; j < NINDIRECT; j++) {  // Go through indirect block checking for bad address
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
		//printf("Name: %s, %d\n", directory[1].name, directory[1].inum);
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

int badBitInode(struct dinode* head) {
	int i, j, indirect;
	
	for(i = 0; i < ninodes; i++) {
		if((head[i].type > 0) && (head[i].type < 4)) {
			for(j = 0; j < NDIRECT; j++) {
				if(checkBitmap(head[i].addrs[j]) < 0) {  // Block is used but not allocated
					return -1;
				}
			}
			if((indirect = head[i].addrs[NDIRECT]) != 0) {  // Uses Indirect
				if(checkIndirect(indirect) < 0) {
					return -1;
				}
			}
		}
	}
	return 0;
}

int badBitmap(struct dinode* head) {
	int inUse[size];
	int i, j, k, indirect;
	int bitmapBlock = 2 + ninodes/IPB + 1;
	memset(inUse, 0, size*sizeof(int));
	
	uint* addr;
	
	for(i = 0; i <= bitmapBlock; i++) {  // Inodes
		inUse[i] = 1;
	}
	
	for(i = 1; i < ninodes; i++) {
		if((head[i].type > 0) & (head[i].type < 4)) {
			for(j = 0; j < NDIRECT; j++) {
				inUse[head[i].addrs[j]] = 1;  // Set in use
			}
			
			if((indirect = head[i].addrs[NDIRECT]) != 0) {
				inUse[indirect] = 1;
				addr = (uint*)getAddr(indirect);
				for(k = 0; k < NINDIRECT; k++) {
					inUse[addr[k]] = 1;
				}
			}
		}
	}
	
	for(i = 0; i < size; i++) {
		int checked = checkBitmap(i);  // Returns 0 if allocated
		
		if((checked == 0) & (inUse[i] == 0)) {
			//printf("bit1: %d\n", i);
			return -1;
		}
	}
	
	for(i = size; i < BPB; i++) {
		if(checkBitmap(i) == 0) {
			//printf("bit2\n");
			return -1;
		}
	}
	return 0;
}

int badInUse(struct dinode* head) {
	int inUse[size];
	int i, j, k, indirect;
	int bitmapBlock = 2 + ninodes/IPB + 1;
	memset(inUse, 0, size*sizeof(int));
	
	uint* addr;
	
	for(i = 0; i <= bitmapBlock; i++) {  // Inodes
		inUse[i] = 1;
	}
	
	for(i = 1; i < ninodes; i++) {
		if((head[i].type > 0) & (head[i].type < 4)) {
			for(j = 0; j < NDIRECT; j++) {
				if(head[i].addrs[j] != 0) {
					if(inUse[head[i].addrs[j]] == 1) {
						return -1;
					} else {
						inUse[head[i].addrs[j]] = 1;  // Set in use
					}
				}
			}
			
			if((indirect = head[i].addrs[NDIRECT]) != 0) {
				if(inUse[indirect] == 1) {
					return -1;
				} else {
					inUse[indirect] = 1;
				}
				
				addr = (uint*)getAddr(indirect);
				for(k = 0; k < NINDIRECT; k++) {
					if(addr[k] != 0) {
						if(inUse[addr[k]] == 1) {
							return -1;
						} else {
							inUse[addr[k]] = 1;  // Set in use
						}
					}
				}
			}
		}
	}
	return 0;
}

// Assuming Directories don't have more than 1 datablock

int badParentDir(struct dinode* head) {
	int i, j, inum;
	struct dirent *direct, *subdirect;
	
	for(i = 2; i < ninodes; i++) {
		if(head[i].type == 1) {  // Found Directory
			direct = (struct dirent *)getAddr(head[i].addrs[0]);  // Get Directory entries
			for(j = 0; j < NDIR; j++ ) {
				inum = direct[j].inum;
				if(head[inum].type == 1) {  // If contains directory
					subdirect = (struct dirent*)getAddr(head[inum].addrs[0]);
					if(subdirect[1].inum != i) {
						return -1;
					}
				}
			}
		}
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
	

/*
	struct dinode* inode = (struct dinode*)getAddr(2);

	int i, j, k, indirect;
	uint* addr;
	for(i = 0; i < ninodes; i++) {
		printf("%d - Type: %hu, Major: %hu, Minor: %hu, Nlink: %hu, Size: %d\n", i, inode[i].type, inode[i].major, inode[i].minor, inode[i].nlink, inode[i].size);
		if(inode[i].type != 0) {
			for(j = 0; j < NDIRECT; j++) {
				printf("Addr[%d] = %d\n", j, inode[i].addrs[j]);
			}
			
			if((indirect = inode[i].addrs[NDIRECT]) != 0) {
				addr = (uint*)getAddr(indirect);
				for(k = 0; k < NINDIRECT; k++) {
					if(addr[k] != 0) {
						printf("I[%d]=%d\n", k, addr[k]);
					}
				}
			}
		}
	}
*/
	
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
	
	if((ret = badBitInode((struct dinode*)getAddr(2))) < 0) {
		fprintf(stderr, "ERROR: address used by inode but marked free in bitmap.\n");
		exit(1);
	}
	
	if((ret = badBitmap((struct dinode*)getAddr(2))) < 0) {
		fprintf(stderr, "ERROR: bitmap marks block in use but it is not in use.\n");
		exit(1);
	}
	
	
	if((ret = badInUse((struct dinode*)getAddr(2))) < 0) {
		fprintf(stderr, "ERROR: address used more than once.\n");
		exit(1);
	}
	
	return 0;
}
