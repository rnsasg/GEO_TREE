#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


#define PREFIX_MATCH 1
#define NOT_PREFIX_MATCH 0
#define MAX_TREE_ROOT 2
#define NO_EDGE -1
#define LEFT_EDGE 0
#define RIGHT_EDGE 1


#define LOG_LEVEL_EMERGENCY   0
#define LOG_LEVEL_VERBOSE     1
#define LOG_LEVEL_ERR         2
#define LOG_LEVEL_INFO        3
#define LOG_LEVEL_DEBUG       4
#define LOG_LEVEL_DEBUG2      5

typedef enum _bool {
   false = 0,
   true = 1
}Bool;


Bool LogOn = false;
int log_level = 0;


#define LOG(level, fmt, args...)\
	do{\
		if( (LogOn == false) && (level == 0))\
			printf(fmt, ## args);\
		else if ((LogOn == true) && (level >= log_level ))\
			printf(fmt, ## args);\
	}while(0)							


void set_loglevel(int level)
{
	if ( (level >=LOG_LEVEL_EMERGENCY ) && (level <=LOG_LEVEL_DEBUG2 ))
	{
		log_level = level;
		LOG(LOG_LEVEL_EMERGENCY,"Log level set to level %d\n",level);
		return;
	}
	else
	{
		LOG(LOG_LEVEL_EMERGENCY,"\n !!! Invalid Log Level , Please enter between (0-4)!!! \n");
		return;
	}
}
void reset_loglevel()
{
	log_level = 0;
}
void logOn()
{
	LogOn = true;
}
void turnlogoff()
{
	LogOn = false;
}

void total_heap_memory_allocation();

unsigned int tree_node_cnt=0;
unsigned int linked_list_cnt=0;
unsigned int linked_list_node_cnt=0;

typedef struct _de_data_row_s
{
        unsigned int start_ip;
        unsigned int end_ip;
        char three_letter_cn_name[4];
        struct _de_data_row_s *next;
}__attribute__((packed)) de_data_row_t;

typedef struct _de_data_record_list_s
{
	unsigned int total_no_of_node;
	unsigned int prefix_len;
	de_data_row_t *head;
	de_data_row_t *tail;
}__attribute__((packed)) de_data_record_list_t;

typedef struct _de_tree_node_s
{
	struct _de_tree_node_s *left;
	struct _de_tree_node_s *right;
        // node_bit will have value either '0' or '1'
        unsigned int node_bit: 1;
        // node marker will indicate that a prefix match is available at this node
        unsigned int node_marker: 1;
        de_data_record_list_t *DR_List;
}__attribute__((packed)) de_tree_node_t;

//de_tree_node_t *root[MAX_TREE_ROOT];
de_tree_node_t  *root=NULL;

de_data_record_list_t *newDRList(void)
{
	de_data_record_list_t *newNode=(de_data_record_list_t*)malloc(sizeof(de_data_record_list_t));
	if(newNode == NULL)
	{
		LOG(LOG_LEVEL_EMERGENCY,"In %s:Error in malloc \n",__FUNCTION__);
		return NULL;
	}
	memset(newNode,0,sizeof(de_data_record_list_t));
	newNode->prefix_len=0;
	newNode->total_no_of_node=0;
	newNode->head=NULL;
	linked_list_cnt++;
	return newNode;
}

de_tree_node_t *newTreeNode(void)
{
        de_tree_node_t *newNode=(de_tree_node_t*)malloc(sizeof(de_tree_node_t));
        if(newNode == NULL )
        {
                LOG(LOG_LEVEL_EMERGENCY,"In %s:Error in malloc \n",__FUNCTION__);
                return NULL;
        }
        memset(newNode,0,sizeof(de_tree_node_t));
	newNode->left = NULL;
	newNode->right = NULL;
	newNode->node_bit = NO_EDGE;
	newNode->node_marker = NOT_PREFIX_MATCH;
	newNode->DR_List = NULL; 
	tree_node_cnt++;
        return newNode;
}

de_data_row_t *newDataRCNode(void)
{
        de_data_row_t *newNode=(de_data_row_t*)malloc(sizeof(de_data_row_t));
        if(newNode == NULL )
        {
                LOG(LOG_LEVEL_EMERGENCY,"%s:Error in malloc \n",__FUNCTION__);
                return NULL;
        }
        memset(newNode,0,sizeof(de_data_row_t));
	newNode->next = NULL;
	linked_list_node_cnt++;
        return newNode;
}

de_tree_node_t *alloc_tree_root_node()
{
	de_tree_node_t *tmp=newTreeNode();
	return tmp;	
}

de_tree_node_t *insertTreeNode(int data)
{
	de_tree_node_t *tmp=newTreeNode();
	tmp->node_bit=data;
        return tmp;
}

de_tree_node_t *insert_de_record_node(de_tree_node_t *node,unsigned int prefix_len,de_data_row_t record)
{
	de_data_row_t *tmpPtr=NULL;
	de_data_row_t *newNode=NULL;
	LOG(LOG_LEVEL_DEBUG,"\n In %s : Start_IP: %u \t End_IP: %u \t Country Name: %s \n",__FUNCTION__,record.start_ip,record.end_ip,record.three_letter_cn_name); 
	if(node->DR_List == NULL)
	{
		node->DR_List=newDRList();	
	}
	if(node->DR_List->total_no_of_node == 0)
	{
		LOG(LOG_LEVEL_DEBUG,"\n In %s : First Data Record Node in Tree Path creating a DR List \n",__FUNCTION__);
		node->DR_List->prefix_len = prefix_len;
		node->DR_List->total_no_of_node++;
		
		newNode=newDataRCNode();
		newNode->start_ip=record.start_ip;
		newNode->end_ip=record.end_ip;
		strncpy(newNode->three_letter_cn_name,record.three_letter_cn_name,3);
		node->DR_List->head=newNode;
		node->DR_List->tail=newNode;
	}
	else
	{
		LOG(LOG_LEVEL_DEBUG,"\n In %s : Not First Data Record Node in Tree Path creating a DR List \n",__FUNCTION__);
		LOG(LOG_LEVEL_DEBUG,"\n In %s : DR list node count is %u \n",__FUNCTION__,node->DR_List->total_no_of_node);
		if(node->DR_List != NULL && node->DR_List->head != NULL)
		{
			tmpPtr=node->DR_List->tail;
			newNode=newDataRCNode();
			node->DR_List->total_no_of_node++;
			newNode->start_ip=record.start_ip;
                	newNode->end_ip=record.end_ip;
                	strncpy(newNode->three_letter_cn_name,record.three_letter_cn_name,3);
			if(tmpPtr)
			{
				tmpPtr->next=newNode;
				node->DR_List->tail=newNode;
			}
		}
		else
		{
			LOG(LOG_LEVEL_VERBOSE,"\n In %s : Should not come here, DR List count is non-zero  but list is empty \n",__FUNCTION__);	
		}
	}
	return node;
}

de_tree_node_t *insertIP(de_tree_node_t *root,de_data_row_t record)
{
	int depth=0;
	de_tree_node_t *ptr=NULL;
	de_tree_node_t *last_one_ref=NULL;
	int prefix_len=0;
	int last_prefix_len=0;
	unsigned int ipnum=record.start_ip;
	LOG(LOG_LEVEL_DEBUG,"\n In %s : Start_IP: %u End_IP: %u Country Name: %s \n",__FUNCTION__,record.start_ip,record.end_ip,record.three_letter_cn_name);
	if(!root)
	{
		LOG(LOG_LEVEL_EMERGENCY,"\n root is not avialable , allocating root tree node \n");
		root=alloc_tree_root_node();			
	}
	ptr=root;
	for (depth = 31; depth >= 0; depth--)
	{	
		if (ipnum & (1 << depth))
	 	{
			if(ptr->right)
			{
				ptr=ptr->right;
			}
			else
			{
				ptr->right=insertTreeNode(1);
				ptr=ptr->right;
			}
			last_one_ref=ptr;
			last_prefix_len=prefix_len;
	 	}
	 	else
	 	{
			if(ptr->left)
                        {
                                ptr=ptr->left;
                        }
                        else
                        {
                                ptr->left=insertTreeNode(0);
                                ptr=ptr->left;
                        }
	
	 	}
		prefix_len++;	
	}
	/*Insert Node Record */
	last_one_ref->node_marker = PREFIX_MATCH;
	last_one_ref=insert_de_record_node(last_one_ref,prefix_len,record);	
	return root;
}
de_data_row_t *searchRecord(de_tree_node_t *node,de_data_row_t srchNode)
{
	int count=1;
	de_data_row_t *tmpPtr=NULL;
	Bool found=false;
	unsigned int ipnum = srchNode.start_ip;
	if(node->node_marker == PREFIX_MATCH)
	{
                if(node != NULL && node->DR_List != NULL && node->DR_List->head !=NULL){}
       		{
			LOG(LOG_LEVEL_INFO,"\n In %s : DR list node count is %u \n",__FUNCTION__,node->DR_List->total_no_of_node);
                        tmpPtr=node->DR_List->head;
                        while(tmpPtr)
			{
				if( (ipnum>=tmpPtr->start_ip && ipnum<=tmpPtr->end_ip) && (tmpPtr->end_ip == srchNode.end_ip) && (strncmp(srchNode.three_letter_cn_name,tmpPtr->three_letter_cn_name,3)==0))
				{
#if 0
                       			LOG(LOG_LEVEL_EMERGENCY,"\n######## In %s : Record Values Are #######\n",__FUNCTION__);
                        		LOG(LOG_LEVEL_EMERGENCY,"\n Node %d :  Start ip : %u %u",count,tmpPtr->start_ip,srchNode.start_ip);
                        		LOG(LOG_LEVEL_EMERGENCY,"\n Node %d :  End ip : %u %u",count,tmpPtr->end_ip,srchNode.end_ip);
                        		LOG(LOG_LEVEL_EMERGENCY,"\n Node %d :  Three letter country code : %s %s",count,tmpPtr->three_letter_cn_name,srchNode.three_letter_cn_name); 
#endif 
						found=true;
						break;
				}
				count++;
				tmpPtr=tmpPtr->next;
			}
                }

	}
	if(found == false )
	{
		return NULL;
	}
	else
	{
		return tmpPtr;
	}
}
de_data_row_t *searchIP(de_tree_node_t *root,de_data_row_t srchNode)
{
	int depth=0;
        de_tree_node_t *ptr=NULL;
        de_tree_node_t *last_one_ref=NULL;
	unsigned int ipnum=srchNode.start_ip;
        if(!root)
        {
                LOG(LOG_LEVEL_EMERGENCY,"In %s : root is not avialable, Exiting ....\n",__FUNCTION__);
		return NULL;
		
        }
        ptr=root;
        for (depth = 31; depth >= 0; depth--)
        {
		if(ptr)
		{
                	if (ipnum & (1 << depth))
                	{
                        	if(ptr->right)
                        	{
                                	ptr=ptr->right;
					if(ptr->node_marker == PREFIX_MATCH)
					{
                        			last_one_ref=ptr;
					}
				}
                	}
                	else
                	{
                        	if(ptr->left)
                        	{
                                	ptr=ptr->left;
                        	}

                	}
		}
		else
		{
			LOG(LOG_LEVEL_EMERGENCY,"In %s :IP is not in the tree\n",__func__);
			break;
			
		}
        }
	/* Now search through DR record list */
	if(last_one_ref)
		return searchRecord(last_one_ref,srchNode);
	else
	{
		LOG(LOG_LEVEL_EMERGENCY,"In %s : reference was not found....\n",__FUNCTION__);
		return NULL;
	}
		
}

int tree_size(de_tree_node_t *root)
{
        if(!root)
                return 0;
        return tree_size(root->left) + 1 + tree_size(root->right);
}

int tree_height(de_tree_node_t *root)
{
	if(!root)	
		return 0;
	else
	{
		int lDepth = tree_height(root->left);
		int rDepth = tree_height(root->right);
	

		if (lDepth > rDepth)
            		return(lDepth + 1);
        	else return(rDepth + 1);
	}
}

void printArray(int ints[], int len) 
{
  int i;
  for (i=1; i<len; i++) 
  {
    printf("%d", ints[i]);
  }
  printf("\n");
}    

void printPathsRecur(de_tree_node_t* node, int path[], int pathLen) 
{
  if (node==NULL) 
    return;
  
  path[pathLen] = node->node_bit;
  pathLen++;
  
  if (node->left==NULL && node->right==NULL) 
  {
    printArray(path, pathLen);
  }
  else 
  {
    printPathsRecur(node->left, path, pathLen);
    printPathsRecur(node->right, path, pathLen);
  }
}

void printPaths(de_tree_node_t* node) 
{
  int path[1000];
  printPathsRecur(node, path, 0);
}

de_tree_node_t *build_ds_by_reading_csv(de_tree_node_t *root,const char *filename)
{
    FILE* fp = fopen(filename, "r");
    unsigned int insertIPCnt=0;
    if (!fp)
    {
	LOG(LOG_LEVEL_EMERGENCY,"IN %s: Can't open file\n",__func__);
    }
    else
    {
	char buffer[1024];
	unsigned int row = 0;
	int column = 0;
	de_data_row_t temp;
	Bool flagInsert=true;
	insertIPCnt=0;
	while (fgets(buffer,1024, fp))
	{
	    column = 0;
	    row++;
	    char* value = strtok(buffer, ";");
	    flagInsert=true;
	    LOG(LOG_LEVEL_VERBOSE,"Row :- %u  ",row);
	    while (value) 
	    {
		if(column == 0)
		{
			temp.start_ip=atoi(value);
			LOG(LOG_LEVEL_VERBOSE,"Start_IP : %u  ",temp.start_ip);
			if(temp.start_ip == 0)
			{
				flagInsert=false;
				break;
			}
		}
		if(column == 1)
		{
			temp.end_ip=atoi(value);
			LOG(LOG_LEVEL_VERBOSE,"End IP : %u   ",temp.end_ip);
			if(temp.end_ip == 0)
			{
				flagInsert=false;
				break;
			}
		}
		if(column == 2)
		{
			strncpy(temp.three_letter_cn_name,value,sizeof(value));
			LOG(LOG_LEVEL_VERBOSE,"Country code : %s   ",temp.three_letter_cn_name);
			if(temp.three_letter_cn_name == 0)
			{
				flagInsert=false;
				break;
			}
		}
		column++;
		value = strtok(NULL, ";");
	    }
	    LOG(LOG_LEVEL_VERBOSE,"\n");
	    if( flagInsert == true )
	    {
	    	root=insertIP(root,temp);
		insertIPCnt++;
	    }
	}
	fclose(fp);
    }
    LOG(LOG_LEVEL_EMERGENCY,"\nIn %s: Total no of row inserted is %u",__func__,insertIPCnt);
    return root;
}
de_data_record_list_t *delete_linked_list(de_data_record_list_t *list_head)
{
    de_data_row_t* next=NULL;
    de_data_row_t* current=NULL;

    if(list_head)
    {
	if( list_head && ( list_head->total_no_of_node > 0 ))
	{
	    current=list_head->head;
	    if( current != NULL )
	    {
		next = current->next;
		free(current);
		current=next;		
	    }
	}

	free(list_head);
	list_head=NULL;
	tree_node_cnt=0;
	linked_list_cnt=0;
	linked_list_node_cnt=0;
    }
    return list_head;
}
de_tree_node_t *delete_tree_ds(de_tree_node_t *root)
{	
	if( root == NULL )
	{
		return NULL;
	}
	
	root->left = delete_tree_ds(root->left);
	root->right = delete_tree_ds(root->right);

	if(root->DR_List != NULL )
	{
		root->DR_List = delete_linked_list(root->DR_List);
		if( root->DR_List == NULL)	
		{
			LOG(LOG_LEVEL_INFO,"\n In %s : Tree Node List node has been deleted\n",__func__);
		}
		else
		{
			LOG(LOG_LEVEL_INFO,"\nIn %s : Error in tree node list deletion \n",__func__);
		}
	}
	free(root);
	root=NULL;
	return root;
}

void log_menu()
{
	int ch=0;
	int level=0;
	do{
		LOG(LOG_LEVEL_EMERGENCY,"\n\n\n ######################### Log Menu ################# :\n");
		LOG(LOG_LEVEL_EMERGENCY," 1. Current Log Config :\n");
		LOG(LOG_LEVEL_EMERGENCY," 2. Do Log On	       :\n");
		LOG(LOG_LEVEL_EMERGENCY," 3. Do Log Off	       :\n");
		LOG(LOG_LEVEL_EMERGENCY," 4. Set log level      :\n");
		LOG(LOG_LEVEL_EMERGENCY," 5. Reset log level    :\n");
		LOG(LOG_LEVEL_EMERGENCY," 6. Exit 	       :\n");
		LOG(LOG_LEVEL_EMERGENCY,"Enter the option       : ");
		scanf("%d",&ch);
		switch(ch)
		{
			case 1:
				LOG(LOG_LEVEL_EMERGENCY,"\n\t Log Config -->  %s \n",((LogOn == true) ? "Enable" : "Disable"));
				LOG(LOG_LEVEL_EMERGENCY,"\n\t Log Level  -->  %d \n",log_level);
				break;
			case 2:	logOn(); 
				break;
			case 3: turnlogoff();
				break;
			case 4: LOG(LOG_LEVEL_EMERGENCY,"\n Enter Log level        : ");
				scanf("%d",&level);
				set_loglevel(level);
				LOG(LOG_LEVEL_EMERGENCY,"Out");
				break;
			case 5: reset_loglevel();
				break;
			case 6:	return;
			default:
				LOG(LOG_LEVEL_EMERGENCY,"\n !!!! Invalid Option !!!!\n");
			 	break;
		}
	}while(1);
}

void total_heap_memory_allocation()
{
	long long int  size=0;
	LOG(LOG_LEVEL_EMERGENCY,"\n ########################### Total Dynamic Memory Allocation ######################## \n");

	size = tree_node_cnt * sizeof(de_tree_node_t);
	LOG(LOG_LEVEL_EMERGENCY,"\n Total Tree Node : %u \n",tree_node_cnt);
	LOG(LOG_LEVEL_EMERGENCY,"\n Tree Node size (Bytes) : %lu \n",sizeof(de_tree_node_t));
	if( size != 0 )
	{
		size = ( size / 1024 ) / 1024 ;
		printf("\n Total tree size(MB) %llu \n",size);
	}

	size = linked_list_cnt * sizeof(de_data_record_list_t);
	LOG(LOG_LEVEL_EMERGENCY,"\n Total Linked List : %u \n",linked_list_cnt);
	LOG(LOG_LEVEL_EMERGENCY,"\n Linked List Size (Bytes): %lu \n",sizeof(de_data_record_list_t));
	if( size != 0 )
	{
		size = ( size / 1024 ) / 1024 ;
		LOG(LOG_LEVEL_EMERGENCY,"\n Total Linked List size(MB) %llu \n",size);
	}

	size = linked_list_node_cnt * sizeof(de_data_row_t);
	LOG(LOG_LEVEL_EMERGENCY,"\n Total Linked List Nodes : %u \n",linked_list_node_cnt);	
	LOG(LOG_LEVEL_EMERGENCY,"\n Linked List Node Size (Bytes) : %lu \n",sizeof(de_data_row_t));
	if( size != 0 )
        {
                size = ( size / 1024 ) / 1024 ;
		LOG(LOG_LEVEL_EMERGENCY,"\n Total Linked List Nodes size(MB) %llu \n",size);
	}
	
	size = tree_node_cnt * sizeof(de_tree_node_t) + linked_list_cnt * sizeof(de_data_record_list_t) + linked_list_node_cnt * sizeof(de_data_row_t);
	LOG(LOG_LEVEL_EMERGENCY,"\n Total DS Dyanamic memory allocation in Bytes (%llu)\n",size);	
		
}

de_tree_node_t *read_csv_and_search(de_tree_node_t *root,const char* filename)
{
    clock_t t;
    double time_taken;
    t = clock();
    LOG(LOG_LEVEL_EMERGENCY,"\n Filename is %s",filename);
    root = build_ds_by_reading_csv( root , filename);	

    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    LOG(LOG_LEVEL_EMERGENCY,"\n build_ds_by_reading_csv() took %f seconds to execute \n", time_taken);
	
    FILE* fp = fopen(filename, "r");
    unsigned int srchFndCnt = 0;
    unsigned int srchNotFndCnt = 0;

    if (!fp)
    {
	LOG(LOG_LEVEL_EMERGENCY,"IN %s: Can't open file\n",__func__);
    }
    else
    {
	char buffer[1024];
	unsigned int row = 0;
	int column = 0;
	de_data_row_t temp;
	de_data_row_t *result = NULL;
	Bool flagSearch=true;
	while (fgets(buffer,1024, fp))
	{
	    column = 0;
	    row++;
	    char* value = strtok(buffer, ";");
	    flagSearch=true;
	    LOG(LOG_LEVEL_VERBOSE,"Row :- %u  ",row);
	    result = NULL;
	    while (value)
	    {
		if(column == 0)
		{
		    temp.start_ip=atoi(value);
		    LOG(LOG_LEVEL_VERBOSE,"Start_IP : %u  ",temp.start_ip);
		    if(temp.start_ip == 0)
		    {
			flagSearch=false;
			break;
		    }
		}
		if(column == 1)
		{
		    temp.end_ip=atoi(value);
		    LOG(LOG_LEVEL_VERBOSE,"End IP : %u   ",temp.end_ip);
		    if(temp.end_ip == 0)
		    {
			flagSearch=false;
			break;
		    }
		}
		if(column == 2)
		{
		    strncpy(temp.three_letter_cn_name,value,sizeof(value));
		    LOG(LOG_LEVEL_VERBOSE,"Country code : %s   ",temp.three_letter_cn_name);
		    if(temp.three_letter_cn_name == 0)
		    {
			flagSearch=false;
			break;
		    }
		}
		column++;
		value = strtok(NULL, ";");
	    }
	    LOG(LOG_LEVEL_VERBOSE,"\n");
	    if( flagSearch == true )
	    {
		t = clock();
		result=searchIP(root,temp);
		if( result != NULL)
			srchFndCnt++;	
		else
			srchNotFndCnt++;
		t = clock() - t;
		time_taken += ((double)t);
	    }
	}
	fclose(fp);
    }
    time_taken = ((double)time_taken)/CLOCKS_PER_SEC;
    LOG(LOG_LEVEL_EMERGENCY,"\n searchIP() took %f seconds for whole csv serach\n", time_taken);
    LOG(LOG_LEVEL_EMERGENCY,"\n Total Found IP %u ",srchFndCnt);
    LOG(LOG_LEVEL_EMERGENCY,"\n Total Not Found IP %u \n",srchNotFndCnt);
    return root;
}
//int main(int argc, char *argv[])
int main(int argc, char **argv)
{	
	clock_t t;
	double time_taken;
	
	
	if( argc == 1)
	{
		LOG(LOG_LEVEL_EMERGENCY,"\n Enter filename for input \n");
		return 0;
	}
	else if ( argc > 2 )
	{
		LOG(LOG_LEVEL_EMERGENCY,"\n Don't give more than one filename \n");
		return 0;
	}
	else
	{
		LOG(LOG_LEVEL_EMERGENCY,"\n File Argument is %s ",argv[1]);
		if( access( argv[1] , F_OK ) != 0 )
		{
			LOG(LOG_LEVEL_EMERGENCY,"\n File Does not exist \n");
			return 0;
		}		
	}
	t = clock();
	root = read_csv_and_search(root,argv[1]);
	t = clock() - t;
	time_taken = ((double)t)/CLOCKS_PER_SEC;
	LOG(LOG_LEVEL_EMERGENCY,"\n read_csv_and_search() took %f seconds to execute \n", time_taken);
	total_heap_memory_allocation();
	root = delete_tree_ds(root);
	return 0;
}

