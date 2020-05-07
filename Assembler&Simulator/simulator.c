#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<math.h>
#define NUMREGS 8
#define NUMMEMORY 65536
#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5
#define HALT 6
#define NOOP 7
typedef struct state_struct
 {
	int pc;
	int mem[NUMMEMORY];
	int reg[NUMREGS];
	int num_memory;
} statetype;
int convert_num(int num);
void print_stats(int n_instrs);
void print_state(statetype *stateptr);
int main(int argc,char *argv[])
{
	statetype type;
 	FILE *fp;
	int c,pos,opcode,regD,regA,offset,regB,i,j,x=0;
	char *fileName;
	char str[200];
	type.pc=0;
	for (int i=0; i<8;i++)
	{
		type.reg[i]=0;
	}//for
	while((c = getopt(argc,argv,"i:")) != -1)
  	{
		switch(c)
		{
			case'i':
			fileName = optarg;
			break;
		}//switch
	}//while
	fp=fopen(fileName,"r");
	if(fp == NULL)
	{
		
		perror("Error opening file");
		return (-1);
	}//if
	while(fgets(str,200,fp) != NULL)
	{
	   type.mem[i]=atoi(str);
	   i++;	
	}
	type.num_memory=i;
	while( j<65536 && x == 0)
	{
		pos=type.mem[type.pc];
		print_state(&type);
		type.pc++;
		opcode = pos>>22;
		if(opcode == ADD || opcode == NAND)
		{
			regA=(pos>>19)&7;
			regB=(pos>>16) & 7;
			regD=(pos>>0)&7;
			if(opcode == ADD)
			{
				type.reg[regD]=type.reg[regA]+type.reg[regB];
			}
			else
			{
				type.reg[regD]=~(type.reg[regA]&type.reg[regB]);
			}
			
		}
		else if(opcode == LW || opcode == SW || opcode ==BEQ)
		{
			regA=(pos>>19)&7;
			regB=(pos>>16) & 7;
			offset=((pos)&(65535));
			offset=convert_num((offset));
			if(opcode == LW)
			{
				type.reg[regA]=type.mem[type.reg[regB]+offset];
				
			}
			else if(opcode == SW)
			{
				type.mem[type.reg[regB]+offset]=type.reg[regA];	

			}
			else//beq
			{
			 	if(type.reg[regA] == type.reg[regB])
				{
					type.pc=type.pc+offset;
				}//if
			}//else
		}//else if
		else if(opcode == JALR)
		{
			regA=(pos<<19) && 7;
			regB=(pos<<16)&&7;
			type.reg[regA]=type.pc;
			type.pc=type.reg[regB];	
		}
		else if(opcode == HALT)
		{
			printf("Machine halted\n");
			x=1;
		}
		j++;
	 }
	 print_stats(j);
}
void print_state(statetype *stateptr)
{
	int i;
	printf("\n@@@\nstate:\n");
	printf("\tpc %d\n", stateptr->pc);
	printf("\tmemory:\n");
	for(i = 0; i < stateptr->num_memory; i++)
	{
		printf("\t\tmem[%d]=%d\n", i, stateptr->mem[i]);
	}
	printf("\tregisters:\n");
	for(i = 0; i < NUMREGS; i++)
	{
		printf("\t\treg[%d]=%d\n", i, stateptr->reg[i]);
	}
	printf("end state\n");
}
int convert_num(int num){
	if(num &(1<<15)){
		num-=(1<<16);
	}
	return num;
}
void print_stats(int n_instrs){
	printf("INSTRUCTIONS: %d\n", n_instrs); // total executed instructions
}

