#include <mysql.h>
#include <stdio.h>
//#include <string.h>

int main() 
{	//Mysql variables - note how they are all pointers
	MYSQL *conn;
	MYSQL_ROW row;
	MYSQL_RES *result;
	MYSQL_FIELD *field;
	
	int num_fields;
	int i;	
	char *server = "localhost";
	char *user = "root";
	char *password = "";//Only bit to edit here
	char *database = "Fidei";
	
	conn = mysql_init(NULL);
	
	//test that connection was valid
	if (conn == NULL) 
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		return(0);
	}
	else {
		printf("Connection starting...\n");
	     }
	
	
	/* Connect to database */
	printf("Connecting to mysql and opening database %s...\n", database);
	if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)) 
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
		return(0);
	}
	else {printf("Connected.\n");}
	
	//Get mysql client version
	printf("MySQL client version: %s\n", mysql_get_client_info());
	
	//Create a MySQL database
	if (mysql_query(conn, "create database testdb")) 
	{
		printf("Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		//return(0); - cancelled out as database will exist
	}
	
	//Create a mysql table
	printf("Creating table writers and adding data into it...\n");
	//mysql_query(conn, "CREATE TABLE writers(name VARCHAR(25))");
	
	//Insert some data into that table - this will go into 'Fidei' as that's what was connected to initially 
	mysql_query(conn, "INSERT INTO writers(name) VALUES('Leo Tolstoy')");
	mysql_query(conn, "INSERT INTO writers(name) VALUES('Jack London')");
	mysql_query(conn, "INSERT INTO writers(name) VALUES('Honore de Balzac')");
	mysql_query(conn, "INSERT INTO writers(name) VALUES('Lion Feuchtwanger')");
	mysql_query(conn, "INSERT INTO writers(name) VALUES('Emile Zola')");

	mysql_free_result(result);
	
	/*
	printf("Entering variable area");
	//Search by variable
	char *test_name="Leo Tolstoy";
	char *search_1="SELECT '";
	char *search_2="' FROM writers";
	printf("Strings defined\n");
	
	
	search_1=strcat(search_1, test_name);
	strcat(search_1, search_2);
	getchar();
	printf("String concatenation: %s",search_1);
	
	//char search=("SELECT '" + %s + "' FROM writers",user_id);
		
	mysql_query(conn, search_1);
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	printf("SELECT AND ROW ARRAY TEST 2: %s, %s \n",row[0], row[1]);
	*/
	
	 
	//Select 'Leo Tolstoy' from table and display that name
	
	//SELECT [column] FROM [table]
	//i.e. "SELECT user_id FROM writers WHERE name='Leo Tolstoy' ORDER BY user_id DESC;"
	//i.e "SELECT Count (user_id) FROM writers WHERE...." - so returns number of matched searches
	//UPDATE changes names: "UPDATE table SET name = 'new variable' WHERE name='xyz' 
	
	mysql_query(conn, "SELECT name, id FROM writers WHERE name='Leo Tolstoy'");
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	printf("SELECT AND ROW ARRAY TEST: %s, %s. \n",row[0],row[1]);
	
	//Now using a string
	char *test_string="SELECT name, id FROM writers WHERE name='Leo Tolstoy'";
	mysql_query(conn, test_string);
	result = mysql_store_result(conn);
	row = mysql_fetch_row(result);
	printf("SELECT AND ROW ARRAY TEST_2: %s, %s. \n",row[0],row[1]);
	
	
	printf("Now pulling in all data from writers...:\n\n");
	//Now connect to table writers in database fidei - this gets all of the names stored
	mysql_query(conn, "SELECT * FROM writers");
	
	//This gets the results set
	result = mysql_use_result(conn);
	
	//Find the number of fields (columns)
	num_fields = mysql_num_fields(result);
	
	//Now for rows 0 to number of number of rows
	while ((row = mysql_fetch_row(result)) != NULL)
	{   //Get all of the fields (i.e. 1)
		for(i = 0; i < num_fields; i++)
		{
			printf("%s ", row[i] != NULL ? row[i] : "NULL");
		}
		printf("\n");
	}
	
	
	//Create another mysql table
	printf("Creating table friends and adding data into it...\n");
	
	//mysql_query(conn, "CREATE TABLE friends(name VARCHAR(25))");
	
	mysql_query(conn, "CREATE TABLE friends(id int not null primary key auto_increment, name VARCHAR(20), age int)");
	
	mysql_query(conn, "INSERT INTO friends (name, age) VALUES('Tom', 25)");
	mysql_query(conn, "INSERT INTO friends (name, age) VALUES('Elisabeth', 32)");
	mysql_query(conn, "INSERT INTO friends (name, age) VALUES('Jane', 22)");
	mysql_query(conn, "INSERT INTO friends (name, age) VALUES('Luke', 28)");
	
	
	//CREATE TABLE, DELETE, DO, INSERT, REPLACE, SELECT, SET, UPDATE, and most variations of SHOW
	printf("Performing delete function...\n");
	
	if (mysql_query(conn, "DELETE FROM friends (name, age) VALUES('Tom', 25)")) 
	{
		printf("delete Error %u: %s\n", mysql_errno(conn), mysql_error(conn));
		//return(0); - cancelled out as database will exist
	}
	
	
	//***Function to search friends table until it finds a value, then return the row number
	mysql_query(conn, "SELECT * FROM friends");
	result = mysql_store_result(conn);
	num_fields = mysql_num_fields(result);
	row = mysql_fetch_row(result);
	
	
	printf("There are %i number of fields...\n",num_fields);
	
	while ((row = mysql_fetch_row(result)) != NULL)
	{   //Get all of the fields (i.e. 1)
		for(i = 0; i < num_fields; i++)
		{
			printf("%s ", row[i] ? row[i] : "NULL");
		}
		printf("\n");
	}
	printf("End search function.\n\n");
	
	
	printf("Now printing data from 'friends'...\n\n");
	//Now select all data from friends table
	mysql_query(conn, "SELECT * FROM friends");
	//Store the result
	result = mysql_store_result(conn);
	
	//Num of fields again found (this time it's 2)
	num_fields = mysql_num_fields(result);
	
	//And print the results again while all the rows to result exist
	while ((row = mysql_fetch_row(result)))
	{   //and then scan all fields
		for(i = 0; i < num_fields; i++)
		{
			if (i == 0) 
			{   //and for the first line, print the field name (note pointer field->name)
				while(field = mysql_fetch_field(result)) 
				{
					printf("%s ", field->name);
				}//end while loop
				printf("\n");
			}//end if loop
			printf("%s  ", row[i] ? row[i] : "NULL");
		}
	}
	printf("\n\n");
	
	getchar();
	//Select testdb
	database="testdb";
	if (mysql_select_db(conn, database));
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
	}
	
	//Select Fidei db
	database="Fidei";
	if (mysql_select_db(conn, database));
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
	}
	
	
	// send SQL query to find which tables exist
	if (mysql_query(conn, "show tables")) 
	{
		fprintf(stderr, "%s\n", mysql_error(conn));
	}
	
	result = mysql_use_result(conn);
	
	// output table name - this then takes the result (number of tables) and fetches them
	printf("MySQL Tables in mysql database:\n");
	while ((row = mysql_fetch_row(result)) != NULL)
		printf("%s \n", row[0]);
	
	getchar();
	
	printf("Closing mysql connection...\n");
			   
	/* close connection */
	mysql_free_result(result);
	mysql_close(conn);
			   
	printf("Mysql connection closed.\n");
	
}