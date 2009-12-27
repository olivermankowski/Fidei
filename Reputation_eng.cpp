#include <stdio.h>
#include <time.h>       //for time stamps
#include <stdlib.h>     //for random numbers

//File Initialisation
FILE *rating_list;  //file to store rating list data.
FILE *centrality;   //file to store centrality results.
FILE *contact_list; //file to read in contact list
FILE *lambda;       //holds the centrality programs lambda result

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
    int previous_source;
    int current_source;
    int current_time_stamp;
    float raw_rating;
    float adjusted_rating;
    int time_zero;
    int company_id;
    float company_score;
    float time_correction_factor;
    float rating_elapsed;
    float current_rating;
    double test_time; //stores the difference between read time and the time now
    float rating_correction; //holds the time adjusted correction factor
};
struct SINGLE single;	//declare singles struct, ref by "singles.xxx"

time_t start, end, now;

void find_users_score()
{    if (single.error!=0)
     {return;
     }
     
     system("cls");
     //Enter user's number
     printf("Please enter desired user's (i.e. target id) number:\n");
     scanf("%i",&single.user_score);
     
     //Open centrality results
     if ((centrality = fopen("centrality.fidei", "r")) == NULL)
           	{
			printf("***ERROR*** - Cannot open centrality list for reading.\n");
			system("PAUSE");single.error=1;
			return;
            }//end if
     
     //Open lambda value
     if ((lambda = fopen("lambda.fidei", "r")) == NULL)
           	{
			printf("***ERROR*** - Cannot open lambda file for reading.\n");
			system("PAUSE");single.error=1;
            return;
            }//end if
     
     //Load lambda value
     fscanf(lambda, "%8.16f",&single.lambda);
     
     //Find number in contact list
     if ((contact_list = fopen("contact_list.fidei", "r")) == NULL)
           	{
			printf("***ERROR*** - Cannot open contact list for reading.\n");
			system("PAUSE");single.error=1;
			return;
            }//end if 
		else
			{ single.rolling_rating_sum=0;
			time(&now); //reset timer
              while(!feof(contact_list))
                 {fscanf(contact_list, "%i, %i, %f, %f\n",&single.current_target, &single.current_source, &single.raw_rating, &single.current_time_stamp);
                  
                 //Once correct user ID is found, need to perform calculation for every source that user has 
                  if (single.current_target==single.user_score)
                     {while(single.current_target==single.user_score)
                            {
                             //Backup encase the while loop skips
                             if(single.current_target!=single.user_score)
                             {printf("User's score computed.\n");
                             break;
                             }//end if loop
                                                                     
                            //Find time correction factor
                            single.rating_elapsed = difftime(now, single.current_time_stamp);
                           
                           //Resets & backup setting (i.e. no time correction if below faults)
                           single.rating_correction=1;
                           
                           //Rating within 1 month - 1 month=60*60*24*30.5=2635200
                           if (single.rating_elapsed<2635200)
                           {single.rating_correction=1;}
                           //Rating within 2 month
                           if (single.rating_elapsed<5270400 && single.rating_elapsed>=2635200)
                           {single.rating_correction=0.9;}
                           //Rating within 3 month
                           if (single.rating_elapsed<7905600 && single.rating_elapsed>=5270400)
                           {single.rating_correction=0.8;}
                           //Rating within 4 month
                           if (single.rating_elapsed<10540800 && single.rating_elapsed>=7905600)
                           {single.rating_correction=0.7;}
                           //Rating within 5 month
                           if (single.rating_elapsed<13176000 && single.rating_elapsed>=10540800)
                           {single.rating_correction=0.6;}
                           //Rating within 6 month
                           if (single.rating_elapsed<15811200 && single.rating_elapsed>=13176000)
                           {single.rating_correction=0.5;}
                           //Rating within 7 month
                           if (single.rating_elapsed<18446400 && single.rating_elapsed>=15811200)
                           {single.rating_correction=0.4;}
                           //Rating within 8 month
                           if (single.rating_elapsed<21081600 && single.rating_elapsed>=18446400)
                           {single.rating_correction=0.3;}
                           //Rating within 9 month
                           if (single.rating_elapsed<23716800 && single.rating_elapsed>=21081600)
                           {single.rating_correction=0.2;}
                           //Rating within 10 month
                           else if (single.rating_elapsed>=23716800)
                           {single.rating_correction=0.1;}
                           
                           //Another backup line
                           else (single.rating_correction=0.1);
                           
                           //Performs time correction
                           single.adjusted_rating=(single.rating_correction*single.raw_rating);
                           
                           //Now find the centrality score of the source ID and factor it in
                           single.current_centrality=0;
                           
                           while(!feof(centrality))
                           {fscanf(centrality, "%i,%4.16f \n", single.i, single.current_centrality); 
                           if (single.i==single.current_source)
                              {break;//i.e. stop when correct source Id found
                              }//end if loop
                           }//end while function 
                           
                           if (single.current_centrality==0)
                           {printf("Error finding source ID...\n");
                           }//end if loop
                           
                           //Compute centrality adjusted result
                           single.adjusted_rating=(single.adjusted_rating*(single.lambda*single.current_centrality));                                                           
                           
                           //Sum up ratings
                           single.rolling_rating_sum=(single.rolling_rating_sum+single.adjusted_rating);
                           }//end inner while loop - this is exited when the next target id is met (so sum complete, can move onto next stage)
                     }//end if loop         
                 }//end outerwhile loop
            }//end else loop
     
     //Close files
     fclose(centrality);
     fclose(lambda);
     fclose(contact_list);

     //Output result
     printf("User ID %i score is: %2.8f\n",single.user_score, single.rolling_rating_sum);
     getchar();
     
 }//end find_users_score function

int main()
{find_users_score();
}//end main
