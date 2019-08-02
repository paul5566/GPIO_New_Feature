#include<stdio.h>
#include<stdlib.h>

/*
   build up the function 
*/
struct node {
	int gpio_chip_no;
	struct node *next;
	int gpio_line[32];
};

static struct node *find_tail(struct node *head){
	struct node *current;

	current = head;
	while(current->next != NULL)
	{
		current = current->next;
	}
	return current;
}
static struct node *find_node(struct  node *head, int target){
	struct node *current;

	current = head;
	while(current->gpio_chip_no != target)
	{
		current = current->next;
	}
	return current;
}

/*
	create new node

	*****Be careful*****
	Create Node Error Handling
*/
static void create_new_node(struct node *head,int val){

	struct node *new_node;
    struct node *current_tail;

    new_node = malloc(sizeof(struct node));
	if (head == NULL)
	{
		printf("the memory allocation failed\n");
	}
    new_node->gpio_chip_no = val;
    current_tail = find_tail(head);
    current_tail->next = new_node;
    new_node->next = NULL;
}
/*
 *	Assign data to node
 *	Tell the flag "unused" to fill
 *	line info
 */

static void fill_line_info(struct node *target){
	//Add the flag check later
	for(int i = 0; i < 32; i++)
	{
		target->gpio_line[i] = 1;
	}
}
/*
 *
 */

static void chip_line_check(struct node *head,int chip_no,int chip_line)
{
	struct node *assign_chip_info;

	assign_chip_info = find_node(head,chip_no);
	if(assign_chip_info->gpio_line[chip_line])
		printf("the gpiochip%d's %dnd line is used\n",chip_no, chip_line);
	else
		printf("the gpiochip%d's %dne line is unused\n",chip_no, chip_line);
}

/*
 * print all the node's line status
 */
static void print_all(struct node *head){
	struct node *current;
	current = head;
	int i = 0;
	while(current->next != NULL)//current->value != NULL the last point would not print out
	{
		printf("\n***the chip_number is %d***\n", current->gpio_chip_no);
		current = current->next;
		i++;
		for(int j = 0;j < 32;j++)
		{
			printf("line_info[%d] = %d\n", j, current->gpio_line[j]);
		}

	}
}


int main(void)
{
	/*
	   Build up the head node
	*/
	struct node *head;//declare a pointer point to the type struct
	struct node *assign_chip_info;

	head = malloc(sizeof(struct node));//memory allocate a segment

	if (head == NULL)
	{
		printf("the memory allocation failed\n");
	}

	head->gpio_chip_no = 4;//Set gpiochip0 as the first chip
	head->next = NULL;//assign the NULL to node head's *next which means the next would be NULL;
	/*
	   Build up the tail node
	*/
	struct node *tail;//declare the tail node
	tail = malloc(sizeof(struct node));
	if (head == NULL)
	{
		printf("the memory allocation failed\n");
	}

	tail->gpio_chip_no = 3;
	head->next = tail;//create the link between the head and tail
	tail->next = NULL;
	create_new_node(head,2);
	create_new_node(head,1);
	create_new_node(head,0);
	//find node
	assign_chip_info = find_node(head,3);
	//assign the line_info
	fill_line_info(assign_chip_info);
	//print all
	//print_all(head);
	//test line
	int gpiochip_test = 3;
	int test_line = 16;
	//check function
	chip_line_check(head,gpiochip_test,test_line);

	return 0;
}
