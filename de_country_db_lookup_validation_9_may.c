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

FILE *debugFilefp = NULL;
Bool LogOn = false;
int log_level = 0;


#define LOG(level, fmt, args...)\
	do{\
		if( (LogOn == false) && (level == 0))\
			printf(fmt, ## args);\
		else if ((LogOn == true) && (level >= log_level ))\
			printf(fmt, ## args);\
	}while(0)							

#define MAX_GEO_LOCS            253

static const char GeoIP_country_code[MAX_GEO_LOCS][3] = {
    "--",
    "AP","EU","AD","AE","AF","AG","AI","AL","AM","AN",
    "AO","AQ","AR","AS","AT","AU","AW","AZ","BA","BB",
    "BD","BE","BF","BG","BH","BI","BJ","BM","BN","BO",
    "BR","BS","BT","BV","BW","BY","BZ","CA","CC","CD",
    "CF","CG","CH","CI","CK","CL","CM","CN","CO","CR",
    "CU","CV","CX","CY","CZ","DE","DJ","DK","DM","DO",
    "DZ","EC","EE","EG","EH","ER","ES","ET","FI","FJ",
    "FK","FM","FO","FR","FX","GA","GB","GD","GE","GF",
    "GH","GI","GL","GM","GN","GP","GQ","GR","GS","GT",
    "GU","GW","GY","HK","HM","HN","HR","HT","HU","ID",
    "IE","IL","IN","IO","IQ","IR","IS","IT","JM","JO",
    "JP","KE","KG","KH","KI","KM","KN","KP","KR","KW",
    "KY","KZ","LA","LB","LC","LI","LK","LR","LS","LT",
    "LU","LV","LY","MA","MC","MD","MG","MH","MK","ML",
    "MM","MN","MO","MP","MQ","MR","MS","MT","MU","MV",
    "MW","MX","MY","MZ","NA","NC","NE","NF","NG","NI",
    "NL","NO","NP","NR","NU","NZ","OM","PA","PE","PF",
    "PG","PH","PK","PL","PM","PN","PR","PS","PT","PW",
    "PY","QA","RE","RO","RU","RW","SA","SB","SC","SD",
    "SE","SG","SH","SI","SJ","SK","SL","SM","SN","SO",
    "SR","ST","SV","SY","SZ","TC","TD","TF","TG","TH",
    "TJ","TK","TM","TN","TO","TL","TR","TT","TV","TW",
    "TZ","UA","UG","UM","US","UY","UZ","VA","VC","VE",
    "VG","VI","VN","VU","WF","WS","YE","YT","RS","ZA",
    "ZM","ME","ZW","A1","A2","O1","AX","GG","IM","JE",
    "BL","MF"
};

static const char * GeoIP_country_name[MAX_GEO_LOCS] = {
    "N/A", "Asia/Pacific Region", "Europe", "Andorra",
    "United Arab Emirates", "Afghanistan", "Antigua and Barbuda",
    "Anguilla", "Albania", "Armenia", "Netherlands Antilles",
    "Angola", "Antarctica", "Argentina", "American Samoa", "Austria",
    "Australia", "Aruba", "Azerbaijan", "Bosnia and Herzegovina",
    "Barbados", "Bangladesh", "Belgium", "Burkina Faso", "Bulgaria",
    "Bahrain", "Burundi", "Benin", "Bermuda", "Brunei Darussalam",
    "Bolivia", "Brazil", "Bahamas", "Bhutan", "Bouvet Island",
    "Botswana", "Belarus", "Belize", "Canada",
    "Cocos (Keeling) Islands",
    "Congo, The Democratic Republic of the",
    "Central African Republic", "Congo", "Switzerland",
    "Cote D'Ivoire", "Cook Islands", "Chile", "Cameroon", "China",
    "Colombia", "Costa Rica", "Cuba", "Cape Verde",
    "Christmas Island", "Cyprus", "Czech Republic", "Germany",
    "Djibouti", "Denmark", "Dominica", "Dominican Republic",
    "Algeria", "Ecuador", "Estonia", "Egypt", "Western Sahara",
    "Eritrea", "Spain", "Ethiopia", "Finland","Fiji",
    "Falkland Islands (Malvinas)", "Micronesia, Federated States of",
    "Faroe Islands", "France", "France, Metropolitan", "Gabon",
    "United Kingdom", "Grenada", "Georgia", "French Guiana", "Ghana",
    "Gibraltar", "Greenland", "Gambia", "Guinea","Guadeloupe",
    "Equatorial Guinea", "Greece",
    "South Georgia and the South Sandwich Islands", "Guatemala",
    "Guam", "Guinea-Bissau", "Guyana", "Hong Kong",
    "Heard Island and McDonald Islands", "Honduras", "Croatia",
    "Haiti", "Hungary", "Indonesia", "Ireland", "Israel", "India",
    "British Indian Ocean Territory", "Iraq",
    "Iran, Islamic Republic of", "Iceland", "Italy", "Jamaica",
    "Jordan", "Japan", "Kenya", "Kyrgyzstan", "Cambodia", "Kiribati",
    "Comoros", "Saint Kitts and Nevis",
    "Korea, Democratic People's Republic of", "Korea, Republic of",
    "Kuwait", "Cayman Islands", "Kazakhstan",
    "Lao People's Democratic Republic", "Lebanon", "Saint Lucia",
    "Liechtenstein", "Sri Lanka", "Liberia", "Lesotho", "Lithuania",
    "Luxembourg", "Latvia", "Libyan Arab Jamahiriya", "Morocco",
    "Monaco", "Moldova, Republic of", "Madagascar",
    "Marshall Islands", "Macedonia", "Mali", "Myanmar", "Mongolia",
    "Macau", "Northern Mariana Islands", "Martinique", "Mauritania",
    "Montserrat", "Malta", "Mauritius", "Maldives", "Malawi",
    "Mexico", "Malaysia", "Mozambique", "Namibia", "New Caledonia",
    "Niger", "Norfolk Island", "Nigeria", "Nicaragua", "Netherlands",
 "Norway", "Nepal", "Nauru", "Niue", "New Zealand", "Oman",
    "Panama", "Peru", "French Polynesia", "Papua New Guinea",
    "Philippines", "Pakistan", "Poland", "Saint Pierre and Miquelon",
  "Pitcairn Islands", "Puerto Rico", "Palestinian Territory", "Portugal",
    "Palau", "Paraguay", "Qatar", "Reunion", "Romania",
    "Russian Federation", "Rwanda", "Saudi Arabia",
    "Solomon Islands", "Seychelles", "Sudan", "Sweden", "Singapore",
    "Saint Helena", "Slovenia", "Svalbard and Jan Mayen", "Slovakia",
    "Sierra Leone", "San Marino", "Senegal", "Somalia", "Suriname",
    "Sao Tome and Principe", "El Salvador", "Syrian Arab Republic",
    "Swaziland", "Turks and Caicos Islands", "Chad",
    "French Southern Territories", "Togo", "Thailand", "Tajikistan",
    "Tokelau", "Turkmenistan", "Tunisia", "Tonga", "Timor-Leste",
    "Turkey", "Trinidad and Tobago", "Tuvalu", "Taiwan",
    "Tanzania, United Republic of", "Ukraine", "Uganda",
    "United States Minor Outlying Islands", "United States",
    "Uruguay", "Uzbekistan", "Holy See (Vatican City State)",
    "Saint Vincent and the Grenadines", "Venezuela",
    "Virgin Islands, British", "Virgin Islands, U.S.", "Vietnam",
    "Vanuatu", "Wallis and Futuna", "Samoa", "Yemen", "Mayotte", "Serbia",
    "South Africa", "Zambia", "Montenegro", "Zimbabwe",
    "Anonymous Proxy", "Satellite Provider", "Other",
    "Aland Islands", "Guernsey", "Isle of Man","Jersey",
    "Saint Barthelemy", "Saint Martin"
};

short country_code_to_num(char *code)
{
        for(int i=1;i<MAX_GEO_LOCS;i++)
        {
                //printf("\n Country Code %s",GeoIP_country_code[i]);
                if(strncasecmp(GeoIP_country_code[i],code,3)== 0)
                                return i;
        }
        return -1;
}


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
        //char three_letter_cn_name[4];
	short country_code;
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

void printList(de_data_row_t *head)
{
    de_data_row_t *temp = head;
    while(temp != NULL)
    {
        fprintf(debugFilefp,"%d-> ", temp->start_ip);
        temp = temp->next;
    }
    printf("NULL");
}

void isSorted(de_data_row_t *head)
{
    if (head == NULL)
        return ;

    for (de_data_row_t *t=head; t->next != NULL; t=t->next)
    {
       if (t->start_ip > t->next->start_ip)
       {
                fprintf(debugFilefp,"\nList is not sorted\n");
                printList(head);
                return ;
       }
    }
    //fprintf(debugFilefp,"\nList is in sorted order %p\n",head);
    return;
}

void printInorder(de_tree_node_t *root)
{
        if(root == NULL)
                return;

        if(root->left)
                printInorder(root->left);

        if(root->DR_List!= NULL && root->DR_List->head!= NULL)
                isSorted(root->DR_List->head);
        //      fprintf(debugFilefp," head %p \n",root->DR_List->head);

        if(root->right)
                printInorder(root->right);

}

de_data_row_t *sorted_insert(de_data_row_t *head_ref,de_data_row_t *new_node,de_data_row_t **tail)
{
        de_data_row_t* current;
        if (head_ref == NULL || head_ref->start_ip >= new_node->end_ip)
        {
                new_node->next = head_ref;
                head_ref = new_node;
        }
        else
        {
                current = head_ref;
                while (current->next!=NULL && current->next->end_ip < new_node->end_ip)
                {
                        current = current->next;
                }
                new_node->next = current->next;
                current->next = new_node;
        }
        (*tail)=current;
        return head_ref;
}


de_tree_node_t *insert_de_record_node(de_tree_node_t *node,unsigned int prefix_len,de_data_row_t record)
{
	de_data_row_t *tmpPtr=NULL;
	de_data_row_t *newNode=NULL;
	LOG(LOG_LEVEL_DEBUG,"\n In %s : Start_IP: %u \t End_IP: %u \t Country Name: %s \n",__FUNCTION__,record.start_ip,record.end_ip,GeoIP_country_name[record.country_code]); 
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
		newNode->country_code = record.country_code;
		node->DR_List->head = sorted_insert(node->DR_List->head,newNode,&(node->DR_List->tail));
		//node->DR_List->head=newNode;
		//node->DR_List->tail=newNode;
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
			newNode->country_code = record.country_code;
			node->DR_List->head = sorted_insert(node->DR_List->head,newNode,&(node->DR_List->tail));
			/*if(tmpPtr)
			{
				tmpPtr->next=newNode;
				node->DR_List->tail=newNode;
			}*/
		}
		else
		{
			LOG(LOG_LEVEL_VERBOSE,"\n In %s : Should not come here, DR List count is non-zero  but list is empty \n",__FUNCTION__);	
		}
	}
	return node;
}

int common_seq_length(unsigned int start_ip, unsigned int end_ip)
{

        int depth=0;
        int common_seq_len=0;
        for (depth = 31; depth >= 0; depth--)
        {
                if ( ( start_ip  & (1 << depth)) == ( end_ip  & (1 << depth)) )
                        common_seq_len ++;
                else
                        break;
        }
	return common_seq_len;
}


de_tree_node_t *insertIP(de_tree_node_t *root,de_data_row_t record)
{
	int depth=0;
	de_tree_node_t *ptr=NULL;
	de_tree_node_t *last_one_ref=NULL;
	int prefix_len=0;
	int last_prefix_len=0;
	unsigned int ipnum=record.start_ip;
	unsigned int start_ipnum=record.start_ip;
        unsigned int end_ipnum=record.end_ip;
	int common_seq_len= common_seq_length(start_ipnum,end_ipnum);

	LOG(LOG_LEVEL_DEBUG,"\n In %s : Start_IP: %u End_IP: %u Country Name: %s \n",__FUNCTION__,record.start_ip,record.end_ip,GeoIP_country_name[record.country_code]);
	if(!root)
	{
		LOG(LOG_LEVEL_EMERGENCY,"\n root is not avialable , allocating root tree node \n");
		root=alloc_tree_root_node();			
	}
	ptr=root;
	for (depth = 31; depth >= ( 32 - common_seq_len ); depth--)
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
	ptr->node_marker = PREFIX_MATCH;
	ptr=insert_de_record_node(ptr,prefix_len,record);	
	return root;
}
Bool searchRecord(de_tree_node_t *node,de_data_row_t srchNode,unsigned int ipnum)
{
	int count=1;
	de_data_row_t *tmpPtr=NULL;
	Bool found=false;
	if(node->node_marker == PREFIX_MATCH)
	{
                if(node != NULL && node->DR_List != NULL && node->DR_List->head !=NULL){}
       		{
			LOG(LOG_LEVEL_INFO,"\n In %s : DR list node count is %u \n",__FUNCTION__,node->DR_List->total_no_of_node);
			if( node->DR_List && node->DR_List->tail )
			{
				if( node->DR_List->tail->end_ip < ipnum )
				{
					fprintf(debugFilefp,"In %s:IP(%u) is not in this current list ( tail-%u)\n",__func__,ipnum,node->DR_List->tail->end_ip);
					return false;
				}
			}
                        tmpPtr=node->DR_List->head;
                        while(tmpPtr)
			{	
				
                                LOG(LOG_LEVEL_VERBOSE,"\n In %s : Prefix Match while start loop \n",__func__);
                                LOG(LOG_LEVEL_VERBOSE,"\n Node %d :  Start ip : %u ",count,tmpPtr->start_ip);
                                LOG(LOG_LEVEL_VERBOSE,"\n Node %d :  End ip : %u ",count,tmpPtr->end_ip);
                                LOG(LOG_LEVEL_VERBOSE,"\n Node %d :  Country Code (Num): %u ",count,tmpPtr->country_code);
                                LOG(LOG_LEVEL_VERBOSE,"\n Node %d :  Three letter country code : %s, Country Full Name : %s  ",count,             GeoIP_country_code[tmpPtr->country_code],GeoIP_country_name[tmpPtr->country_code]);
                                LOG(LOG_LEVEL_VERBOSE,"\n In %s : Prefix Match while start end \n",__func__);
	
				
				if( (ipnum>=tmpPtr->start_ip && ipnum<=tmpPtr->end_ip) && (tmpPtr->country_code == srchNode.country_code))
				{
#if 0 
                       			LOG(LOG_LEVEL_EMERGENCY,"\n######## In %s : Record Values Are #######\n",__FUNCTION__);
                        		LOG(LOG_LEVEL_EMERGENCY,"\n Node %d :  Start ip : %u %u",count,tmpPtr->start_ip,srchNode.start_ip);
                        		LOG(LOG_LEVEL_EMERGENCY,"\n Node %d :  End ip : %u %u",count,tmpPtr->end_ip,srchNode.end_ip);
					LOG(LOG_LEVEL_EMERGENCY,"\n Node %d :  Country Code (Num): %u ",count,tmpPtr->country_code);
                                        LOG(LOG_LEVEL_EMERGENCY,"\n Node %d :  Three letter country code : %s, Country Full Name : %s  ",count,   GeoIP_country_code[tmpPtr->country_code],GeoIP_country_name[tmpPtr->country_code]);
#endif 
						found=true;
						break;
				}
				count++;
				tmpPtr=tmpPtr->next;
			}
                }

	}
	return found;
}
Bool searchIP(de_tree_node_t *root,de_data_row_t srchNode,unsigned int ipnumtmp)
{
	int depth=0;
        de_tree_node_t *ptr=NULL;
	Bool found=false;
	//unsigned int ipnum = srchNode.start_ip;
	unsigned int ipnum = ipnumtmp;
        if(!root)
        {
                LOG(LOG_LEVEL_EMERGENCY,"In %s : root is not avialable, Exiting ....\n",__FUNCTION__);
		return false;
		
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
					LOG(LOG_LEVEL_VERBOSE,"In %s : Right Branch %d\n",__func__,depth);
                                	ptr=ptr->right;
				}
                	}
                	else
                	{
                        	if(ptr->left)
                        	{
					LOG(LOG_LEVEL_VERBOSE,"In %s : Left Branch %d\n",__func__,depth);
                                	ptr=ptr->left;
                        	}

                	}
			if(ptr->node_marker == PREFIX_MATCH)
                        {
                        	LOG(LOG_LEVEL_VERBOSE,"In %s : Prefix match tree depth (%d) found %d\n",__func__,depth, found);                                                 
				found = searchRecord(ptr,srchNode,ipnum);
                                if(true == found)
                                	break;

                        }
		}
		else
		{
			LOG(LOG_LEVEL_EMERGENCY,"In %s :IP is not in the tree\n",__func__);
			break;
			
		}
        }
	return found;
/*	if(found == false )
        	LOG(LOG_LEVEL_EMERGENCY,"In %s : reference was not found....\n",__FUNCTION__);*/	
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
			/*strncpy(temp.three_letter_cn_name,value,sizeof(value));
			LOG(LOG_LEVEL_VERBOSE,"Country code : %s   ",temp.three_letter_cn_name);
			if(temp.three_letter_cn_name == 0)
			{
				flagInsert=false;
				break;
			}*/
			temp.country_code = country_code_to_num (value);
			LOG(LOG_LEVEL_VERBOSE," Country code : %s   ",value);
                        LOG(LOG_LEVEL_VERBOSE," Country code Num  : %d   ",temp.country_code);
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
    unsigned int ip_range_scanned = 0;
    t = clock();
    LOG(LOG_LEVEL_EMERGENCY,"\n Filename is %s",filename);
    root = build_ds_by_reading_csv( root , filename);	

    t = clock() - t;
    time_taken = ((double)t)/CLOCKS_PER_SEC;
    LOG(LOG_LEVEL_EMERGENCY,"\n build_ds_by_reading_csv() took %f seconds to execute \n", time_taken);
	
    FILE* fp = fopen(filename, "r");
    unsigned int srchFndCnt = 0 , cumSrchFndCnt = 0;
    unsigned int srchNotFndCnt = 0 , cumSrchNotFndCnt = 0;

    if (!fp)
    {
	LOG(LOG_LEVEL_EMERGENCY,"IN %s: Can't open file\n",__func__);
    }
    else
    {
	char buffer[1024];
	unsigned int row = 0;
	int column = 0;
	unsigned int start_ip=0;
	de_data_row_t temp;
	Bool result = false;
	Bool flagSearch=true;
	while (fgets(buffer,1024, fp))
	{
	    column = 0;
	    row++;
	    char* value = strtok(buffer, ";");
	    flagSearch=true;
	    LOG(LOG_LEVEL_VERBOSE,"Row :- %u  ",row);
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
		   /* strncpy(temp.three_letter_cn_name,value,sizeof(value));
		    LOG(LOG_LEVEL_VERBOSE,"Country code : %s   ",temp.three_letter_cn_name);
		    if(temp.three_letter_cn_name == 0)
		    {
			flagSearch=false;
			break;
		    }*/
		    temp.country_code = country_code_to_num (value);
		    LOG(LOG_LEVEL_VERBOSE," Country code : %s   ",value);
                    LOG(LOG_LEVEL_VERBOSE," Country code Num  : %d   ",temp.country_code);
		}
		column++;
		value = strtok(NULL, ";");
	    }
	    LOG(LOG_LEVEL_VERBOSE,"\n");
	    if( flagSearch == true )
	    {
		srchNotFndCnt = 0;
		srchFndCnt = 0;
		t = clock();
		for( start_ip = temp.start_ip;start_ip<= temp.end_ip ; start_ip++)
		{
			result=searchIP(root,temp,start_ip);
			if( result == true )
				srchFndCnt++;	
			else
			{
				srchNotFndCnt++;
				fprintf(debugFilefp," %u %u %u %s \n",start_ip,temp.start_ip,temp.end_ip,GeoIP_country_name[temp.country_code]);
			}
		}
		t = clock() - t;
		time_taken += ((double)t);
		ip_range_scanned++;
		if( ip_range_scanned % 10 == 0)
			printf(".");
	    }
	    cumSrchFndCnt += srchFndCnt;
	    cumSrchNotFndCnt += srchNotFndCnt;
	}
	fclose(fp);
    }
    time_taken = ((double)time_taken)/CLOCKS_PER_SEC;
    LOG(LOG_LEVEL_EMERGENCY,"\n searchIP() took %f seconds for whole csv serach\n", time_taken);
    LOG(LOG_LEVEL_EMERGENCY,"\n Total Found IP %u ",cumSrchFndCnt);
    LOG(LOG_LEVEL_EMERGENCY,"\n Total Not Found IP %u \n",cumSrchNotFndCnt);
    return root;
}
void openDebugFile(void)
{
    debugFilefp = fopen("Not_Found_DE_Record.txt", "w");
    if (!debugFilefp)
    {
        LOG(LOG_LEVEL_EMERGENCY,"IN %s: Can't open file Not_Found_DE_Record.txt \n",__func__);
    }
}

void closeDebugFile(void)
{
	if(debugFilefp != NULL)
	{
		fclose(debugFilefp);
		debugFilefp = NULL;
		LOG(LOG_LEVEL_EMERGENCY,"IN %s: Successfully closed file Not_Found_DE_Record.txt \n",__func__);
	}
	else
	{
		LOG(LOG_LEVEL_EMERGENCY,"IN %s: File Not_Found_DE_Record.txt  alreay closed \n",__func__);
		debugFilefp  = NULL;
	}
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
	openDebugFile();
	t = clock();
	root = read_csv_and_search(root,argv[1]);
	t = clock() - t;
	time_taken = ((double)t)/CLOCKS_PER_SEC;
	LOG(LOG_LEVEL_EMERGENCY,"\n read_csv_and_search() took %f seconds to execute \n", time_taken);
	total_heap_memory_allocation();
	root = delete_tree_ds(root);
	closeDebugFile();
	return 0;
}

