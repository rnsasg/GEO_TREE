#include<stdio.h>

int main(void)
{
	int start_ip,end_ip;
	char cn_name[4];
	
	scanf("%d;%d;%s",&start_ip,&end_ip,cn_name);
	printf("\nStart IP : %d \n End IP : %d \n Country Name : %s \n ",start_ip,end_ip,cn_name);
	return 0;
}
