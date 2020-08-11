Tim Callies and Mugdha Danda

cacheSimulator.c:
	The C code which contains our project.

Instructions on how to run the cache simulator:

1. Type "make" to compile the program and create the executable.
2. Type "./cacheSimulator -f program.mc -b <blocksize> -s <number of sets> -a <number of ways>" or "./cachesimulator" to run it.

Test Cases:
	Using assembler, we will get the machine code for these text files, which contain assembly code.
	
testdemo.as.4.2.1:
	This test case is to make sure our simulator is working accurately.
Test1.as.256.1.1:
	In this test case, we are adding a bunch of instructions apart from lw and sw, to check that our cache is not interrupting the simulator.	
Test1.as.4.1.16:
	In this test case, we have the assembly code similar to Test1.as.256.1.1. However, our cache is fully associative.
Test1.as.4.16.1:
	In this test case, we have the assembly code similar to Test1.as.256.1.1. However, our cache is direct-mapped.
Test2.as.8.8.2:
	In this test case, we are testing a bunch of SW and LW. Along with this, our cache is 2-way set associative.

Overview:
	Description of our process working on the project, as well as a high level overview of how it works.