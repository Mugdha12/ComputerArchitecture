#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define NUMMEMORY 65536 /* maximum number of data words in memory */
#define NUMREGS 8 /* number of machine registers */
#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5
#define HALT 6
#define NOOP 7
#define NOOPINSTRUCTION 0x1c00000

enum action_type {cache_to_processor, processor_to_cache, memory_to_cache, cache_to_memory,cache_to_nowhere};

void print_action(int address, int size, enum action_type type)
{
	printf("transferring word [%i-%i] ", address, address + size - 1);
	if (type == cache_to_processor) {
		printf("from the cache to the processor\n");
	} else if (type == processor_to_cache) {
		printf("from the processor to the cache\n");
	} else if (type == memory_to_cache) {
		printf("from the memory to the cache\n");
	} else if (type == cache_to_memory) {
		printf("from the cache to the memory\n");
	} else if (type == cache_to_nowhere) {
		printf("from the cache to nowhere\n");
	}
}

int blockSize;
int ways;
int sets;
typedef struct blk_struct
{
	int valid;
	int tag;
	int dirty;
	int lastUsed;
	int index;
	int *blockArray;
}blktype;

typedef struct state_struct
 {
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int num_memory;
	blktype ** cache;
} statetype;



//Updates the last used bits in a single array, which determines the LRU
//lastUsed == 0 is LRU
void setLastUsed(int setIndex, blktype set[]) {
	int oldValue = set[setIndex].lastUsed;

	for(int i=0; i<ways; i++)
	{
		if(set[i].lastUsed <= oldValue && i!=setIndex)
			set[i].lastUsed++;
	}

	set[setIndex].lastUsed = 0;
}

//Gets the index of a block in a set. If -1, then the block is not in the cache
int getFromCache(int tag, blktype set[])
{
	for (int i=0; i<ways; i++)
	{
		if( set[i].valid == 1 && set[i].tag == tag )
		{
			setLastUsed(i, set);
			return i;
		}
	}

	return -1;
}

//Stores a block into the cache
int loadIntoCache(int tag, blktype set[], int memoryIndex, statetype * state)
{
	int index = 0;

	int tagIndex = -1;
	int invalidIndex = -1;
	int lruIndex = -1;


	//Keep going through until we find a block with the same tag, a block that is invalid, or until we find the LRU.
	for (int i=0; i< ways; i++) {
		if(tagIndex<0 && tag == set[i].tag)
			tagIndex = i;
		if(invalidIndex<0 && set[i].valid == 0)
			invalidIndex = i;
		if(lruIndex<0 && set[i].lastUsed == ways-1)
			lruIndex = i;
	}
	if(tagIndex!=-1)
		index = tagIndex;
	else if(invalidIndex!=-1)
		index = invalidIndex;
	else
		index = lruIndex;

	//Write the dirty block into memory.
	if(set[index].dirty == 1)
	{
		print_action(set[index].index, blockSize, cache_to_memory);
		for(int i=0; i< blockSize; i++)
		{
			state->mem[set[index].index + i] = set[index].blockArray[i];
		}

		set[index].dirty = 0;
	}

	else if (set[index].valid == 1)
	{
		print_action(set[index].index, blockSize, cache_to_nowhere);
	}

	int memoryStart = memoryIndex - (memoryIndex%blockSize);
	print_action(memoryStart, blockSize, memory_to_cache);
	for(int i=0; i< blockSize; i++)
	{
		set[index].blockArray[i] = state->mem[memoryStart + i];
	}

	set[index].index = memoryStart;
	set[index].tag=tag;
	set[index].valid = 1;
	setLastUsed(index, set);

	return index;
}

int myLog2(int x)
{
	int ans = 0;
	while(x>>=1) ans++;
	return ans;
}

int field0(int instruction){
	return( (instruction>>19) & 0x7);
}

int field1(int instruction){
	return( (instruction>>16) & 0x7);
}

int field2(int instruction){
	return(instruction & 0xFFFF);
}

int opcode(int instruction){
	return(instruction>>22);
}

int setindex(int address){
	return((address >> (myLog2(blockSize)))  & (~(-sets)));
}

int blockoffset(int address){
	return(address & (~(-blockSize)) );
}

int tag(int address){
	return(address >> ((myLog2(blockSize))+(sets>>1)) );
}



int signExtend(int num){
	// convert a 16-bit number into a 32-bit integer
	if (num & (1<<15) ) {
		num -= (1<<16);
	}
	return num;
}

void run(statetype* state){

	// Reused variables;
	int instr = 0;
    int regA = 0;
	int regB = 0;
	int offset = 0;
	int branchTarget = 0;
	int aluResult = 0;

	int total_instrs = 0;

	// Primary loop
    while(1){
		//printf("\tPC: %i\n",state->pc);
		total_instrs++;

		//printState(state);

		// Instruction Fetch
		int thissetindex = setindex(state->pc);
		int thistag = tag(state->pc);
		int thisblockoffset = blockoffset(state->pc);

		int thisindex = getFromCache(thistag, state->cache[thissetindex]);

		if(thisindex < 0)
		{
			thisindex = loadIntoCache(thistag, state->cache[thissetindex], state->pc, state);
		}
		print_action(state->pc, 1, cache_to_processor);

		instr = state->cache[thissetindex][thisindex].blockArray[thisblockoffset];
		if (state->pc > 10)
		 break;

		/* check for halt */
		if (opcode(instr) == HALT) {
			break;
		}

		// Increment the PC
		state->pc = state->pc+1;

		// Set reg A and B
		regA = state->reg[field0(instr)];
		regB = state->reg[field1(instr)];

		// Set sign extended offset
		offset = signExtend(field2(instr));

		// Branch target gets set regardless of instruction
		branchTarget = state->pc + offset;

		/**
		 *
		 * Action depends on instruction
		 *
		 **/
		// ADD
		if(opcode(instr) == ADD){
			// Add
			aluResult = regA + regB;
			// Save result
			state->reg[field2(instr)] = aluResult;
		}

		// NAND
		else if(opcode(instr) == NAND){
			// NAND
			aluResult = ~(regA & regB);
			// Save result
			state->reg[field2(instr)] = aluResult;
		}

		// LW or SW
		else if(opcode(instr) == LW || opcode(instr) == SW){
			// Calculate memory address
			aluResult = regB + offset;
			if(opcode(instr) == LW){
				// Load
				thissetindex = setindex(aluResult);
				thistag = tag(aluResult);
				thisblockoffset = blockoffset(aluResult);

				//printf("\t\tLW: Index: %i\tTag: %i\t Offset:%i\n",thissetindex, thistag, thisblockoffset);

				thisindex = getFromCache(thistag, state->cache[thissetindex]);

				if(thisindex < 0)
				{
					thisindex = loadIntoCache(thistag, state->cache[thissetindex], aluResult, state);
				}
				print_action(aluResult, 1, cache_to_processor);
				state->reg[field0(instr)] = state->cache[thissetindex][thisindex].blockArray[thisblockoffset];

			}else if(opcode(instr) == SW){
				// Store
				thissetindex = setindex(aluResult);
				thistag = tag(aluResult);
				thisblockoffset = blockoffset(aluResult);

				thisindex = getFromCache(thistag, state->cache[thissetindex]);

				if(thisindex < 0)
				{
					thisindex = loadIntoCache(thistag, state->cache[thissetindex], aluResult, state);
				}

				print_action(aluResult, 1, processor_to_cache);
				state->cache[thissetindex][thisindex].blockArray[thisblockoffset] = regA;
				state->cache[thissetindex][thisindex].dirty = 1;
			}
		}
		// JALR
		else if(opcode(instr) == JALR){
			// rA != rB for JALR to work
			if(field0(instr) != field1(instr)){
				// Save pc+1 in regA
				state->reg[field0(instr)] = state->pc;
				//Jump to the address in regB;
				state->pc = state->reg[field1(instr)];
			}
		}
		// BEQ
		else if(opcode(instr) == BEQ){
			// Calculate condition
			aluResult = (regA == regB);

			// ZD
			if(aluResult){
				// branch
				state->pc = branchTarget;
			}
		}
    } // While
	//print_stats(total_instrs);
}



int main(int argc, char** argv)
{
	char fname[256];
	char blocks[8];
	char numc[8];
	char assc[8];
	FILE *fp;
	statetype* state = (statetype*)malloc(sizeof(statetype));

	int cin;
	opterr = 0;
	fname[0] = '\0';
	blocks[0]='\0';
	numc[0]='\0';
	assc[0]='\0';
	while((cin = getopt(argc, argv, "f:b:s:a:")) != -1){
	//Reads the command
		switch(cin)
		{
			case 'f':
				strncpy(fname, optarg, strlen(optarg)+1);
				//printf("FILE: %s\n", fname);
				break;
			case 'b':


				 strncpy(blocks, optarg, strlen(optarg)+1);
				 break;
			case 's':


				 strncpy(numc, optarg, strlen(optarg)+1);
				 break;
			case 'a':


				 strncpy(assc, optarg, strlen(optarg)+1);
				 break;
			case '?':
				if(optopt == 'f'){
					printf("Option -%c requires an argument.\n", optopt);
				}
				else if(isprint(optopt)){
					printf("Unknown option `-%c'.\n", optopt);
				}
				else{
					printf("Unknown option character `\\x%x'.\n", optopt);
					return 1;
				}
				break;
			default:
				abort();
		}
	}

	//Takes input from machine
	if(fname[0] == '\0')
	{
		printf("Enter the machine code program to simulate:");
		scanf("%s",fname);
		getchar();
	}
	if(blocks[0] == '\0')
	{
		printf("Enter the block size of the cache (in words):");
		scanf("%s",blocks);
		getchar();
	}
	if(numc[0] == '\0')
	{
		printf("Enter the number of sets in the cache:");
		scanf("%s", numc);
		getchar();
	}
	if(assc[0]=='\0')
	{
		printf("Enter the associativity of the cache:");
		scanf("%s",assc);
	}


	//Reads the file
	fp = fopen(fname, "r");

	if (fp == NULL) {
		printf("Cannot open file '%s' : %s\n", fname, strerror(errno));
		return -1;
	}
	int line_count = 0;
	int c;
	while (EOF != (c=getc(fp))) {
		if ( c == '\n' ){
			line_count++;
		}
	}

	rewind(fp);

	blockSize=atoi(blocks);
	sets=atoi(numc);
	ways=atoi(assc);

	state->pc = 0;


	memset(state->mem, 0, NUMMEMORY*sizeof(int));
	memset(state->reg, 0, NUMREGS*sizeof(int));
	state->num_memory = line_count;

	state->cache = malloc(sizeof(blktype *)*sets);

	for (int i=0; i < sets; i++)
	{
		state->cache[i] = malloc(sizeof(blktype)*ways);
		for (int j=0; j < ways; j++)
		{
			state->cache[i][j].lastUsed = 0;
			state->cache[i][j].valid = 0;
			state->cache[i][j].dirty = 0;
			state->cache[i][j].tag = 0;
			state->cache[i][j].blockArray = malloc(sizeof(int) * blockSize);
			for(int k=0; k< blockSize; k++)
			{
				state->cache[i][j].blockArray[k] = 0;
			}
		}
	}


	char line[256];
	int i = 0;

	while (fgets(line, sizeof(line), fp)) {
	    /* note that fgets doesn't strip the terminating \n, checking its
	    presence would allow to handle lines longer that sizeof(line) */
		state->mem[i] = atoi(line);
		i++;
	}
	fclose(fp);

	/** Run the simulation **/
	run(state);
/*
	for (int i=0; i<8; i++)
		printf("Reg[%i]: %i\n",i,state->reg[i]);
	for (int i=0; i<10; i++)
		printf("Mem[%i]: %i\n",i,state->mem[i]);
	for (int i=0; i<sets; i++)
	{
		for (int j=0; j<ways; j++)
			printf("Cache[%i][%i]: %i\n",i,j,state->cache[i][j].index);
	}*/
	free(state);
}