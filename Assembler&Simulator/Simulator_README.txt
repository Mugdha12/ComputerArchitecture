Belinda Huang and Mugdha Danda

Instructions on how to run the assembler:

1. type "make" to compile the assembler and create the executable.
2. type "./assembler -i program.as > program.mc" or "./assembler -i program.as -o program.mc" 
	where "program.as" is the assembly code input and "program.mc" is the file it will output the machine code to

Instructions on how to run the simulator once you have your program in machine code:

1. type "make" to compile the simulator and create the executable
2. type "./simulator -i program.mc"

Test Cases:
test0.as
	This tests sw, sw using a label in one of the fields, nand, nand using a label, jalr, jalr with a label, halt, and .fill.

test1.as
	This tests lw, beq, nand, halt, .fill, add, and the use of labels.

test2.as 
	This tests lw noop, beq, add, .fill, and use of labels.  

