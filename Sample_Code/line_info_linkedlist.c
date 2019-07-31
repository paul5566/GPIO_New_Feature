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

struct node *find_tail(struct node *head){
	struct node *current;

	current = head;
	while(current->next != NULL)
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
void create_new_node(struct node *head,int val){

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


void print_all(struct node *head){
	struct node *current;
	current = head;
	int i = 1;
	while(current != NULL)//current->value != NULL the last point would not print out
	{
		printf("the %d of the chip_number is %d\n",i, current->gpio_chip_no);
		current = current->next;
		i++;
	}
}


int main(void)
{
	/*
	   Build up the head node
	*/
	struct node *head;//declare a pointer point to the type struct
	head = malloc(sizeof(struct node));//memory allocate a segment

	if (head == NULL)
	{
		printf("the memory allocation failed\n");
	}

	head->gpio_chip_no = 0;//Set gpiochip0 as the first chip
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

	tail->gpio_chip_no = 1;
	head->next = tail;//create the link between the head and tail
	tail->next = NULL;
	create_new_node(head,2);
	create_new_node(head,3);
	create_new_node(head,4);
	print_all(head);
	return 0;
}
