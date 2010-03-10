#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main (int argc, const char * argv[]) 
{	
	int user_id=68;
	char * search_1="INSERT INTO '";
	char * search_2;
	char * search_3="' writers";
	char * search;
	printf("Seperate strings: \n%s \n %s \n %s \n %i.\n",search_1,search_2,search_3,user_id);
	search = (char *) calloc (strlen(search_1)+strlen(search_3) + 1, sizeof(char));
	sprintf(search,"%s%i%s",search_1,user_id,search_3);
	printf("Sprintf string: %s \n\n",search);
	 
	//Now convert string to integer
	search_2="13";
	user_id=atoi(search_2);
	printf("User_id converted to 13: %i\n\n",user_id);
	sprintf(search,"%s%d%s",search_1,user_id,search_3);
	printf("Sprintf string: %s \n\n",search);
	
	free(search);
	getchar();
    return 0;
}
