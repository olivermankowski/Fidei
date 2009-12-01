#include <stdio.h>
#include <time.h>       //for time stamps
#include <stdlib.h>     //for random numbers
#include <string.h>     //for strings

//FILE NAMES
FILE *contact_list;
FILE *user_data;
FILE *company_data;
FILE *rating_list;

//VARIABLES
struct RL                   //rating database/list
{	int *target_id;
	int *source_id;
	float *rating;
};
struct RL rl;		//define rating database array & pointer

struct UDB             //user details database
{ 	int *user_id;
    int *connected_company_id;				
};
struct UDB udb;				//define user database array & pointer

struct CL                   //Contact list
{	int *target_id;
	int *source_id;
	int *time_stamp;
	float *raw_rating;
};
struct CL cl;		//define rating database array & pointer

struct CDB
{
	int *company_id;
};
struct CDB cdb;			//define company database pointer & pointer

struct SINGLE
{	int size;	//inputted number of users
	int c_size;	//inputted company size	
	int i,j;	//holds for loop values (i=outer, j=inner)
	int rand_dec; //rand number from 0 to 10
	int sparse; //sets how many users a source will have
	int target_id; //temp storage whilst creating target ID's
	int source_id; //temp storage whilst creating source ID's
	int rand_date; //holds 2008-2009 dates in seconds
	int rand_rating; //1 to 10
	int current_id;
	int old_id;
	float rand_rating_2; //0 to 5000
    int rand_link_level; //0 to 5
    int rand_sources_rating; //0 to 100
    int list_length;
    int location; //holds i * j memory location
    int req_mem; //holds the total memory allocation size (size * sparse)
    float rating; 
    int count; //holds a count value
    char buf [50];    
    int comp_id; //holds a random company id number
};
struct SINGLE single;

void *memset( void *buffer, int ch, size_t count );

//When defining strings, the first number denotes the number of rows, the 2nd the number of charecters per row. 
//When calling a string (i.e. in printf) the second brackets and number is ignored.

char f_names[10][20]={"John","Greg","Peter","Tom","Emma","Lucy","Susie","Jess","Edward","Michael"};
char s_names[10][20]={"Thompson","Tull","Peterson","Jones","Roberts","Carter","Christie","Simon","Hazel","Holt"};
char sectors[10][20]={"Oil","Auto","Strat","Ships","Forest","Manu","High","Aero","Law","Medical"};

//Hold variables - dynamic setup
char company_name[99999999][ 20 ];
char firstname[99999999][ 20 ];
char lastname[99999999][ 20 ];

void memory_control() //Function to allocate the array memory based upon the number of users (test_size)
{	printf("Allocating memory...\n");

    //Array 1 allocation
	rl.source_id = (int *) realloc (rl.source_id, single.req_mem * sizeof(int));
     if (rl.source_id == NULL)
       { puts ("Error (re)allocating Rating list source ID memory\n"); getchar();}

    //Array 2 allocation
	rl.target_id = (int *) realloc (rl.target_id, single.req_mem * sizeof(int));
     if (rl.target_id == NULL)
       { puts ("Error (re)allocating Rating list target ID memory\n"); getchar();}
       
    //Array 3 allocation
	rl.rating = (float *) realloc (rl.rating, single.req_mem * sizeof(float));
     if (rl.rating == NULL)
       { puts ("Error (re)allocating Rating list rating memory\n"); getchar();}
       
     //Array 4 allocation
	udb.user_id = (int *) realloc (udb.user_id, single.req_mem * sizeof(int));
     if (udb.user_id == NULL)
       { puts ("Error (re)allocating Rating list source ID memory\n"); getchar();}
       
    //Array 5 allocation
	udb.connected_company_id = (int *) realloc (udb.connected_company_id, single.req_mem * sizeof(int));
     if (udb.connected_company_id == NULL)
       { puts ("Error (re)allocating User database connected company id memory\n"); getchar();}

    //Array 6 allocation
	cl.raw_rating = (float *) realloc (cl.raw_rating, single.req_mem * sizeof(float));
     if (cl.raw_rating == NULL)
       { puts ("Error (re)allocating Contact list raw rating memory\n"); getchar();}

    //Array 7 allocation
	cl.source_id = (int *) realloc (cl.source_id, single.req_mem * sizeof(int));
     if (cl.source_id == NULL)
       { puts ("Error (re)allocating Contact list source id memory\n"); getchar();}
       
    //Array 8 allocation
	cl.target_id = (int *) realloc (cl.target_id, single.req_mem * sizeof(int));
     if (cl.target_id == NULL)
       { puts ("Error (re)allocating Contact list target id memory\n"); getchar();}
       
     //Array 9 allocation
	cl.time_stamp = (int *) realloc (cl.time_stamp, single.req_mem * sizeof(int));
     if (cl.time_stamp == NULL)
       { puts ("Error (re)allocating Contact list time stamp memory\n"); getchar();}

     //Array 10 allocation
	cdb.company_id = (int *) realloc (cdb.company_id, single.req_mem * sizeof(int));
     if (cdb.company_id == NULL)
       { puts ("Error (re)allocating Company database company id memory\n"); getchar();}

       printf("Memory allocated.\n");
}//end memory control function

void set_memory()
 {
 printf("Setting memory...\n");
 memset (rl.source_id, '\0', sizeof(rl.source_id));
 memset (rl.target_id, '\0', sizeof(rl.target_id));
 memset (rl.rating, '\0', sizeof(rl.rating));
 memset (udb.user_id, '\0', sizeof(udb.user_id));
 memset (udb.connected_company_id, '\0', sizeof(udb.connected_company_id));
 memset (cl.raw_rating, '\0', sizeof(cl.raw_rating));
 memset (cl.source_id, '\0', sizeof(cl.source_id));
 memset (cl.target_id, '\0', sizeof(cl.target_id));
 memset (cl.time_stamp, '\0', sizeof(cl.time_stamp));
 memset (cdb.company_id, '\0', sizeof(cdb.company_id));
 printf("Memory set.\n");
 }//end set memory function

void delete_files()
{
	 remove ("rating_list.fidei");
	 remove ("user_data.fidei");
	 remove ("company_data.fidei");
	 remove ("contact_list.fidei");
 }//end delete files function

 //Functions
void random ()
{	single.rand_dec = ( rand() % 9);	//0 to 9
	sprintf (single.buf, "%i", single.rand_dec); //converts the integer into a charecter    
    single.rand_date = ( rand() % 3600); //any time over a year
    single.rand_date = (single.rand_date*8760)+(38*31536000); //adds 38 years to time  
    single.rand_rating =  ( rand() % 9);
    single.rand_rating_2 = ( rand() % 5000);  //number rating from 0000 to 5000   
    single.rand_rating=single.rand_rating+1; //so rand rating runs from 1-10
    single.rand_link_level = ( rand() % 5);
    single.rand_sources_rating = ( rand() % 99);   
    single.comp_id=( rand() % single.c_size);                                      
}// end random number

void generate_contact_list()
{
     printf("Generating contact list...\n");
         single.target_id=0; //intitalise  
         single.location=0;  //intitalise       
         for (single.i=0;single.i<single.size;single.i++)
         {
         single.source_id=0;  //resets for next target id
         random();
         single.target_id=single.target_id+single.rand_dec;
             for (single.j=0;single.j<single.sparse;single.j++)
             {
              random();
              single.location=single.i*single.j;
              single.source_id=single.source_id+single.rand_dec;
              
              cl.target_id[single.location]=single.target_id;
              cl.source_id[single.location]=single.source_id;
              cl.time_stamp[single.location]=single.rand_date;
              
              //A raw rating includes link level, user's current rating and value given. 
              cl.raw_rating[single.location]=(0.2*single.rand_rating)+(0.2*single.rand_link_level)+(0.1*single.rand_sources_rating);
              //NEED TO INSERT PROPER FUNCTIONS INTO HERE
              
             }//end inner for loop
         }//end outer for loop
         printf("Contact list generated.\n");
}//end generate contact list function

void create_rating_list()
{   printf("Creating 'rating list'...\n");
    single.target_id=0; single.source_id=0; single.rating=0;
    random();
    if ((rating_list = fopen("rating_list.fidei", "w")) == NULL)
           	{
			printf("***ERROR*** - Cannot open rating list.\n");
			system("pause");
			}//end if 
		else
			{
             for (single.i=0; single.i<single.size;single.i++)
                {  random();
                 single.target_id=single.target_id+single.rand_dec;
                 for (single.j=0;single.j<single.sparse;single.j++)
                     { random();
                       single.source_id=single.source_id+single.rand_dec;
                       single.rating=(single.rand_rating_2/1000);
                       fprintf(rating_list, "%i %i %1.4f\n",single.target_id, single.source_id, single.rating);
                     };//end inner for loop
                     single.source_id=0;

                    //Progress Indicator     
                    single.count++;
                    if (single.count>=1000000)
                    {printf("Still writing rating list...\n");
                    single.count=0;
                    }
                  };//end outer for loop
             };//end else loop
fclose(rating_list);
single.count=0;
};//end create rating list

void generate_companies()
{ printf("Generating companies...\n");
  
  for (single.i=0; single.i<single.c_size; single.i++)
  {random();
   
   cdb.company_id[single.i]=single.i;
   company_name[single.i][19]=sectors[single.rand_dec][19];

  }//end for loop
  
  printf("Companies generated.\n");
 }//end generate companies function

void generate_users ()
{	
	printf("Generating users data...\n");
	
	//Initialise
	single.count=0;
	
	for(single.i=0;single.i<single.size;single.i++)
	{
	single.j=(single.i-1);
	
	single.current_id=cl.target_id[single.i];
	if (single.i==0)
	{single.old_id=cl.target_id[0];}
	else
    {single.old_id=cl.target_id[single.j];}
    
    	if (single.current_id!=single.old_id)
    	{random();
    	 
         udb.user_id[single.count]=cl.target_id[single.i];
         udb.connected_company_id[single.count]=single.comp_id;
         printf("enter if loop...\n");
         firstname[single.count][20]=sectors[single.rand_dec][20
         
         ];
         
         //firstname[single.count][20]=f_names[single.rand_dec][20];
    	 //lastname[single.count][19]=s_names[single.rand_dec][19];
         printf("user names saved to row: %i\n",single.count);
         single.count++;
         getchar();
        }  
	}//end for loop
	
	printf("Users generated...\n");
};//end generate users function

void save_data ()	//function to save generated data
{
    printf("Writing contact list...\n");
     if ((contact_list = fopen("contact_list.fidei", "w")) == NULL)
           	{
			printf("***ERROR*** - Cannot open contact list file for writing.\n");
			system("pause");
			return;
            }//end if 
		else
		{  for (single.i=0;single.i<single.req_mem;single.i++)
				{
					fprintf(contact_list, "%i, %i, %i, %2.8f\n",cl.target_id[single.i],cl.source_id[single.i],cl.time_stamp[single.i],cl.raw_rating[single.i]);
            }//end for loop	
     }//end else
    fclose(contact_list);

    printf("Writing company data...\n");
     if ((company_data = fopen("company_data.fidei", "w")) == NULL)
           	{
			printf("***ERROR*** - Cannot open company data file for writing.\n");
			system("pause");
			return;
            }//end if 
		else
		{  for (single.i=0;single.i<single.c_size;single.i++)
				{
					fprintf(company_data, "%i, %s\n",cdb.company_id[single.i],company_name[single.i][19]);
            }//end for loop	
     }//end else
    fclose(company_data);

    printf("Writing user data...\n");
     if ((user_data = fopen("user_data.fidei", "w")) == NULL)
           	{
			printf("***ERROR*** - Cannot open user data file for writing.\n");
			system("pause");
			return;
            }//end if 
		else
		{  printf("First iteration...\n");
            for (single.i=0;single.i<single.size;single.i++)
				{fprintf(user_data, "%i, %s, %s, %i\n",udb.user_id[single.i],firstname[single.i][19],lastname[single.i][19],
                                                       udb.connected_company_id);
                }//end for loop	
        }//end else loop
     fclose(user_data);     
    printf("All data saved.\n\n");

};//end save data function

void free_memory()
{ 
 printf("Freeing memory...\n");
 free (rl.source_id);
 free (rl.target_id);
 free (rl.rating);
 free (udb.user_id);
 free (udb.connected_company_id);
 free (cl.raw_rating);
 free (cl.source_id);
 free (cl.target_id);
 free (cl.time_stamp);
 free (cdb.company_id);
 printf("Memory freed.\n");
}//end free memory function

int main()
{ 
srand(time(NULL));

  		   delete_files();	
		   printf("Data Generator Program\n"
				 "----------------------\n\n"
				 "Please enter number of users to be generated:\n\n");
			scanf("%i",&single.size);	 
			printf("Please enter number of companies to be generated\n\n");
			scanf("%i",&single.c_size);
            printf("Please enter sparsity level (1=few connections, 10=loads):\n\n");
			scanf("%i",&single.sparse);
			
			if (single.sparse<1)
            {single.sparse=1;}
			if (single.sparse>10)
            {single.sparse=10;}
			single.sparse=single.sparse*10;
			single.list_length=single.sparse*single.size;
			single.req_mem=single.sparse*single.size;
			
			memory_control();
			set_memory();
            generate_contact_list();
            create_rating_list();		
			generate_companies();
            generate_users();
            getchar();
			save_data();
			getchar();
			free_memory();
			
			return(0);
}; //end main
