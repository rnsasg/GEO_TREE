// C program for the above approach
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
 
void read_csv(const char *filename)
{
    FILE* fp = fopen(filename, "r");
  
    if (!fp)
    {
        printf("Can't open file\n");
    }
    else 
    {
        char buffer[1024];
        int row = 0;
        int column = 0;
 	//printf("Start_IP \t End_IP \tCountry_Code\n");
	printf("Started \n"); 
        while (fgets(buffer,1024, fp))
        {
            column = 0;
            row++;
  
            char* value = strtok(buffer, ";");
  
            while (value) {
		if(column == 0)
                {
                        printf("\n Start_IP : %s  ",value);
                }
                if(column == 1)
                {
                        printf(" End IP : %s ",value);
                }
                if(column == 2)
                {
                        printf(" Country code  : %s   ",value);
                }
                column++;
		value = strtok(NULL, ";");
            }
  
        }
        fclose(fp);
    }
}
int main(int argc,char *argv[])
{
	clock_t t;
	double time_taken;
	t = clock();
	read_csv(argv[1]);	
        t = clock() - t;
        time_taken = ((double)t)/CLOCKS_PER_SEC;
        printf("read_csv() took %f seconds to execute \n", time_taken);
	return 0;
}

