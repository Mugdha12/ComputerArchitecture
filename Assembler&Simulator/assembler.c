#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<math.h>
#define ADD 0
#define NAND 1
#define LW 2
#define SW 3
#define BEQ 4
#define JALR 5
#define HALT 6
#define NOOP 7
int main(int argc,char *argv[])
{
	int c;
	char *fileName;
	char *outFileName;
	FILE *wp,*fs;
	char str[100];
	char *line;
	int i,loc,pos,oflag,x = 0;
	char holdL[100],holdM[100];
	const char *token;
	while((c = getopt(argc,argv,"i:o:")) != -1)
  	{
		switch(c)
		{
			case'i':
			fileName = optarg;
			break;
			case'o':
			outFileName = optarg;
			oflag=1;
			break;
		}//switch
	}//while

	fs=fopen(fileName,"r");
	if(fs == NULL)
	{
		
		perror("Error opening file");
		return (-1);
	}//if
	while(fgets(holdM,100,fs) != NULL )
	{
	 	i++;
		
	}//while
	char labAdd[i][7];

	rewind(fs);
		i=0;

		while(fgets(holdL,100,fs) != NULL)
		{
	 		token=strtok(holdL," \t\n");
			for(int l=0;l<i;l++)
				{				
					if(strcmp(labAdd[l],"nolab") != 0 && token != NULL && strcmp(labAdd[l], token) == 0)
					{
						
						perror("Error label is a duplicate");
						return (-1);	
					}
				}//for 
			
			if(token != NULL && strcmp(token,"lw") !=0 && strcmp(token,"sw") !=0 && strcmp(token,"add") !=0 && strcmp(token,"nand") !=0 && strcmp(token,"beq") !=0 && strcmp(token,"jalr") !=0 && strcmp(token,"halt") !=0 && strcmp(token,"noop") !=0)
			{	
				
				strcpy(labAdd[i],token);	
				if(strlen(labAdd[i]) > 7  || !((labAdd[i][0]>= 65 && labAdd[i][0] <= 90 ) || ((labAdd[i][0]>= 97 && labAdd[i][0] <= 122) )))
				{
					perror("Error label is invalid");
					return (-1);		
				}//if
			}//if
			else
			{
				strcpy(labAdd[i],"nolab");
			}			
		i++;
	}
	
	int hold[i];
	rewind(fs);
		loc=0;
	if(oflag == 1)
		{

			wp=fopen(outFileName,"w+");
			if(wp == NULL)
			{
			perror("Error opening file");
			return (-1);
			}
}
	
	while(fgets(str,100,fs) != NULL)
	{
		
		line = strtok(str, " \t\n");
		x=0;
		while(line != NULL)
		{
			hold[loc]=0;
				if((strcmp(line,"add")) == 0)
				{
					hold[loc] = hold[loc]  | (ADD << 22);
					line = strtok(NULL, " \t\n");
					if(line != NULL)
					{
					hold[loc] = hold[loc]  | ((atoi(line)) << 0);
					}
					line = strtok(NULL, " \t\n");
					if(line != NULL)
					{
					hold[loc] = hold[loc]  | ((atoi(line)) << 19);
					}
					line = strtok(NULL, " \t\n");
					
					hold[loc] = hold[loc]  | ((atoi(line)) << 16);
					line = strtok(NULL, " \t\n");

				}
				else if((strcmp(line,"nand")) == 0)
				{
					hold[loc] = hold[loc]  | (NAND << 22);
					
					hold[loc] = hold[loc] | (atoi(strtok(NULL, " \t\n")) << 0);
					
					hold[loc] = hold[loc]  | (atoi(strtok(NULL, " \t\n")) << 19);
					
					hold[loc] = hold[loc] | (atoi(strtok(NULL, " \t\n")) << 16);
					line = strtok(NULL, " \t\n");
				}
				else if((strcmp(line,"lw")) == 0)
				{
					hold[loc] = hold[loc]  | (LW << 22);
					
					hold[loc] = hold[loc]  | (atoi(strtok(NULL, " \t\n")) << 19);
					
					hold[loc] = hold[loc] | (atoi(strtok(NULL, " \t\n")) << 16);
					line = strtok(NULL, " \t\n");
					if( line != NULL )
					{

					if((pos = atoi(line)) != 0  )
					{
						if((pos <= ((1<<16)-1)) && (pos >= ((1<<16)*-1)))
						{
							hold[loc] = hold[loc]  |  (pos << 0);		
						}
						else if(pos > ((1<<16)-1))
						{
							perror("offset is greater than 16 bits");
							return -1;
						}
						else
						{
							perror("offset is lesser than 16 bits");
							return -1;

						}
					}
					else
					{
						for(int j=0; j<i; j++)
						{
							if(strcmp(line,labAdd[j]) == 0)
							{
								hold[loc] = hold[loc] |  (j<<0);
								x=1;
								j=i+1;
							}
						
						}
						if(x==0)
						{
							perror("label does not exist");
							return -1;
						}
						
					}}
					line = strtok(NULL, " \t\n");

				}
				else if((strcmp(line,"sw")) == 0)
				{
					hold[loc] = hold[loc] |  (SW << 22);
					
					hold[loc] = hold[loc] |  (atoi(strtok(NULL, " \t\n")) << 19);
					
					hold[loc] = hold[loc] |  (atoi(strtok(NULL, " \t\n")) << 16);
					line = strtok(NULL, " \t\n");
					if( line != NULL )
					{

					if((pos = atoi(line)) != 0  )
					{
						if((pos <= ((1<<16)-1)) && (pos >= ((1<<16)*-1)))
						{
							hold[loc] = hold[loc]  |  (pos << 0);		
						}
						else if(pos > ((1<<16)-1))
						{
							perror("offset is greater than 16 bits");
							return -1;
						}
						else
						{
							perror("offset is lesser than 16 bits");
							return -1;

						}
					}
					else
					{
						for(int j=0; j<i; j++)
						{
							if(strcmp(line,labAdd[j]) == 0)
							{
								hold[loc] = hold[loc] |  (j<<0);
								x=1;
								j=i;
							}
						}
						if(x==0)
						{
							perror("label doesnot exist");
							return -1;
						}


					}}
					line = strtok(NULL, " \t\n");
				}
				else if((strcmp(line,"beq")) == 0)
				{
					hold[loc] = hold[loc] |  (BEQ << 22);
					
					hold[loc] = hold[loc] |  (atoi(strtok(NULL, " \t\n")) << 19);
					
					hold[loc] = hold[loc] |  (atoi(strtok(NULL, " \t\n")) << 16);
					line = strtok(NULL, " \t\n");
					if( line != NULL )
					{

					if((pos = atoi(line)) != 0  )
					{
						if((pos <= ((1<<16)-1)) && (pos >= ((1<<16)*-1)))
						{
							hold[loc] = hold[loc]  |  (pos << 0);		
						}
						else if(pos > ((1<<16)-1))
						{
							perror("offset is greater than 16 bits");
							return -1;
						}
						else
						{
							perror("offset is lesser than 16 bits");
							return -1;

						}
					}
					else
					{
						for(int j=0; j<i; j++)
						{
							if(strcmp(line,labAdd[j]) == 0)
							{
								pos=j-(loc+1);
								if(pos < 0)
								{
								pos = pos & ( 0x0000ffff);
								}
								hold[loc] = hold[loc] |  (pos << 0);
								x=1;	
								j=i;
							}
						}
						if(x==0)
						{
							perror("label doesnot exist");
							return -1;
						}


					}
				}//if

					line = strtok(NULL, " \t\n");
				}
				else if((strcmp(line,"jalr")) == 0)
				{
					hold[loc] = hold[loc] |  (JALR << 22);
					line = strtok(NULL, " \t\n");
					if(line != NULL)
					{
					hold[loc] = hold[loc] |  (atoi(line) << 19);
					}
					line = strtok(NULL, " \t\n");
					if(line != NULL)
					{
					hold[loc] = hold[loc] |  (atoi(line) << 16);
					}
					line = strtok(NULL, " \t\n");

				}
				else if((strcmp(line,"halt")) == 0)
				{
					hold[loc] = hold[loc] |  (HALT << 22);
					hold[loc] = hold[loc] |  (0 << 0);
					line = strtok(NULL, " \t\n");
				}
				else if((strcmp(line,"noop")) == 0)
				{
					hold[loc] = hold[loc] |  (NOOP << 22);
					hold[loc] = hold[loc] |  (0 << 0);
					line = strtok(NULL, " \t\n");
				}
				else if((strcmp(line,".fill")) == 0)
				{
					line = strtok(NULL, " \t\n");
					if(line != NULL)
					{
						if((pos = atoi(line)) != 0)
						{
							hold[loc] = hold[loc] |  (pos << 0);		
						}
						else
						{
							for(int j=0; j<i; j++)
							{
								if(strcmp(line,labAdd[j]) == 0)
								{
									hold[loc] = hold[loc] |  (j<<0);
									j=i;
								}//if
							}//for
						}//else
					}//if
					line = strtok(NULL, " \t\n");
				}//else if
									
				else
				{
					 char *res;
					res=strtok(NULL, " \t\n");

							if(line != NULL && res!=NULL)
							{
							if(atoi(res)!=0)
							{
								perror("Unrecognized opcode");
								return -1;
							}
							else if(atoi(res) == 0)
							{
								strcpy(line,res);
							}}
				}	
										

				


			
    		}//while-line
		if(oflag == 1)
		{
		 fprintf(wp,"%d\n",hold[loc]);
		}
		else
		{
			printf("%d\n",hold[loc]);
		}
		loc=loc+1;
				
	}//while-str
	if(oflag == 1)
		{
			fclose(wp);	
		}
	fclose(fs);

	
}//main
