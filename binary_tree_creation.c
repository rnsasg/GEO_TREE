#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>


#define PREFIX_MATCH 1
#define NOT_PREFIX_MATCH 0
#define MAX_TREE_ROOT 2
#define NO_EDGE -1
#define LEFT_EDGE 0
#define RIGHT_EDGE 1

#define NumMiliSecInSec        1000
#define NumUsecInMiliSec       1000

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
}__attribute__((packed)) de_data_record_list_t;

typedef struct _de_tree_node_s
{
	struct _de_tree_node_s *left;
	struct _de_tree_node_s *right;
        // node_bit will have value either '0' or '1'
        unsigned int node_bit;// : 1;
        // node marker will indicate that a prefix match is available at this node
        unsigned int node_marker;//: 1;
        de_data_record_list_t *DR_List;
}__attribute__((packed)) de_tree_node_t;

//de_tree_node_t *root[MAX_TREE_ROOT];
de_tree_node_t  *root=NULL;

de_data_record_list_t *newDRList(void)
{
	de_data_record_list_t *newNode=(de_data_record_list_t*)malloc(sizeof(de_data_record_list_t));
	if(newNode == NULL)
	{
		printf("Error in malloc \n");
		return NULL;
	}
	memset(newNode,0,sizeof(de_data_record_list_t));
	newNode->prefix_len=0;
	newNode->total_no_of_node=0;
	newNode->head=NULL;
	return newNode;
}

de_tree_node_t *newTreeNode(void)
{
        de_tree_node_t *newNode=(de_tree_node_t*)malloc(sizeof(de_tree_node_t));
        if(newNode == NULL )
        {
                printf("Error in malloc \n");
                return NULL;
        }
        memset(newNode,0,sizeof(de_tree_node_t));
	newNode->left = NULL;
	newNode->right = NULL;
	newNode->node_bit = NO_EDGE;
	newNode->node_marker = NOT_PREFIX_MATCH;
	newNode->DR_List = NULL; 
        return newNode;
}

de_data_row_t *newDataRCNode(void)
{
        de_data_row_t *newNode=(de_data_row_t*)malloc(sizeof(de_data_row_t));
        if(newNode == NULL )
        {
                printf("Error in malloc \n");
                return NULL;
        }
        memset(newNode,0,sizeof(de_data_row_t));
	newNode->next = NULL;
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

int insert_de_record_node(de_tree_node_t *node,unsigned int prefix_len)
{
	unsigned int start_ip=0,end_ip=0;
	char two_letter_cn_name[3],three_letter_cn_name[4];
	de_data_row_t *tmpPtr=NULL;
	de_data_row_t *newNode=NULL;
	printf("\n In %s \n",__FUNCTION__); 
	if(node->DR_List == NULL)
	{
		node->DR_List=newDRList();	
	}
	if(node->DR_List->total_no_of_node == 0)
	{
		printf("\n In %s : First Data Record Node in Tree Path creating a DR List \n",__FUNCTION__);
		node->DR_List->prefix_len = prefix_len;
		node->DR_List->total_no_of_node++;
		
		newNode=newDataRCNode();
		printf("\n######## In %d (if-block) : Enter the record values now #######\n");
		printf("\nEnter start ip :");
		scanf("%d",&start_ip);
		printf("\nEnter end ip :");
		scanf("%d",&end_ip);
		printf("\nEnter three letter country code :");
		scanf("%s",three_letter_cn_name);
		newNode->start_ip=start_ip;
		newNode->end_ip=end_ip;
		strncpy(newNode->three_letter_cn_name,three_letter_cn_name,4);
		node->DR_List->head=newNode;
	}
	else
	{
		printf("\n In %s : Not First Data Record Node in Tree Path creating a DR List \n",__FUNCTION__);
		printf("\n In %s : DR list node count is %d \n",node->DR_List->total_no_of_node);
		if(node->DR_List != NULL && node->DR_List->head != NULL)
		{
			tmpPtr=node->DR_List->head;
			while(tmpPtr->next !=NULL)
				tmpPtr=tmpPtr->next;
			newNode=newDataRCNode();
			node->DR_List->total_no_of_node++;
			printf("\n######## In %d (else-block) : Enter the record values now #######\n");
			printf("\nEnter start ip :");
               		scanf("%d",&start_ip);
                	printf("\nEnter end ip :");
                	scanf("%d",&end_ip);
                	printf("\nEnter three letter country code :");
                	scanf("%s",three_letter_cn_name);
                	newNode->start_ip=start_ip;
                	newNode->end_ip=end_ip;
                	strncpy(newNode->three_letter_cn_name,three_letter_cn_name,4);
			tmpPtr->next=newNode;
		}
		else
		{
			printf("\n In %s : Should not come here, DR List count is non-zero  but list is empty \n",__FUNCTION__);	
		}
	}
	return 1;
}

de_tree_node_t *insertIP(de_tree_node_t *root,int ipnum)
{
	int depth=0;
	de_tree_node_t *ptr=NULL;
	de_tree_node_t *last_one_ref=NULL;
	int prefix_len=0;
	int last_prefix_len=0;
	int ret=0;
	if(!root)
	{
		printf("root is not avialable , allocating root tree node \n");
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
	ret=insert_de_record_node(last_one_ref,prefix_len);	
	printf("\n Return value from function insert_de_record_node is %d \n",ret);	
	return root;
}
void searchRecord(de_tree_node_t *node,int ipnum)
{
	int count=1;
	de_data_row_t *tmpPtr=NULL;
	if(node->node_marker == PREFIX_MATCH)
	{
                if(node != NULL && node->DR_List != NULL && node->DR_List->head !=NULL){}
       		{
			printf("\n In %s : DR list node count is %d \n",__FUNCTION__,node->DR_List->total_no_of_node);
                        tmpPtr=node->DR_List->head;
                        while(tmpPtr)
			{
				if(ipnum>=tmpPtr->start_ip && ipnum<=tmpPtr->end_ip)
				{
                       			printf("\n######## In %s : Record Values Are #######\n",__FUNCTION__);
                        		printf("\n Node %d :  Start ip : %d ",count,tmpPtr->start_ip);
                        		printf("\n Node %d :  End ip : %d ",count,tmpPtr->end_ip);
                        		printf("\n Node %d :  Three letter country code : %s ",count,tmpPtr->three_letter_cn_name);
					tmpPtr=tmpPtr->next;
					count++;
				}
			}
                }

	}	
}
void searchIP(de_tree_node_t *root,int ipnum)
{
	int depth=0;
        de_tree_node_t *ptr=NULL;
        de_tree_node_t *last_one_ref=NULL;
        int ret=0;
        if(!root)
        {
                printf("In %s : root is not avialable, Exiting ....\n",__FUNCTION__);
		return ;
		
        }
        ptr=root;
        for (depth = 31; depth >= 0; depth--)
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
	/* Now search through DR record list */
	if(last_one_ref)
		searchRecord(last_one_ref,ipnum);
	else
		printf("In %s : reference was not found....\n",__FUNCTION__);
		
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

void tree_operations(void)
{
        int ch=0;
	int ipnum=0;
	clock_t t;
	double time_taken;
	int i=0;
        do{
                printf(" 1. Insert an IP         :\n");
                printf(" 2. Search a IP Record    :\n");
                printf(" 3. Delete a node         :\n");
                printf(" 4. Delete tree           :\n");
		printf(" 5. Size of tree          :\n");
		printf(" 6. Print root-leaf path  :\n");
		printf(" 7. Tree Height 	  :\n");
                printf(" 8. Exit                  :\n");
                printf("Enter the option          : ");
                scanf("%d",&ch);
                switch(ch)
                {
                        case 1: printf("--------- Inserting a  node ---------------\n");
				printf("\n Enter IP  to be inserted :");
				scanf("%d",&ipnum);
				root = insertIP(root,ipnum);				
                                break;
                        case 2: printf("--------- Searching a node ---------------\n");
				printf("\n Enter the IP to be search :");
				scanf("%d",&ipnum);
				t = clock();
				searchIP(root,ipnum);
				t = clock() - t;
				time_taken = ((double)t)/CLOCKS_PER_SEC;
				printf("searchIP() took %f seconds to execute \n", time_taken);
                                break;
                        case 3: printf("--------- Deleting a node ---------------\n");
                                break;
                        case 4: printf("--------- Deleting tree ---------------\n");
                                break;
			case 5: printf("--------- Tree Size ---------------\n");
				printf("Size of tree is %d\n",tree_size(root));
				break;
			case 6: printf("--------- Root to leaf node Path print  ---------------\n");
				printPaths(root);
				break;
			case 7: printf("--------- Tree Height---------------\n");
				printf("Height of tree is %d\n",tree_height(root));
				break;
                        case 8: printf("--------- Exiting ---------------\n");
                                exit(0);
                        default :
                                printf("!!!!! Invalid Option !!!!!\n");
                }
        }while(1);
}
int main(void)
{
        tree_operations();
        return 0;
}

