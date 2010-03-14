#include <stdio.h>
#include <time.h>       //for time stamps
#include <stdlib.h>     //for random numbers
#include <mysql.h>		//For mysql

//File Initialisation
FILE *rating_list;  //file to store rating list data.
FILE *centrality;   //file to store centrality results.
FILE *contactlist; //file to read in contact list
FILE *lambda;       //holds the centrality programs lambda result
FILE *ratingfactors; //holds the values that are used in the rating calculations

//Array Structure Initialisation
struct ARRAYS
{	double *eigen_vector;		//ARRAY 1
	double *eigen_vector_new;	//ARRAY 2
};
struct ARRAYS arrays;

struct RATING_FACTORS
{	float time_1month;
	float time_2month;
	float time_3month;
	float time_4month;
	float time_5month;
	float time_6month;
	float time_7month;
	float time_8month;
	float time_9month;
	float time_10month;
	float link_level1;
	float link_level2;
	float link_level3;
	float link_level4;
	float link_level5;
	float link_level6;
};
struct RATING_FACTORS rating_factors;

struct CONTACT_LIST
{	int *target_id;
	int *source_id;
	float *raw_rating;
	int *timestamp;
};
struct CONTACT_LIST contact_list;

struct SEARCH_COMPANY
{	int * company_id;
	int * user_id;
};
struct SEARCH_COMPANY search_company;

struct ADJ_LIST
{   int *target_id;
	int *source_id;
	double *rating;
};
struct ADJ_LIST adj_list;

//Singles Structure Initialisation
struct SINGLE
{
	int its; //its is the number of iterations the power method takes
	int error;//if an error occurs in a previous process, skips later processes
	int rand_num; //holds the generated random number
	int rand_dec; //0 to 10 random number
	int count; //used for progress indicators if reading length exceeds a number (org. set as 1,000,000)
    int i, j, k; //i=row; j=col, k=3d!
	int connections; //controls number of connections a user has
	int target_id;   //saving rating list target id
	int source_id;   //saving rating list source id
	int target_id_o; //previous line target id
	int source_id_o; //previous line source id
	int max_target;   //holds maximum target ID
	int min_target;   //holds minimum target ID
	int max_source;   //holds maximum source ID
	int min_source;  //holds minimum source ID 
	int largest;     //holds the largest value, whether max_target or max_source
	int current_n;   //holds the largest 'n' in the list so far whilst scanning
	int old_n;       //holds previous iteration target value
	int found_n;     //determines 'n' from rating list - max. number of target_id's
	int current_m;   //holds the largest 'm' in the list so far whilst scanning
    int found_m;     //determines 'm' from rating list - max. number of connections
    int *test_mem;   //finds max memory available
	int timestamp;   //holds the time value read in from contact list
    float max_mem;     //holds the total memory available
    float req_mem;     //holds the expected required memory for 'n' and 'm' 
    int harddrive_flag; //if =0 ram can be used, if =1 hard drive must be used
    int cumu_m;         //adds up all m's (i.e. total number of rows in rating list)
    long size;			//Controls size of matricies.matrix
	long size_sqr;		//controls list length
	float rand_rating; //holds random rating number
	float sparse;	//test to control sparsity
	float accuracy; //Determines how close An/An-1 needs to be before ending power method
	float x0_max;		//holds largest number in x0 for normalising
	float x0_old; 	//holds the largest value of x0 from the previous iteration
	float eigen_test;	//holds the test value
	float lambda; 	//holds the dominant eigen value
	float time_taken;	//Finds out the processing time for the function	
	float rating;     //For saving rating
	double x0_temp;	//holds the x0 values during processing
    int previous_target;
    int user_score;
    int memory_question;
    float rolling_rating_sum;
    float current_centrality;
    int current_target;
    int menu_select; //for switch for function switches
    int previous_source;
    int current_source;
    int current_time_stamp;
    float raw_rating;
    long time_now; //for casted time value
    float adjusted_rating;
    int time_zero;
    int company_id;
    float company_score;
    float time_correction_factor;
    int search_id; //used by user_rating algo as input variable for desired user_id
    float rating_elapsed;
    float current_rating;
    double test_time; //stores the difference between read time and the time now
    float rating_correction; //holds the time adjusted correction factor
};
struct SINGLE single;	//declare singles struct, ref by "singles.xxx"

// time_t basis: 00:00:00 on January 1, 1970, Coordinated Universal Time. (aka 'epoch') 
time_t start, end, now;	//Time variables (calc start time, end time and current time when using difftime)
void *memset( void *buffer, int ch, size_t count );
void mysql_disconnect();
void order_contact_list();
void user_reputation();
void loadin_factors();

//Mysql Variables
MYSQL *conn;
MYSQL_ROW row;
MYSQL_RES *result;
MYSQL_FIELD *field;

//Set below for local work
char *server = "localhost";
char *user = "root";
char *password = "";
char *database = "Fidei";

//Set below for remote work
//char *server = "212.110.184.15";
//char *user = "oliver@myfidei.com";
//char *password = "";
//char *database = "Fidei";


//Below functions used for the order_contact_list program
int  compare(struct CONTACT_LIST *, struct CONTACT_LIST *);
typedef int (*compfn)(const void*, const void*);
//Comparison_1 function
int compare(struct CONTACT_LIST *elem1, struct CONTACT_LIST *elem2)
{
	if ( elem1->target_id < elem2->target_id)
		return -1;
	else if (elem1->target_id > elem2->target_id)
		return 1;
	else
		return 0;
}
//Comparison_2 function
int compare_2(struct CONTACT_LIST *elem1, struct CONTACT_LIST *elem2)
{	//Need to add limits so it only compares whilst user_id stays the same
	if ( elem1->source_id < elem2->source_id)
		return -1;
	else if (elem1->source_id > elem2->source_id)
		return 1;
	else
		return 0;
}

//Functions
void mysql_connect()
{	conn = mysql_init(NULL);
		if (conn == NULL) 
		{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		return;
		}//end if
		else 
		{
		printf("Connection starting...\n");
	
		// Connect to database
		printf("Connecting to mysql and opening database %s...\n", database);
		if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) 
			{
			fprintf(stderr, "%s\n", mysql_error(conn));
			return;
			}//end if loop
		else {printf("Connected.\n");}
		
		//Get mysql client version
		printf("MySQL client version: %s\n", mysql_get_client_info());
		}//end else
}//end function

void mysql_loadin()
{//Loads data from a text file into MySQL
	if     ((rating_list = fopen("contact_list.fidei", "r")) == NULL)
	{
		printf("***ERROR*** - Cannot open rating list.\n");
		return;
	}
	
	//Select Fidei
	database="Fidei";
	if (mysql_select_db(conn, database));
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
	}
	
	char * search_1="INSERT INTO contact_list(user_id,source_id,raw_rating,timestamp) VALUES('";
	char * search_2="')";
	char * search; 
	
	single.i=0;
	while(!feof(contactlist))
	{      
		fscanf(contactlist,"%i %i %f %i\n",&single.target_id, &single.source_id, &single.rating, &single.timestamp);
	    sprintf(search,"%s%i,%i,%f,%i%s",search_1,single.target_id, single.source_id, single.rating,single.timestamp,search_2);
		mysql_query(conn, search);
		single.i++;
	}//end while loop
	printf("Loaded in %i values from contact_list into MySQL.\n",single.i);
	fclose(contactlist);
	remove("contact_list.fidei");
	printf("Contact list file delete.\n");
}//end mysql_loadin function


void mysql_loadout()
{	//Loads data from mysql into text file
	//Select Fidei
	database="Fidei";
	if (mysql_select_db(conn, database));
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		getchar();
		return;
	}

	//Connect to database adj_list table
	if (mysql_query(conn, "SELECT * FROM contact_list"))
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		getchar();
		return;
	}
	
	result=mysql_use_result(conn);
	row = mysql_fetch_row(result);
	
	if ((rating_list = fopen("contact_list.fidei", "w")) == NULL)
	{
		printf("***ERROR*** - Cannot open contact list.\n");
	}
	
	printf("Loading in data from MySQL into contact list...\n");
	//Start loading in data from mysql and save to text file
	single.i=0;
	while ((row = mysql_fetch_row(result)) != NULL)
	{
		fprintf(rating_list, "%i %i %d %i\n",atoi(row[1]),atoi(row[2]),atoi(row[3]),atoi(row[4]));
		single.i++;
	}//end while loop
	
	printf("Loaded in %i rows of data from MySQL.\n",single.i);
	
	//Unload memory
	mysql_disconnect();
	
}//end mysql_loadout function

void mysql_disconnect()
{
	mysql_free_result(result);
	mysql_close(conn);
	printf("Mysql connection closed and memory freed.\n");
}

void generate ()
{	single.rand_dec = ( rand() % 9);  //number random for 0 to 9
    single.rand_dec = ((single.rand_dec)+1); //makes rand_dec 1 to 10
    single.rand_rating = ( rand() % 5000);  //number rating from 0000 to 5000
}// end random number

void find_m_n ()
{    system("cls");
	printf("Reading 'rating list' and finding 'n' and 'm'...\n");
	if     ((rating_list = fopen("rating_list.fidei", "r")) == NULL)
	{
		printf("***ERROR*** - Cannot open rating list.\n");
		single.error=1;
		getchar();
		return;
	}//end if 
	else
	{
		//Reset All Values       
		single.target_id=0; 
		single.source_id=0; 
		single.rating=0; 
		single.target_id_o=0;
		single.found_n=0; 
		single.found_m=0; 
		single.source_id_o=0;
		single.cumu_m=0; 
		single.max_target=0;
		single.min_target=2147483647;
		single.max_source=0;
		single.min_source=2147483647;
		
		//Finds maximum n and m, total number of rows in rating list
		while(!feof(rating_list))
		{      
            fscanf(rating_list,"%i %i %f\n",&single.target_id, &single.source_id, &single.rating);
            
            //Progress Indicator
            single.count++;
            if (single.count>=1000000)
            {printf("Still reading for m & n...\n");
				single.count=0;
            }
            
            if (single.target_id>single.target_id_o)
			{
				single.found_n++;
				single.current_m=0;
			}//end if loop
			if (single.target_id==single.target_id_o)
			{single.current_m++;}//end if loop			   
            single.target_id_o=single.target_id;
            single.source_id_o=single.source_id;
            if(single.current_m>single.found_m)
            {
				single.found_m=single.current_m;
            }//end if loop 
            single.cumu_m++;
            //Finds maximum target ID
            if (single.target_id>single.max_target)
            {single.max_target=single.target_id;}
            
            //Finds minimum target ID
            if (single.target_id<single.min_target)
            {single.min_target=single.target_id;}            
            
            //Finds maximum source ID
            if (single.source_id>single.max_source)
            {single.max_source=single.source_id;}           
			
            //Finds minimum source ID
            if (single.source_id<single.min_source)
            {single.min_source=single.source_id;}  
			
		}//end do while loop
		single.found_n=((single.found_n));
		single.found_m=(((single.found_m)+1));
		
		//Finds largest between max target and max source
		if(single.max_source>single.max_target){single.largest=single.max_source;}
		else if(single.max_target>=single.max_source){single.largest=single.max_target;}
		
	}//end else loop
	fclose(rating_list);
	printf("Number of targets: %i   Max number of connections: %i \nCumulative number of connections: %i\n",single.found_n, single.found_m, single.cumu_m);
	printf("Min target ID=%i   Min source ID=%i \n",single.min_target, single.min_source);
	printf("Max target ID=%i   Max source ID=%i \n",single.max_target, single.max_source);
	single.count=0;
};//end find_m_n function

void memory_input()
{ if (single.error!=0)
{return;
}
	single.memory_question=0;
	printf("Please enter memory available in mega bytes (i.e. 700Mb=700) \nor 0 for automatic memory test and setup...\n");
	scanf("%i",&single.memory_question);
	if (single.memory_question<=0)
    {single.memory_question=0;
    }//end if loop
	if (single.memory_question!=0)
    {single.max_mem=(single.memory_question*1024*1024);
    }//end if loop 
}//end memory input function

void memory_test()
{    if (single.error!=0)
{return;
}
	
	//Find how much RAM is allocatable
	if (single.memory_question==0)
	{
		printf("Testing available memory...\n");
		for (single.i=0;single.i<10000;single.i++)
		{
			single.test_mem = (int *) realloc (single.test_mem, (sizeof(double)*12*1024*1024*single.i));//tests 48Mb at a time
			printf("Testing 96 x %iMb...\n",single.i);
			if (single.test_mem == NULL)
			{ single.max_mem = (((single.i)-1)*8*12*1024*1024);
				printf("Maximum memory available is %f bytes.\n",single.max_mem);
				free (single.test_mem);
				break;
			};//end if loop
		};//end for loop
	};//end if loop
	
	//Determine expected RAM requirements
	single.req_mem=(2*8*(single.found_n));                      //X0 and X0-1 arrays
	single.req_mem=(single.req_mem+((8+4+4)*single.cumu_m));    //Rating list in memory
	single.req_mem=1.1*single.req_mem;                          //safety margin
	printf("Estimated memory requirement for 2x arrays & Rating list is: %10.0f bytes.\n",single.req_mem);
	
	//Establish if hard drive or RAM to be used
	if (single.req_mem>=single.max_mem)
	{single.harddrive_flag=1;
		printf("Using hardrive for Rating List.\n");
	}//end if loop
	else {single.harddrive_flag=0;
		printf("Using RAM for Rating List.\n");}
	
}//end memory tests

void memory_control() //Function to allocate the array memory based upon the number of users (test_size)
{	if (single.error!=0)
{return;
}
    
    printf("Allocating memory...\n");
	
	single.accuracy=0.01;       //Sets convergence accuracy limit
    
    //Array 1 allocation - eigen vector
	arrays.eigen_vector = (double *) realloc (arrays.eigen_vector, single.largest * sizeof(double));
	if (arrays.eigen_vector == NULL)
	{ puts ("Error (re)allocating eigen vector array memory\n"); getchar();single.error=1;}
	
	//Array 2 allocation - eigen vector new
	arrays.eigen_vector_new = (double *) realloc (arrays.eigen_vector_new, single.largest * sizeof(double));
	if (arrays.eigen_vector_new == NULL)
	{ puts ("Error (re)allocating eigen vector new array memory\n");getchar(); single.error=1;}	
	
	//Allocates Adj List Memory
	if (single.harddrive_flag==0)
	{printf("Allocating adjacency list memory...\n");
		//Target ID
		adj_list.target_id = (int *) realloc (adj_list.target_id, (single.cumu_m) * sizeof(int*));
		if (adj_list.target_id==NULL)
		{ puts ("Error (re)allocating adjacency list target id memory\n"); getchar();single.error=1;}
		//Source ID
		adj_list.source_id = (int *) realloc (adj_list.source_id, (single.cumu_m) * sizeof(int*));
		if (adj_list.source_id==NULL)
		{ puts ("Error (re)allocating adjacency list source id memory\n"); getchar();single.error=1;}
		//Rating
		adj_list.rating = (double *) realloc (adj_list.rating, (single.cumu_m*2) * sizeof(double*));
		if (adj_list.rating==NULL)
		{ puts ("Error (re)allocating adjacency list rating memory\n"); getchar();single.error=1;}
    }//end if loop  
	printf("Memory allocated...\n");
	printf("Zeroing Memory...\n");
	
	memset (arrays.eigen_vector, '\0', sizeof(arrays.eigen_vector));
	memset (arrays.eigen_vector_new, '\0',sizeof(arrays.eigen_vector_new));
	
    if (single.harddrive_flag==0)
	{
		memset (adj_list.target_id, '\0', sizeof(adj_list.target_id));
		memset (adj_list.source_id, '\0',sizeof(adj_list.source_id) );
		memset (adj_list.rating, '\0', sizeof(adj_list.rating));
    }
    printf("Memory Reset...\n");
}//end memory control

void harddrive_matrix()
{ if (single.error!=0)
{return;
}
	printf("Reading Rating List from HD.\n");   
	//Create initial X0 (sets it all to 1)
	printf("Creating initial X0 \n");
	for (single.i=0;single.i<=single.max_source;single.i++)
	{	arrays.eigen_vector[single.i]=1;
	}//close for loop
	
	single.x0_temp=0;        //resets for results
	
	printf("Iteration: ");
	
	//START MATRIX MULTIPLICATION  
	for (single.its=0;single.its<=100;single.its++)
	{   
		printf("%i, ",single.its);
		
		//Read Rating List onto HD
		if ((rating_list = fopen("rating_list.fidei", "r")) == NULL)
		{
			printf("***ERROR*** - Cannot open Rating List.\n"); getchar(); single.error=1;
		}//end if 
		
		//Set entire eigen vector new to zero (so only new values will update)
		for (single.i=0;single.i<=single.max_target;single.i++)
		{arrays.eigen_vector_new[single.i]=0;
		}//close for loop
		
		//Perform matrix calc (A x X0)
		//Set all ID's from 0 to first value as zero
		for (single.i=0;single.i<single.min_target;single.i++)
		{   arrays.eigen_vector_new[single.i]=0;
		}
		
		//As the length of the list is known, no need to test for EOF  
		for (single.i=0; single.i<single.cumu_m; single.i++)
		{
			fscanf(rating_list,"%i %i %f\n",&single.target_id, &single.source_id, &single.rating);
			
            //Progress Indicator              
            single.count++;
            if (single.count>=1000000)
            {printf("\nStill calculating from HD...\n");
				single.count=0;
            }
			
			if(single.i==0) //corrects single.old for 1st iteration
			{single.old_n=single.target_id;}
			
			if (single.target_id>single.old_n)
			{
                single.old_n=single.current_n;
			}//end if loop
			
			single.current_n=single.target_id;
			single.current_m=single.source_id;
			
			if (single.target_id == (single.old_n))
			{ single.x0_temp=single.x0_temp+(single.rating*arrays.eigen_vector[single.current_m]);
			}//else loop for same target id
			else
			{                                   
				arrays.eigen_vector_new[single.old_n]=single.x0_temp;    //saves result
				single.x0_temp=0;                                        //resets for next row
				for (single.j=(single.old_n+1);single.j<single.current_n;single.j++)             //set all inbetween target id's to zero
				{
					arrays.eigen_vector_new[single.j]=0;
				}//end for loop
				single.x0_temp=single.x0_temp+(single.rating*arrays.eigen_vector[single.current_m]);
			}//if loop for next target
		}//end for loop
		//END MATRIX MULTIPLICATION
		
		//For last row value
		arrays.eigen_vector_new[single.old_n]=single.x0_temp;    //saves final row result
		single.x0_temp=0;
        
		//Copy Eigen_vector_new to Eigen_vector   - so if target>source, not a problem as eigen_vector increased in size 
		for (single.i=0;single.i<=single.max_target;single.i++)
		{arrays.eigen_vector[single.i]=arrays.eigen_vector_new[single.i];
		} //end outer loop
		
		//Correction if source > target
		if (single.max_source>single.max_target)
			for (single.i>single.max_target; single.i<=single.max_source; single.i++)
			{arrays.eigen_vector[single.i]=0;
			}//end for loop
		
		//Define x0_old - force initial iterations to greatly differ (i.e. fails test criteria its 1)
		if (single.its >=1)
		{  single.x0_old=single.x0_max;
		}
		else
		{ single.x0_old=single.x0_max*3;
		}
		single.x0_max=0;
		
		//Find largest value in eigen_vector
		for (single.i=0;single.i<=single.largest;single.i++)
    	{	if ( single.x0_max < arrays.eigen_vector[single.i])
		{ single.x0_max = arrays.eigen_vector[single.i];
		};
		}; //end for loop
		
		//Normalise X0
		for (single.i=0;single.i<=single.largest;single.i++)
		{ arrays.eigen_vector[single.i] = (arrays.eigen_vector[single.i] / single.x0_max);
		}; //end for loop
		single.lambda=single.x0_max;
		
		//Test for error level
		single.eigen_test=((( single.x0_max - single.x0_old ) / single.x0_max * 100));
		
		if (single.eigen_test <= single.accuracy && single.eigen_test >= (-single.accuracy) && single.its>3)
		{	printf("\nPowermethod reached %2.2f criteria after %i iterations\n", single.accuracy, single.its);
			single.count=0; fclose(rating_list); break;
        }
		
		else if(single.its==100)
		{printf("\nPowermethod **DID NOT** reach %f criteria after %i iterations\n", single.accuracy, single.its);
			fclose(rating_list);
			single.count=0;
			getchar();
		};
		fclose(rating_list);
	}//end iterations for loop  
};//end harddrive_matrix function

void memory_matrix()
{   if (single.error!=0)
{return;
}
	
	//Read Rating List into memory
	printf("Reading Rating List into Memory.\n");     
	if ((rating_list = fopen("rating_list.fidei", "r")) == NULL)
	{
		printf("***ERROR*** - Cannot open Rating List.\n"); getchar();single.error=1;
		return;
	}//end if 
	else
	{ single.i=0;
		while(!feof(rating_list))
		{fscanf(rating_list,"%i %i %f\n",&single.target_id, &single.source_id, &single.rating);
			adj_list.target_id[single.i]=single.target_id;
			adj_list.source_id[single.i]=single.source_id;
			adj_list.rating[single.i]=single.rating;
			single.i++;
			
			//Progress Indicator              
			single.count++;
			if (single.count>=1000000)
			{printf("Still reading in list for memory operation...\n");
				single.count=0;
			}                
		}//end while loop
	}//end else loop
	fclose(rating_list);            
	single.count=0;
	
	//Create initial X0 (sets it all to 1)
	printf("Creating initial X0 \n");
	for (single.i=0;single.i<=single.max_source;single.i++)
	{	arrays.eigen_vector[single.i]=1;
	}//close for loop
	
	single.x0_temp=0;        //resets for results
	printf("Iteration: ");
	//START MATRIX MULTIPLICATION  
	for (single.its=0;single.its<=100;single.its++)
	{
		printf("%i, ",single.its);   
		//Set entire eigen vector new to zero (so only new values will update)
		for (single.i=0;single.i<=single.max_target;single.i++)
		{arrays.eigen_vector_new[single.i]=0;
		}//close for loop
		
		//Perform matrix calc (A x X0)
		//Set all ID's from 0 to first value as zero
		for (single.i=0;single.i<single.min_target;single.i++)
		{   arrays.eigen_vector_new[single.i]=0;
		}
		
		for (single.i=0; single.i<single.cumu_m; single.i++)
		{
			single.current_n=adj_list.target_id[single.i];
			single.current_m=adj_list.source_id[single.i];
			
			//Progress Indicator              
			single.count++;
			if (single.count>=1000000000)
			{printf("\nStill performing calculation in memory...\n");
				single.count=0;
			} 
			
			if(single.i==0) //corrects single.old for 1st iteration
			{single.old_n=single.current_n;}
			else 
			{single.old_n=(adj_list.target_id[((single.i)-1)]);}
			
			if (adj_list.target_id[single.i] == (single.old_n))
			{ single.x0_temp=single.x0_temp+(adj_list.rating[single.i]*arrays.eigen_vector[single.current_m]);
			}//else loop for same target id
			else
			{                                   
				arrays.eigen_vector_new[single.old_n]=single.x0_temp;    //saves result
				single.x0_temp=0;                                        //resets for next row
				for (single.j=(single.old_n+1);single.j<single.current_n;single.j++)             //set all inbetween target id's to zero
				{
					arrays.eigen_vector_new[single.j]=0;
				}//end for loop
			}//end else loop for next target
		}//end for loop
		//END MATRIX MULTIPLICATION
		
		//Update for last row value
		arrays.eigen_vector_new[single.old_n]=single.x0_temp;    //saves final row result
		single.x0_temp=0;
        
		//Copy Eigen_vector_new to Eigen_vector   - so if target>source, not a problem as eigen_vector increased in size 
		for (single.i=0;single.i<=single.max_target;single.i++)
		{arrays.eigen_vector[single.i]=arrays.eigen_vector_new[single.i];
		} //end outer loop
		
		//Correction if source > target
		if (single.max_source>single.max_target)
			for (single.i>single.max_target; single.i<=single.max_source; single.i++)
			{arrays.eigen_vector[single.i]=0;
			}//end for loop
		
		//Define x0_old - force initial iteration to greatly differ (i.e. forces test criteria fail for its 1)
		if (single.its >=1)
		{  single.x0_old=single.x0_max;
		}
		else
		{ single.x0_old=single.x0_max*3;
		}
		single.x0_max=0;
		
		//Find largest value in eigen_vector
		for (single.i=0;single.i<=single.largest;single.i++)
    	{	if ( single.x0_max < arrays.eigen_vector[single.i])
		{ single.x0_max = arrays.eigen_vector[single.i];
		};
		}; //end for loop
		
		//Normalise X0	
		for (single.i=0;single.i<=single.largest;single.i++)
		{ arrays.eigen_vector[single.i] = (arrays.eigen_vector[single.i] / single.x0_max);
		}; //end for loop
		single.lambda=single.x0_max;
		
		//Test for error level
		single.eigen_test=((( single.x0_max - single.x0_old ) / single.x0_max * 100));
		
		if (single.eigen_test <= single.accuracy && single.eigen_test >= (-single.accuracy) && single.its>3)
		{	printf("\nPowermethod reached %2.2f criteria after %i iterations\n", single.accuracy, single.its);
	   		break;
        }
		
		else if(single.its==100)
		{printf("\nPowermethod **DID NOT** reach %f criteria after %i iterations\n", single.accuracy, single.its);
			getchar();
		};
	}//end iterations for loop  
};// end memory matrix function

void export_results()
{
    if (single.error!=0)
	{	printf("Error code does not equal 0. Aborting.\n");
    getchar();
    return;
	}
	
	printf("Exporting results...\n");
	printf("Saving centrality results...\n");
	if ((centrality = fopen("centrality.fidei", "w")) == NULL)
	{
		printf("***ERROR*** - Cannot open centrality for saving results.\n");single.error=1;getchar();
	}//end if 
	else
	{for (single.i=0;single.i<=single.max_target;single.i++)
	{if (arrays.eigen_vector[single.i]==0)
	{
	}//end if loop
	else
	{
		fprintf(centrality, "%i,%4.16f \n", single.i, arrays.eigen_vector[single.i]);                                                 
	}             
		
		//Progress Indicator              
		single.count++;
		if (single.count>=1000000)
		{printf("Still exporting...\n");
			single.count=0;
		}
	}//end for loop
	}//end else
	fclose(centrality);
	
	printf("Saving lambda value...\n");
	if ((lambda = fopen("lambda.fidei", "w")) == NULL)
	{
		printf("***ERROR*** - Cannot open lambda for saving results.\n");
		getchar();single.error=1;
	}//end if 
	
	else
	{time(&now);
        single.time_now = (unsigned long) (now);   
		fprintf(lambda, "%8.16f, %li",single.lambda, single.time_now);
	}//end else loop
	fclose(lambda);
	printf("Saved lambda value.\n");
	printf("Both results saved.\n");
}//end export_results function

void export_results_mysql()
{
    if (single.error!=0)
    {	printf("Error code does not equal 0. Aborting.\n");
        getchar();
        return;
    }
	
    char * output;
    
	printf("Exporting results to MySQL...\n");
	printf("Saving centrality results...\n");
	
    single.count=0;
    mysql_connect();
    
    //INSERT BIT THAT CONNECTS TO FIDEI DATABASE
	
    //Save eigen vector results
    time(&now);//keep now here so that all results have same timestamp value
    	
    	for (single.i=0;single.i<=single.max_target;single.i++)
		{
            if (arrays.eigen_vector[single.i]==0)
    		{//i.e. if no result, do nothing
			}//end if loop
			
            else
			{
            single.time_now = (unsigned long) (now);    
        	sprintf(output,"INSERT INTO centrality_results (user_id, rating, timestamp) VALUES ('%i','%f','%li')",single.i,arrays.eigen_vector[single.i],single.time_now);
			if (mysql_query(conn, output)) 
        		{
            	printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
        		}//end if loop
			}//end else loop             
		
			//Progress Indicator              
			single.count++;
			if (single.count>=1000000)
			{printf("Still exporting...\n");
			single.count=0;
			}//end if loop
		}//end for loop
	
    //Save lambda value
	printf("Saving lambda value...\n");
	output="NULL";//reset
    single.time_now = (unsigned long) (now);
    sprintf(output,"INSERT INTO lambda (lambda, timestamp) VALUES ('%f','%li')",single.lambda, single.time_now);

	if (mysql_query(conn, output)) 
    {
        printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
    }//end if loop
	
    mysql_disconnect();
    printf("Saved lambda value.\n");
	printf("Both results saved.\n");
    
}//end void export_results_mysql

void free_memory ()		//Frees memory allocated at the beginning
{	printf("Freeing up memory...\n");
    free(arrays.eigen_vector);
	free(arrays.eigen_vector_new);
	printf("X0 freed...\n");
	printf("Results arrays freed...\n");
    if (single.harddrive_flag==0)
    {
		free(adj_list.target_id);
		free(adj_list.source_id);
		free(adj_list.rating);
		printf("Adjacency List freed...\n");
    }
    printf("All Memory Freed.\n");
    getchar();
}//end free memory function

void check_contact_list ()
{ if (single.error!=0)
	{return;
	}//end of is loop
	
	system("cls");
	printf("Reading 'contact list' and verifying target and source id correct order...\n");
	if     ((contactlist = fopen("contact_list.fidei", "r")) == NULL)
	{
		printf("***ERROR*** - Cannot open contact list.\n");
		getchar();single.error=1;
		return;
	}//end if 
	else
	{       //Initialise
		single.current_target=0;
		single.current_source=0;
		single.current_rating=0;
		single.current_time_stamp=0;
		single.count=0;
		single.time_zero=0;
		
		while(!feof(contactlist))
		{
			single.count++;                            
			fscanf(contactlist,"%i, %i, %f, %i\n",&single.current_target, &single.current_source, 
				   &single.current_rating, &single.current_time_stamp);
			
			//Tests to see that target id's increase ascendingly
			if (single.current_target<single.previous_target)
			{printf("Error in target ID's, line %i.\n",single.count);
				order_contact_list();
				break;
			}// end if
			//Tests that the sources all increase ascendingly, and that no sources are repeated on two lines together    
			if (single.current_source<=single.previous_source)
			{printf("Error in source ID's, line %i.\n",single.count);
				order_contact_list();
				break;
			}//end if
			
			//Test that rating is a real number greater than zero
			if (single.current_rating<0)
			{printf("Error in rating number, line %i. Correct error manually.\n",single.count);
				break;
			}// end if
			
			//Test that timestamp is a number greater than zero and in the past
			time(&now); //stores the current time
			single.test_time = difftime (single.current_time_stamp, single.time_zero);
			if (single.test_time>now)
			{printf("Error in time value (i.e. in future or <0), line %i. Correct error manually.\n",single.count);
				break;
			};// end if
			
		};//end while loop
		printf("%i lines read, no errors found.\n",single.count);
		fclose(contactlist);
	};//end else
};//end check contact list function

void order_contact_list()
{	
    single.menu_select=0;
	printf("A source_id or target_id was found in the Contact List order. \n"
		   "Do you wish to attempt to reorder list and correct error?\n"
		   "Enter '1' to proceed, '2' to abort.\n");
	scanf("%d",&single.menu_select);
	while(1)
	{
	switch(single.menu_select)
	{
		case 1: {
				//Order program
				//Determine length of contact list
				if (single.error!=0)
					{printf("ALERT: Error code does not equal zero. Upstream error - aborting.\n");
						break;	
					}
				if ((contactlist = fopen("contact_list.fidei", "r")) == NULL)
				{
				printf("***ERROR*** - Cannot open Contact List.\n"); getchar();
				break;
				}//end if 
				printf("Scanning contact list to determine number of rows.\n");
				single.count=0;
				while(!feof(contactlist))
				{fscanf(contactlist, "\n");
				 single.count++;
				}//end while loop
				fclose(contactlist);
				printf("Scan completed. %i rows found.\n",single.count);
			
				//Allocate memory
				 //Target ID
				contact_list.target_id = (int *) realloc (contact_list.target_id, (single.count) * sizeof(int*));
				if (contact_list.target_id==NULL)
				{ puts ("Error (re)allocating contact list target id memory\n"); getchar();single.error=1;}

				//Source ID
				contact_list.source_id = (int *) realloc (contact_list.source_id, (single.count) * sizeof(int*));
				if (contact_list.source_id==NULL)
				{ puts ("Error (re)allocating contact list source id memory\n"); getchar();single.error=1;}
			
				//Raw Rating
				contact_list.raw_rating = (float *) realloc (contact_list.raw_rating, (single.count) * sizeof(float*));
				if (contact_list.raw_rating==NULL)
				{ puts ("Error (re)allocating contact list raw rating memory\n"); getchar();single.error=1;}
			
				//Timestamp
				contact_list.timestamp = (int *) realloc (contact_list.timestamp, (single.count) * sizeof(int*));
				if (contact_list.timestamp==NULL)
				{ puts ("Error (re)allocating contact list timestamp memory\n"); getchar();single.error=1;}
				if (single.error==1){break;}
				printf("All memory allocated successfully.\n");
			
				//Load contact list into memory
				printf("Loading contact list into memory.\n");
				if ((contactlist = fopen("contact_list.fidei", "r")) == NULL)
				{
				printf("***ERROR*** - Cannot open Contact List.\n"); getchar();
				break;
				}//end if 
				single.count=0;
				while(!feof(contactlist))
				{fscanf(contactlist, "%i %i %f %i\n",&contact_list.target_id[single.count],&contact_list.source_id[single.count],
						&contact_list.raw_rating[single.count],&contact_list.timestamp[single.count]);
				single.count++;
				}//end while loop
				fclose(contactlist);
				printf("Loaded in %i lines of contact list into memory.\n",single.count);
			
				//Performing sort
				printf("Sorting by target_id (elem1).\n");
				//Compare function 1
				qsort((void *) &contact_list,                    // Beginning address of array
				  single.count,                         // Number of elements in array
				  sizeof(struct CONTACT_LIST),             // Size of each element
				  (compfn)compare );                  // Pointer to compare function
				
				printf("Sorting source_id (elem2)\n");
				//Compare function 2
				//Find first address of element to order & number of rows affected
				int i,j=0;
				int length=0;
				int start=0;
				int end=0; 
			
				for (i=0;i<single.count;i++)
				{	if(contact_list.target_id[i]!=contact_list.target_id[j])
					{
						start=j;
						end=i;
						length = end - start;	
						//printf("Length found, sorting (start %i, end %i, length %i.)\n",start,end,length);
						//	printf("Location of first element is %i.\n",&array[start].user_id);
				
						//Perform Sort
						qsort((void *) (
								&contact_list.target_id[start]),		// Beginning address of array
								length,                             // Number of elements in array
								sizeof(struct CONTACT_LIST),             // Size of each element
								(compfn)compare_2);                 // Pointer to compare function	
						j=i;
					}//end if loop
					end++;
				
				//printf("I: %d, J: %d.\n",i,j);	
				//getchar();	
				}//end while loop
				printf("Contact List has now been sorted correctly.\n");
				
				//Exporting results
				printf("Exporting results.\n");
				remove("contact_list.fidei");
				printf("Deleted old contact list file.\n");
				if ((contactlist = fopen("contact_list.fidei", "r")) == NULL)
				{
				printf("***ERROR*** - Cannot open Contact List.\n"); getchar();
				break;
				}//end if 
				printf("Writing new contact list...\n");
				for (i=0;i<single.count;i++)
					{fprintf(contactlist, "%i %i %f %i\n",contact_list.target_id[i],contact_list.source_id[i],
							 contact_list.raw_rating[i],contact_list.timestamp[i]);
					}//end for loop
				fclose(contactlist);
				printf("Wrote %i lines to contact list. Sorting completed.\n",i);
				
				//End of program
				printf("Now restarting contact list check program.\n");
				check_contact_list();
				}
			
		case 2: //returns to main program
				break;
			
	}//end switch loop
	}//end while loop
}

void find_user_rating_mysql()
{	
    single.search_id=0;
    char * output;
	system("cls");
	printf("Please enter desired user's id number:\n");
	scanf("%i",&single.search_id);
	
	if (single.search_id==0)
	{printf("No value entered - aborting.\n");
		return;
	}
	
    //Call function to get result
	user_reputation();
    if (single.rating!=0)
    {printf("Rating result is %f.\n",single.rating);
    }//end if
    else 
    {printf("Error in establishing user rating.\n");
    }
    
    //Save results to MySQL
    time(&now); //resets clock to current time
    single.time_now = (unsigned long) (now);
    sprintf(output,"INSERT INTO user_ratings (user_id,user_rating,timestamp VALUES ('%i','%f','%li')",single.search_id,single.rating,single.time_now);
    if (mysql_query(conn, output)) 
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
	}
    printf("Results inserted into MySQL.\n");
    
	getchar();
}//end find user rating mysql

//Function that takes in single.search_id and returns computed rating
void user_reputation()
{	
    char * search;
    single.rating=0;
    mysql_connect();
    loadin_factors();
    
	//Switch to Fidei database
	database="Fidei";
	if (mysql_select_db(conn, database));
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		getchar();
		return;
	}
	
	sprintf(search,"SELECT target_id, source_id, raw rating FROM rating_list WHERE target_id='%i' ORDER BY source_id",single.search_id);
	
	//Send query to read data from database
	printf("Performing search on user_id %i.\n",single.search_id);
	if (mysql_query(conn, search));
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		getchar();
		return;
	}
	
	result = mysql_store_result(conn);
	if (result == NULL)
	{printf("No results found.\n");
	};
	
	time(&now); //resets clock to current time
	single.i=0;
	
	//Now for rows 0 to number of number of rows
	while ((row = mysql_fetch_row(result)) != NULL)
	{   
		if (row == NULL)
		{printf("No results found.\n");
		};

		single.current_time_stamp=atoi(row[3]);
		single.raw_rating=atoi(row[2]);
		single.rating_elapsed=difftime(now,single.current_time_stamp);
		
		if (single.rating_elapsed<0)
		{printf("Error in difftime calc; timestamp in future.\nProcess will continue,but result is invalid.\n");
			getchar();
		}//end if loop
		
		//Resets & backup setting (i.e. no time correction if below faults)
		single.rating_correction=1;
		
		//Rating within 1 month - 1 month=60*60*24*30.5=2635200 seconds
		if (single.rating_elapsed<2635200)
		{single.rating_correction=rating_factors.time_1month;}
		//Rating within 2 month
		if (single.rating_elapsed<5270400 && single.rating_elapsed>=2635200)
		{single.rating_correction=rating_factors.time_2month;}
		//Rating within 3 month
		if (single.rating_elapsed<7905600 && single.rating_elapsed>=5270400)
		{single.rating_correction=rating_factors.time_3month;}
		//Rating within 4 month
		if (single.rating_elapsed<10540800 && single.rating_elapsed>=7905600)
		{single.rating_correction=rating_factors.time_4month;}
		//Rating within 5 month
		if (single.rating_elapsed<13176000 && single.rating_elapsed>=10540800)
		{single.rating_correction=rating_factors.time_5month;}
		//Rating within 6 month
		if (single.rating_elapsed<15811200 && single.rating_elapsed>=13176000)
		{single.rating_correction=rating_factors.time_6month;}
		//Rating within 7 month
		if (single.rating_elapsed<18446400 && single.rating_elapsed>=15811200)
		{single.rating_correction=rating_factors.time_7month;}
		//Rating within 8 month
		if (single.rating_elapsed<21081600 && single.rating_elapsed>=18446400)
		{single.rating_correction=rating_factors.time_8month;}
		//Rating within 9 month
		if (single.rating_elapsed<23716800 && single.rating_elapsed>=21081600)
		{single.rating_correction=rating_factors.time_9month;}
		//Rating within 10 month
		else if (single.rating_elapsed>=23716800)
		{single.rating_correction=rating_factors.time_10month;}
		//Another backup line
		else (single.rating_correction=rating_factors.time_10month);
        
		single.adjusted_rating=0; //reset
		single.adjusted_rating=(single.rating_correction*single.raw_rating);
		
		single.rating=single.rating+single.adjusted_rating;
	};
	mysql_disconnect();
    
    return;
}//end function user reputation


void convert_contact_list()
{	
	system("cls");
	printf("Converting contact list into rating list...\n");
	loadin_factors();
    
	if     ((contactlist = fopen("contact_list.fidei", "r")) == NULL)
	{
		printf("***ERROR*** - Cannot open contact list for reading.\n");
		getchar();single.error=1;
		return;
	}//end if 
	else
	{   if((contactlist = fopen("rating_list.fidei", "w")) == NULL)
	{
		printf("***ERROR*** - Cannot open rating list for writing.\n");
		getchar();single.error=1;
		return;
	}//end if 
	else
	{   //to run below code if contact list is open-able and rating list can be written
		time(&now); //resets clock to current time
		while(!feof(rating_list))
		{	
            //Reset values for each iteration
            single.current_source=0;
            single.current_target=0;
            single.raw_rating=0;
            single.current_time_stamp=0;
            
            fscanf(contactlist,"%i, %i, %f, %i\n",&single.current_target, &single.current_source, 
				&single.raw_rating, &single.current_time_stamp);
			
			single.rating_elapsed=difftime(now,single.current_time_stamp);
			
			//Resets & backup setting (i.e. no time correction if below faults)
			single.rating_correction=1;
			
            //Rating within 1 month - 1 month=60*60*24*30.5=2635200 seconds
            if (single.rating_elapsed<2635200)
            {single.rating_correction=rating_factors.time_1month;}
            //Rating within 2 month
            if (single.rating_elapsed<5270400 && single.rating_elapsed>=2635200)
            {single.rating_correction=rating_factors.time_2month;}
            //Rating within 3 month
            if (single.rating_elapsed<7905600 && single.rating_elapsed>=5270400)
            {single.rating_correction=rating_factors.time_3month;}
            //Rating within 4 month
            if (single.rating_elapsed<10540800 && single.rating_elapsed>=7905600)
            {single.rating_correction=rating_factors.time_4month;}
            //Rating within 5 month
            if (single.rating_elapsed<13176000 && single.rating_elapsed>=10540800)
            {single.rating_correction=rating_factors.time_5month;}
            //Rating within 6 month
            if (single.rating_elapsed<15811200 && single.rating_elapsed>=13176000)
            {single.rating_correction=rating_factors.time_6month;}
            //Rating within 7 month
            if (single.rating_elapsed<18446400 && single.rating_elapsed>=15811200)
            {single.rating_correction=rating_factors.time_7month;}
            //Rating within 8 month
            if (single.rating_elapsed<21081600 && single.rating_elapsed>=18446400)
            {single.rating_correction=rating_factors.time_8month;}
            //Rating within 9 month
            if (single.rating_elapsed<23716800 && single.rating_elapsed>=21081600)
            {single.rating_correction=rating_factors.time_9month;}
            //Rating within 10 month
            else if (single.rating_elapsed>=23716800)
            {single.rating_correction=rating_factors.time_10month;}
            //Another backup line
            else (single.rating_correction=rating_factors.time_10month);

			single.adjusted_rating=(single.rating_correction*single.raw_rating);
			
			//Export results to rating list
			fprintf(rating_list, "%i %i %f\n",single.current_target,single.current_source,single.adjusted_rating);
			
		}//end while loop
	}//end inner else loop
		
		fclose(rating_list);
		fclose(contactlist);      
	}//end else loop
	
}//end convert contact list function

void company_rating()
{    
	system("cls");
	char * search;
    char * output;
	single.company_id=0;
	int num_rows=0;
	int num_users=0;
    float company_rating=0;
	
	if (single.error!=0)
	{	printf("Warning, error code 1 in system.\nContinuing...\n");
		getchar();
	}
	
	printf("Determine company rating.\nPlease enter company ID number:\n");
	scanf("%i",&single.company_id);
	if (single.company_id==0)
	{	printf("No value entered - aborting.\n");
		return;
	}
	
	//Search company list for company
	//Switch to Fidei database
	database="Fidei";
	if (mysql_select_db(conn, database));
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		getchar();
		return;
	}
	
	sprintf(search,"SELECT company_id, user_id FROM company WHERE company_id='%i' ORDER BY user_id",single.company_id);
	
	//Send query to read data from database
	printf("Performing search on company_id %i.\n",single.company_id);
	if (mysql_query(conn, search));
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		getchar();
		return;
	}
	
	result = mysql_store_result(conn);
	if (result == NULL)
	{printf("No results found.\n");
	};
	
	//Determine number of rows
	//Find the number of fields (columns)
	num_rows = mysql_num_rows(result);
	printf("Found %i connected users.\n",num_rows);
	
	//Memory allocation
	//Company ID
	search_company.company_id = (int *) realloc (search_company.company_id, ((num_rows +1) * sizeof(int)));
	if (search_company.company_id == NULL)
	{ puts ("Error (re)allocating search company company id memory\n"); getchar();single.error=1;}
	
    //User ID
	search_company.user_id = (int *) realloc (search_company.user_id, ((num_rows +1) * sizeof(int)));
	if (search_company.user_id == NULL)
	{ puts ("Error (re)allocating search company user id memory\n"); 
        getchar();
        single.error=1;
    };
	printf("Memory allocated.\n");
											  
	//Now for rows 0 to number of number of rows
	single.i=0;
	while ((row = mysql_fetch_row(result)) != NULL)
	{   * search_company.company_id=atoi(row[0]);
		* search_company.user_id=atoi(row[1]);	
		single.i++;
	}//end while loop
	
	printf("Read in %i of company users.\n",single.i);
    num_users=single.i;
                                              
	//Sum up all respective user's scores
	for (single.i=0;single.i<num_users;single.i++)
	{
		single.search_id=search_company.user_id[single.i];
        user_reputation();
        company_rating=company_rating+single.rating;
            
	};//end for loop

	
	//Output results
	printf("Processed %i connected users, company's score is %f.\n",single.i,company_rating);
	
    //Save results to MySQL
    time(&now); //resets clock to current time
    single.time_now = (unsigned long) (now);
    sprintf(output,"INSERT INTO company_ratings (company_id,company_rating,timestamp VALUES ('%i','%f','%li')",single.company_id,company_rating,single.time_now);
    if (mysql_query(conn, output)) 
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
	}
    printf("Results inserted into MySQL.\n");
    mysql_disconnect();
    free (search_company.company_id);
    free (search_company.user_id);
    printf("Memory freed.\n");    
    getchar();
    
};//end company rating function

void loadin_factors()
{
        if     ((ratingfactors = fopen("ratingfactors.fidei", "r")) == NULL)
        {
            printf("***ERROR*** - Cannot open rating factors.\n");
            return;
        }
        fscanf(ratingfactors, "%f\n",&rating_factors.time_1month);
        fscanf(ratingfactors, "%f\n",&rating_factors.time_2month);
        fscanf(ratingfactors, "%f\n",&rating_factors.time_3month);
        fscanf(ratingfactors, "%f\n",&rating_factors.time_4month);
        fscanf(ratingfactors, "%f\n",&rating_factors.time_5month);
        fscanf(ratingfactors, "%f\n",&rating_factors.time_6month);
        fscanf(ratingfactors, "%f\n",&rating_factors.time_7month);
        fscanf(ratingfactors, "%f\n",&rating_factors.time_8month);
        fscanf(ratingfactors, "%f\n",&rating_factors.time_9month);
        fscanf(ratingfactors, "%f\n",&rating_factors.time_10month);
        fscanf(ratingfactors, "%f\n",&rating_factors.link_level1);
    	fscanf(ratingfactors, "%f\n",&rating_factors.link_level2);
        fscanf(ratingfactors, "%f\n",&rating_factors.link_level3);
        fscanf(ratingfactors, "%f\n",&rating_factors.link_level4);
        fscanf(ratingfactors, "%f\n",&rating_factors.link_level5);
        fscanf(ratingfactors, "%f\n",&rating_factors.link_level6);
        fclose(ratingfactors);
        
};//end loadin_factors function
                                              
void list_factors()
    {	printf("Rating factors...\n");
        float factor=0;
        
        if((ratingfactors = fopen("ratingfactors.fidei", "r")) == NULL)
    	{
        printf("***ERROR*** - Cannot open rating factors.\n");
        return;
   	 	}//close if loop
        
        single.i=0;
        while(!feof(ratingfactors))
		{ fscanf(ratingfactors, "%f\n",&factor);
          printf("Factor %i is %f.\n",single.i,factor);
        }//end while loop
		printf("To edit factors, edit text file 'ratingfactors.fidei.\n");
        getchar();
    fclose(ratingfactors);
}//end list factors

//TODO
//1) Finish data generator
//3) Function that makes a raw rating
//6) Test and prove program works well
//7) Generate basic stats/analytics for users
//8) Generate parralesied process

int main(int argc, char *argv[])
{ int menu_option=0;
	srand(time(NULL));
	time_t rawtime;
	struct tm * timeinfo;
	time ( &rawtime );
	timeinfo = localtime ( &rawtime );
	
	time (&start);
	while(1)
	{   single.error=0;
		system("cls");  
		printf("Fidei Engine v0.96 - Beta\n"
			   "-------------------------\n\n"); 
		printf("Enter 0 to load in MySQL data.\n"
			   "Enter 1 to check contact list is correct.\n"
			   "Enter 2 to convert contact list into the rating list.\n"
			   "Enter 3 to perform centrality calculation and save centrality results.\n"
			   "Enter 4 to perform stages 0 - 3 automatically.\n"
               "Enter 6 to find a user's own rating score from mysql database.\n"
			   "Enter 7 to find a company's score from mysql.\n"
			   "Enter 8 to list the rating factors.\n"
			   "Enter 99 to exit.\n");
		scanf("%d",&menu_option);
		switch(menu_option)
		{	
			case 0: mysql_connect();
					mysql_loadin();
					mysql_loadout();
				break;
			case 1: check_contact_list ();
				break;
			case 2: convert_contact_list();
				break;
			case 3: 
				find_m_n();
				memory_input();   
				memory_test();   
				memory_control();
				
				if (single.harddrive_flag==0)
				{memory_matrix();
				}//end if loop
				if (single.harddrive_flag==1)
				{harddrive_matrix();
				}//end if loop
				export_results();
				export_results_mysql();
                
				time (&end);
				single.time_taken = difftime (end, start);
				printf("Processing time was: %4.0f seconds. \n",single.time_taken);
				getchar();
				free_memory();	
				//insert functions
				break;
			case 4: 
				mysql_connect();
				mysql_loadin();
				mysql_loadout();
				check_contact_list ();
				convert_contact_list();
				find_m_n();
				memory_input();
				memory_test();        
				memory_control();
				
				if (single.harddrive_flag==0)
				{memory_matrix();
				}//end if loop
				if (single.harddrive_flag==1)
				{harddrive_matrix();
				}//end if loop
				export_results();
                export_results_mysql();
				
				time (&end);
				single.time_taken = difftime (end, start);
				printf("Processing time was: %4.0f seconds. \n",single.time_taken);
				getchar();
				free_memory();	
				break;
			case 6: find_user_rating_mysql();
				break;
			case 7: company_rating();
				break;
			case 8: list_factors();
				break;
			case 99: exit(1);
				
			default: printf("\n\nYou have not chosen a valid selection. Please re-try.\n\n");
				getchar();
				system("cls");
				break; 
		}//end switch
	}//end while loop
};//end main