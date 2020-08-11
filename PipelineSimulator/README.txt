Tim Callies and Mugdha Danda

pipelineSimulator.c:
	The C code which contains our project.

Instructions on how to run the pipeline simulator:

1. Type "make" to compile the program and create the executable.
2. Type "./pipelineSimulator -i program.mc

Test Cases:
	Using assembler, we will get the machine code for these text files, which contain assembly code.
Test1.as:
	 In this test case, we are testing instructions without any hazards.
Test2.as:
	This test case is to test, if we are able to data forwarding to fix data hazards.
Test3.as:
	This test case is to test, if we are able to do load stall and data forwarding to fix data hazards.
Test4.as:
	This test case is to test, if we are able to do load stall or not.
Test5.as:
	This test case is to test, if we are able to fix control hazard by doing flush and we are checking our instruction stat count, fetched and retired.
Test6.as:
	This test case is to test a branch, which is not taken.
Test7.as:
	This test case is to test data hazards before store word.
Test8.as:
	This test case is to test data hazards before store word with slightly different data hazard.
Test9.as:
	In this testcase, we have hazards for each instruction excluding the first one.
	
Test10.as:
	This test case is to see, if we can fix the data and control hazards happening at the same time by doing data forwarding, flush and load stall.
	
Test11.as:
	This test case is mainly to test instruction fetch count, by passing in noop instructions.
	
Overview:
	Description of our process working on the project, as well as a high level overview of how it works.