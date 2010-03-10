#include <stdio.h>
#include <stdlib.h>

typedef int (*compfn)(const void*, const void*);

struct ratings 
{ int   user_id;
  int   source_id;
  float rating;
};

int array_size=11;

struct ratings array [11]  = 
{ 
	{  10, 2, 10.2  },
	{  10, 1, 10.1  },
	{  8, 6, 8.6    },
	{  8, 4, 8.4    },
	{  6, 12, 6.12  },
	{  6, 1, 6.1    },
	{  4, 11, 4.11  },
	{  4, 2, 4.2    },
	{  4, 4, 4.4    },
	{  2, 9, 2.9    },
	{  2, 6, 2.6    },
};//end array insertion

void printarray(void);
int  compare(struct ratings *, struct ratings *);

//Comparison_1 function
int compare(struct ratings *elem1, struct ratings *elem2)
{
	if ( elem1->user_id < elem2->user_id)
		return -1;
	
	else if (elem1->user_id > elem2->user_id)
		return 1;
	
	else
		return 0;
}

//Comparison_2 function
int compare_2(struct ratings *elem1, struct ratings *elem2)
{	//Need to add limits so it only compares whilst user_id stays the same
	if ( elem1->source_id < elem2->source_id)
		return -1;
	
	else if (elem1->source_id > elem2->source_id)
		return 1;
	
	else
		return 0;
}

//Prints array
void printarray(void)
{	int i;
	for (i = 0; i < 10; i++)
		printf("%d:  %d - %d - %1.5f \n",
               i+1, array[i].user_id, array[i].source_id, array[i].rating);
}

int main (int argc, const char * argv[]) 
{   printf("List before sorting:\n");
	printarray();
	printf("Sorting by user_id (elem1)\n");
	
	//Compare function 1
	qsort((void *) &array,                    // Beginning address of array
		  array_size,                         // Number of elements in array
		  sizeof(struct ratings),             // Size of each element
		  (compfn)compare );                  // Pointer to compare function
	
	
	printarray();
	printf("Sorting source_id (elem2)\n");
	
	//Compare function 2
	//Find first address of element to order & number of rows affected
	
	int i,j=0;
	int length=0;
	int start=0;
	int end=0; 
	
	for (i=0;i<array_size;i++)
	{	if(array[i].user_id!=array[j].user_id)
			{
			
			start=j;
			end=i;
			length = end - start;	
			//printf("Length found, sorting (start %i, end %i, length %i.)\n",start,end,length);
			//	printf("Location of first element is %i.\n",&array[start].user_id);
			
			//Perform Sort
			qsort((void *) (
					&array[start].user_id),		// Beginning address of array
				    length,                             // Number of elements in array
				    sizeof(struct ratings),             // Size of each element
					(compfn)compare_2);                 // Pointer to compare function	
				
			j=i;
			}//end if loop
		end++;
		
		//printf("I: %d, J: %d.\n",i,j);	
		//getchar();	
		
	}//end while loop
	
	printarray();
    getchar();
}