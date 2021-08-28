#include<stdio.h>

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
        //unsigned int node_bit: 1;
        // node marker will indicate that a prefix match is available at this node
        //unsigned int node_marker: 1;
	unsigned int start_ip[4];
	unsigned int end_ip[4];
	short country_code;
        de_data_record_list_t *DR_List;
}__attribute__((packed)) de_tree_node_t;

int main()
{
	printf(" size of tree node %ld \n",sizeof(de_tree_node_t));	
}

/*{
     printf("Size of short is %ld bytes\n",sizeof(short));
     printf("Size of int is %ld bytes\n",sizeof(int));
     printf("Size of unsigned int is %2d bytes\n",sizeof(unsigned int));
     printf("Size of long is %ld bytes\n",sizeof(long));

     printf("Size of float is %ld bytes\n",sizeof(float));
     printf("Size of double is %ld bytes\n",sizeof(double));
     printf("Size of long double is %ld bytes\n",sizeof(long double));

     printf("Size of char is %ld bytes\n",sizeof(char));
     printf("Size of void is %ld bytes\n",sizeof(void));
     return 0;
}
*/
