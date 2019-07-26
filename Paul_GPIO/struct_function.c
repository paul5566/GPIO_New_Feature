#include <stdio.h>


struct data	{
	char name[16];
	int ID;
};

static void print_struct(struct data print_data){
	printf("the name is %s\n",print_data.name);
	printf("the ID is %d\n",print_data.ID);
}

static void print_struct_ptr(struct data *data_ptr){
	printf("the name is %s\n",data_ptr->name);
	printf("the ID is %d\n",data_ptr->ID);
}


int main(void)
{
	struct data JOHN = {
		.name = "John",
		.ID = 1234
	};
	struct data *ptr = &JOHN;
	print_struct(JOHN);
	print_struct_ptr(ptr);
	return 0;
}
