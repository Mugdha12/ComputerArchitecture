Belinda Huang and Mugdha Danda

Instructions on how to run the assembler:

1. type "make" to compile the program and create the executable.
2. type "./assembler -i program.as > program.mc" or "./assembler -i program.as -o program.mc" 
	where "program.as" is the assembly code input and "program.mc" is the file it will output the machine code to
	
	
Test Cases:

test0.as
	This tests the error case of duplicate labels.

test1.as
	This tests the error case of a label starting with a number.

test2.as
	This tests the error case of a positive offset_field does not fit in 16 bits.
	
test3.as
	This tests the error case of an unrecognized opcode

test4.as
	This tests the error case of a negative offset_field that doesn't fit in 16 bits.
	
test5.as
	This tests the error an undefined label that starts with a wierd character, in this case a %.
	
test6.as
	This tests sw, sw using a label in one of the fields, nand, beq, jalr, and .fill.

test7.as
	This tests the error case of a missing label.

test8.as
	This tests lw, add, beq, done, halt, .fill, use of labels, and noop.
