/**
CIS 26B - Advanced C Programming
Homework #3: 
 Hashing to a file and using advanced string manipulation functions.
 
 This program allows additions to, deletions from, or displays of database records in a toy database.

 NAME: Patrick Collins
 IDE: Visual Studio
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define TABSIZE 40
#define BATCHSIZE 10
#define BUCKETSIZE 3
#define NAMESIZE 64
#define IDSIZE 5
#define MAXAGE 16
#define MINAGE 0
#define MAXPRICE 550.0
#define MINPRICE 1.0
#define HASHFILE "out.dat"
#define FILENAME "toys.txt"
#define REJECTFILE "rejected.txt"
#define FLUSH while(getchar() != '\n')

enum MENU_STATE {SEARCH = 1, DELETE = 2, INSERT = 3, EXIT = 4};
enum INSERT_STATE {FROM_FILE = 1, FROM_USER = 2, BACK = 3};

typedef struct {
	char toy_id[IDSIZE];
	char name[NAMESIZE];
	int age;
	double price;
	char award;
} RECORD;

FILE *create_hash_file(char *filename);
void read_records(FILE *fp, char *filename);
void insert_record(RECORD *record, long loc, FILE *fp);
void insert_rejected(RECORD *record);
void search_record(char *toy_id, long loc, FILE *fp);
void delete_record(char *toy_id, long loc, FILE *fp);
char *toAllCaps(char *str);
long hash(char *key, int size);
void insert_new_record(FILE *fp, char *s);
int validateID(char *toy_id);
int validateName(char *name);
int validateAge(char *age);
double validatePrice(char *price);
int validateAward(char *award);
void menuManager(FILE *fp);
void searchManager(FILE *fp);
void deleteManager(FILE *fp);
void insertionManager(FILE *fp);

void printMainMenu();
void printSearchMenu();
void printDeleteMenu();
void printInsertionMenu();
void print_records(FILE *fp);

int main(int argc, char *argv[])
{
	FILE *fp;
	printf("Initializing hashed binary file....\n");
	fp = create_hash_file(HASHFILE);
	printf("Reading records into hashed binary file.....\n");

	if (!argv[1])
		read_records(fp, FILENAME);
	else
		read_records(fp, argv[1]);
	printf("Reading completed.\n");

	menuManager(fp);
	print_records(fp);
	fclose(fp);
	system("pause");
    return 0;
}

/************************************* printMainMenu ****************************************
 * prints the display for the main menu
 *		PRE:  none
 *		POST: none
 ********************************************************************************************/

void printMainMenu()
{
	printf("\n------------------------ TOY RECORD MANAGER ---------------------------\n"
		   "   Hello! Welcome to the Toy Record Manager! Here you can search the   \n"
		   "   toy database, delete records from the database, and insert new      \n"
		   "   records either from a file or from your input. Please enter the     \n"
		   "   number to access the function you wish to perform.\n"
		   "\t\t1: Search\n"
		   "\t\t2: Delete\n"
		   "\t\t3: Insert\n"
		   "\t\t4: EXIT\n"
		   "-----------------------------------------------------------------------\n");
}

/************************************* printSearchMenu *************************************
* prints the display for the search menu
*		PRE:  none
*		POST: none
********************************************************************************************/
void printSearchMenu()
{
	printf("------------------------ TOY RECORD SEARCHER --------------------------\n"
		   "   To search for a toy in the record database enter the ID, a 4		  \n"
		   "   character key, when prompted. The ID must only contain digits. The \n"
		   "   search will fail if it does not.									  \n"
		   " 		 Example of valid input: 4112								  \n"
		   "         Enter QUIT as the id to return to the main menu.			  \n"
		   "-----------------------------------------------------------------------\n");
}

/************************************* printDeleteMenu *************************************
* prints the display for the delete menu
*		PRE:  none
*		POST: none
********************************************************************************************/
void printDeleteMenu()
{
	printf("------------------------ TOY RECORD DELETER ---------------------------\n"
		   "   To delete a toy from the record database enter the ID, a 4		  \n"
		   "   character key, when prompted. The ID must only contain digits. The \n"
		   "   deletion will fail if the toy is not in the record.				  \n"
		   " 		 Example of valid input: 4112								  \n"
		   "         Enter QUIT as the id to return to the main menu.			  \n"
		   "-----------------------------------------------------------------------\n");
}

/********************************* printInsertionMenu **************************************
* prints the display for the insertion menu
*		PRE:  none
*		POST: none
********************************************************************************************/
void printInsertionMenu()
{
	printf("------------------------ TOY RECORD INSERTER --------------------------\n"
		   "   You can either insert a single toy, from your input, or multiple	  \n"
		   "   toys from a file. When inserting a single toy, make sure to follow \n"
		   "   the proper format. Your toy record will not be inserted if it is   \n"
		   "   formatted properly!												  \n"
		   "\t\t1: Insert from file.\n"
		   "\t\t2: Insert single toy.\n"
		   "\t\t3: Back to MAIN MENU. \n"
		   "-----------------------------------------------------------------------\n");
}

/************************************* menuManager *****************************************
* manages the main menu for the program that allows the user to search, delete, and insert
* toy records.
*		PRE:  fp - pointer to FILE
*		POST: none
********************************************************************************************/
void menuManager(FILE *fp)
{
	enum MENU_STATE state;
	
	do
	{
		printMainMenu();
		printf("Please enter your choice: ");
		scanf("%d", &state);
		FLUSH;

		switch (state)
		{
			case(SEARCH):
				searchManager(fp);
				break;
			case(DELETE):
				deleteManager(fp);
				break;
			case(INSERT):
				insertionManager(fp);
				break;
			case(EXIT):
				printf("Exiting program... contents of the database will be printed shortly...\n\n");
				break;
			default:
				printf("Invalid choice. Please enter the number of the function you wish to perform.\n");
				break;
		} 
	}while (state != EXIT);
}

/************************************* searchManager ***************************************
* manages the search for a toy from the record binary file
*		PRE:  fp - pointer to file
*		POST: none
********************************************************************************************/
void searchManager(FILE *fp)
{
	char toy_id[5];
	long loc;

	printSearchMenu();

	do
	{
		printf("Please enter an ID: ");
		scanf("%4s", toy_id);
		FLUSH;
		if (strcmp(toy_id, "QUIT"))
		{
			loc = hash(toy_id, TABSIZE);
			search_record(toy_id, loc, fp);
		}	
	} while (strcmp(toy_id, "QUIT"));
}

/************************************* deleteManager ***************************************
* manages the deletion of a toy from the record binary file
*		PRE:  fp - pointer to file
*		POST: none
********************************************************************************************/
void deleteManager(FILE *fp)
{
	char toy_id[5];
	long loc;

	printDeleteMenu();

	do
	{
		printf("Please enter an ID: ");
		scanf("%4s", toy_id);
		FLUSH;
		if (strcmp(toy_id, "QUIT"))
		{
			loc = hash(toy_id, TABSIZE);
			delete_record(toy_id, loc, fp);
		}
	} while (strcmp(toy_id, "QUIT"));
}

/************************************* insertionManager *************************************
* manages the insertion of new record(s) to the file. Can be done either with a file or
* user input.
*		PRE:  fp - pointer to file
*		POST: none
********************************************************************************************/
void insertionManager(FILE* fp)
{
	char input[256];
	enum INSERT_STATE state;

	printInsertionMenu();

	do
	{
		printf("Please enter your choice: ");
		scanf("%d", &state);
		FLUSH;

		switch (state)
		{
			case(FROM_FILE):
				printf("Please enter the file of toy records you wish to insert: ");
				scanf("%255s", input);
				FLUSH;
				read_records(fp, input);
				break;
			case(FROM_USER):
				printf("Please enter the record you wish to insert: ");
				scanf("%255[^\n]", input);
				FLUSH;
				insert_new_record(fp, input);
				break;
			default:
				printf("Invalid menu choice.\n");
				break;
		}
	} while (state != BACK);
}

/************************************* create_hash_file ************************************
* creates the empty hashed binary file.
*		PRE:  filename - name of the file
*		POST: fp - pointer to the binary file
********************************************************************************************/
FILE *create_hash_file(char *filename)
{
	int i;
	FILE *fp;
	RECORD hashtable[BATCHSIZE][BUCKETSIZE] = { "", "", 0, 0, ' ' };

	if ((fp = fopen(filename, "w+b")) == NULL)
	{

		printf("Could not open %s.\n", filename);
		exit(1);
	}

	for (i = 0; i < TABSIZE / BATCHSIZE; i++)
	{
		fwrite(&hashtable[0][0], sizeof(RECORD), BATCHSIZE * BUCKETSIZE, fp);
	}

	rewind(fp);
	return fp;
}

/************************************* read_records ****************************************
* reads records from a text file and inserts them into a binary file.
*		PRE:  fp - pointer to file
*			  filename - name of textfile
*		POST: contents of textfile stored in binary file
********************************************************************************************/
void read_records(FILE *fp, char *filename)
{
	FILE *fpRecords;
	RECORD newRecord;
	long loc;

	fpRecords = fopen(filename, "r");
	if (!fpRecords)
	{
		printf("Invalid file name %s!!\n", filename);
		return;
	}
		
	while (fscanf(fpRecords, "%s %[^;]%*c %d %lf %c", newRecord.toy_id, newRecord.name, &newRecord.age, &newRecord.price, &newRecord.award) > 0)
	{
		strcpy(newRecord.name, toAllCaps(newRecord.name));
		loc = hash(newRecord.toy_id, TABSIZE);
		insert_record(&newRecord, loc, fp);
	}
	rewind(fp);
	fclose(fpRecords);
}

/************************************* insert_record ***************************************
* inserts the passed record into the binary file at the correct location. writes to a
* reject file if insertion is not successful.
*		PRE:  record - record to be inserted
*			  loc    - location in file to insert
*			  fp	 - pointer to binary file
*		POST: record inserted in binary file, inserted in reject file if unsuccessful
********************************************************************************************/
void insert_record(RECORD *record, long loc, FILE *fp)
{
	RECORD detect;
	int duplicate = 0;
	int i;

	if (fseek(fp, loc * BUCKETSIZE * sizeof(RECORD), SEEK_SET) != 0)
	{
		printf("Fatal seek error! Abort!\n");
		exit(4);
	}

	//CHANGE DUPLICATE CHECK
	for (i = 0; i < BUCKETSIZE && !duplicate; i++)
	{
		fread(&detect, sizeof(RECORD), 1, fp);
		if (!strcmp(detect.toy_id, record->toy_id))
			duplicate = 1;
		if (*detect.toy_id == '\0' && !duplicate) // available slot
		{
			fseek(fp, -1L * (signed) sizeof(RECORD), SEEK_CUR);
			fwrite(record, sizeof(RECORD), 1, fp);
			return; // nothing left to do
		}
	}
	insert_rejected(record);
}

/************************************* insert_rejected *************************************
* appends the rejected record to file
*		PRE:  record - record to append
*		POST: record appended to reject file
********************************************************************************************/
void insert_rejected(RECORD *record)
{
	FILE *fp;
	fp = fopen(REJECTFILE, "a");

	printf("%s cannot be inserted. Writing to %s...\n", record->name, REJECTFILE);
	fprintf(fp, "%s %s; %d %.2f %c\n", record->toy_id, record->name, record->age, record->price, record->award);

	fclose(fp);
}

/************************************* search_record ****************************************
* searches the hashed binary file for a record
*		PRE:  toy_id - the search target
*			  loc    - location in the binary file to search
*			  fp     - pointer to the binary file
*		POST: the record is displayed if it is found
********************************************************************************************/
void search_record(char *toy_id, long loc, FILE *fp)
{
	RECORD detect;
	int i;

	if (fseek(fp, loc * BUCKETSIZE * sizeof(RECORD), SEEK_SET) != 0)
	{
		printf("Fatal seek error! Abort");
		exit(4);
	}
	// find first available slot in bucket
	for (i = 0; i < BUCKETSIZE; i++)
	{
		fread(&detect, sizeof(RECORD), 1, fp);
		if (strcmp(detect.toy_id, toy_id) == 0) // found it!
		{
			printf("\t%s found \n\tat hash bucket %ld.\n", toy_id, loc);
			printf("\tName: %s\n\tID: %s\n\tAge: %d\n\tPrice: %.2f\n\tAward: %c\n", 
									detect.name, detect.toy_id, detect.age, detect.price, detect.award);
			return; // nothing left to do
		}
	}
	// not found
	printf("Record with key: %s : not found.\n", toy_id);
	return;
}

/************************************* delete_record ***************************************
* deletes the record from the binary file
*		PRE:  toy_id - the record to be deleted
*			  loc    - location in the binary file
*			  fp     - pointer to binary file
*		POST: record is deleted from the binary file.
********************************************************************************************/
void delete_record(char *toy_id, long loc, FILE *fp)
{
	RECORD detect, empty = {"", "", 0, 0, ' '};
	int i;

	if (fseek(fp, loc * BUCKETSIZE * sizeof(RECORD), SEEK_SET) != 0)
	{
		printf("Fatal seek error! Abort");
		exit(5);
	}

	for (i = 0; i < BUCKETSIZE; i++)
	{
		fread(&detect, sizeof(RECORD), 1, fp);
		if (strcmp(detect.toy_id, toy_id) == 0)
		{
			fseek(fp, -1L * (signed)sizeof(RECORD), SEEK_CUR);
			fwrite(&empty, sizeof(RECORD), 1, fp);
			printf("\tRecord for %s at %ld deleted.\n", detect.name, loc);
			return;
		}
	}
	printf("Record could not be found; not deleted.\n");
}

/************************************* insert_new_record ************************************
* inserts a new record based on user input. Validates the input before insertion.
*		PRE:  fp - pointer to binary file
*			  s  - user entered record
*		POST: record added to the binary file if the input is valid1
********************************************************************************************/
void insert_new_record(FILE *fp, char *s)
{
	RECORD newRecord;
	int age;
	double price;

	if (!strpbrk(s, ";"))
	{
		printf("Missing name separator! (;)\n");
		return;
	}

	char *sec = strtok(s, "\040\t\n");

	if (!validateID(sec))
		return;
	strcpy(newRecord.toy_id, sec);

	sec = strtok(NULL, ";");

	if (!validateName(sec))
		return;
	strcpy(newRecord.name, toAllCaps(sec));


	sec = strtok(NULL, "\040\t\n");
	age = validateAge(sec);

	if (age == -1)
		return;
	newRecord.age = age;

	sec = strtok(NULL, "\040\t\n");
	price = validatePrice(sec);

	if (!price)
		return;
	newRecord.price = price;

	sec = strtok(NULL, "\040\t\n");
	if (!validateAward(sec))
		return;

	newRecord.award = *sec;

	long loc = hash(newRecord.toy_id, TABSIZE);
	printf("Inserting new record.\n");
	insert_record(&newRecord, loc, fp);
	rewind(fp);
}

/************************************* validateID ******************************************
* validates the ID portion of the new record.
*		PRE:  toy_id - the new ID
*		POST: 0 - if the ID is not formatted properly
*			  1 - if the ID is valid
********************************************************************************************/
int validateID(char *toy_id)
{
	if (strlen(toy_id) != IDSIZE - 1)
	{
		printf("Toy ID invalid. Size of ID must have exactly 4 characters.\n");
		return 0;
	}

	while(*toy_id)
	{
		if (!isdigit(*toy_id))
		{
			printf("Toy ID invalid. ID must consist entirely of numbers.\n");
			return 0;
		}
		toy_id++;
	}

	return 1;
}

/************************************* validateName ****************************************
* validates the name of the record
*		PRE:  name - the new name
*		POST: 0 - if the name is invalid
*			  1 - if the name is valid
********************************************************************************************/
int validateName(char *name)
{
	char s[NAMESIZE];
	if (!name)
	{
		printf("Missing name!\n");
		return 0;
	}

	if (strlen(name) > NAMESIZE - 1)
	{
		printf("Name is too long! Must be 63 characters or less!\n");
		return 0;
	}

	sscanf(name, "%[A-Za-z\040\t\'\":!-]", s);
	if(strcmp(name, s))
	{
		printf("Name contains invalid characters.\n");
		return 0;
	}

	return 1;
}

/************************************* validateAge *****************************************
* validates the age field of the new record
*		PRE:  age    - the new age field
*		POST: -1	 - if the age is invalid
*			  ageNum - int representation of the age field
********************************************************************************************/
int validateAge(char *age)
{
	char *check;
	int ageNum;

	if (!age)
	{
		printf("Missing age!\n");
		return -1;
	}
	ageNum = strtod(age, &check);

	if (*check != '\0')
	{
		printf("Age field is invalid. Age field must only contain characters..\n");
		return -1;
	}

	if (ageNum > MAXAGE)
	{
		printf("Age is too high. Maximum age is %d.\n", MAXAGE);
		return -1;
	}
	else if (ageNum < MINAGE)
	{
		printf("Age is too low. Minimum age is %d.\n", MINAGE);
		return -1;
	}
	else
		return ageNum;

}

/************************************* validatePrice ***************************************
* validates the price field of the new record
*		PRE:  price - the new price field
*		POST: 0		- if the price is not valid
*			  p		- if the price is valid, a double representation of the input
********************************************************************************************/
double validatePrice(char *price)
{
	char *check;
	double p = strtod(price, &check);
	if (*check != '\0')
	{
		printf("Price is not valid. The price must be a number between %.2f - %.2f.\n", MINPRICE, MAXPRICE);
		return 0;
	}

	if (p > MAXPRICE || p < MINPRICE)
	{
		printf("Price is not valid. Price must be a number between %.2f - %.2f.\n", MINPRICE, MAXPRICE);
		return 0;
	}
	
	return p;
}

/************************************* validateAward ***************************************
* validates the award field of the new record
*		PRE:  award - the new award field
*		POST: 0     - if the award is invalid
*			  1     - if the award is valid
********************************************************************************************/
int validateAward(char *award)
{
	if (strlen(award) > 1)
	{
		printf("Invalid award. Award entered is more than one character. Award must be only one character.\n");
		return 0;
	}

	if (*award == 'A' || *award == 'G' || *award == 'S' || *award == 'R')
		return 1;

	printf("Invalid award %1s. Valid awards are G, S, R, and A.\n", award);
	return 0;
}

/************************************* print_records ***************************************
* prints the records contained in the binary file. Skips over any empty positions
*		PRE:  fp - pointer to binary file
*		POST: none
********************************************************************************************/
void print_records(FILE *fp)
{
	RECORD temp;
	while (fread(&temp, sizeof(RECORD), 1, fp) > 0)
	{	
		if (*temp.toy_id != '\0')
			printf("%s %s; %d %.2f %c\n", temp.toy_id, temp.name, temp.age, temp.price, temp.award);
	}
	rewind(fp);
}

/************************************** toAllCaps ******************************************
* capitalizes all alphabetical characters in the string
*		PRE:  str - string to modify
*		POST: str - string with all caps
********************************************************************************************/
char *toAllCaps(char *str)
{
	char *temp = str;

	while (*temp)
	{
		if(isalpha(*temp))
			*temp = toupper(*temp);
		temp++;
	}
	return str;
}

/************************************** hash ***********************************************
 * hashes the key to get the location in the binary file
 *		PRE:  key  - key to hash
 *			  size - size of the table
 *		POST: the hashed location
 ********************************************************************************************/
long hash(char *key, int size)
{
	long address = 0;
	for (; *key != '\0'; key++)
	{
		address += *key * *key * *key;
	}
	return address % size;
}

/**************** OUTPUT 1 *****************/
/*
E:\code\cis26b\HW3\hw3\Debug>hw3.exe toys.txt
Initializing hashed binary file....
Reading records into hashed binary file.....
AARDVARK ADVENTURE TALES cannot be inserted. Writing to rejected.txt...
Reading completed.

------------------------ TOY RECORD MANAGER ---------------------------
Hello! Welcome to the Toy Record Manager! Here you can search the
toy database, delete records from the database, and insert new
records either from a file or from your input. Please enter the
number to access the function you wish to perform.
1: Search
2: Delete
3: Insert
4: EXIT
-----------------------------------------------------------------------
Please enter your choice: 1
------------------------ TOY RECORD SEARCHER --------------------------
To search for a toy in the record database enter the ID, a 4
character key, when prompted. The ID must only contain digits. The
search will fail if it does not.

Example of valid input: 4112

Enter QUIT as the id to return to the main menu.

-----------------------------------------------------------------------
Please enter an ID: 4359
4359 found
at hash bucket 9.
Name: THE PIRATE FAIRY: PIXIE DUST
ID: 4359
Age: 5
Price: 10.00
Award: A
Please enter an ID: 4567
4567 found
at hash bucket 4.
Name: SOCK PUPPET CHARADES
ID: 4567
Age: 8
Price: 24.99
Award: A
Please enter an ID: QUIT

------------------------ TOY RECORD MANAGER ---------------------------
Hello! Welcome to the Toy Record Manager! Here you can search the
toy database, delete records from the database, and insert new
records either from a file or from your input. Please enter the
number to access the function you wish to perform.
1: Search
2: Delete
3: Insert
4: EXIT
-----------------------------------------------------------------------
Please enter your choice: 2
------------------------ TOY RECORD DELETER ---------------------------
To delete a toy from the record database enter the ID, a 4
character key, when prompted. The ID must only contain digits. The
deletion will fail if the toy is not in the record.

Example of valid input: 4112

Enter QUIT as the id to return to the main menu.

-----------------------------------------------------------------------
Please enter an ID: 5219
Record for CAULDRON QUEST at 39 deleted.
Please enter an ID: 1235
Record for LASER MAZE at 17 deleted.
Please enter an ID: QUIT

------------------------ TOY RECORD MANAGER ---------------------------
Hello! Welcome to the Toy Record Manager! Here you can search the
toy database, delete records from the database, and insert new
records either from a file or from your input. Please enter the
number to access the function you wish to perform.
1: Search
2: Delete
3: Insert
4: EXIT
-----------------------------------------------------------------------
Please enter your choice: 3
------------------------ TOY RECORD INSERTER --------------------------
You can either insert a single toy, from your input, or multiple
toys from a file. When inserting a single toy, make sure to follow
the proper format. Your toy record will not be inserted if it is
formatted properly!

1: Insert from file.
2: Insert single toy.
3: Back to MAIN MENU.
-----------------------------------------------------------------------
Please enter your choice: 1
Please enter the file of toy records you wish to insert: more_toys.txt
MIX UP! FIX UP! MIX UP! FIX UP! cannot be inserted. Writing to rejected.txt...
Please enter your choice: 2
Please enter the record you wish to insert: 4112 toy tester; 5 24.99 G
Inserting new record.
Please enter your choice: 3
Invalid menu choice.

------------------------ TOY RECORD MANAGER ---------------------------
Hello! Welcome to the Toy Record Manager! Here you can search the
toy database, delete records from the database, and insert new
records either from a file or from your input. Please enter the
number to access the function you wish to perform.
1: Search
2: Delete
3: Insert
4: EXIT
-----------------------------------------------------------------------
Please enter your choice: 4
Exiting program... contents of the database will be printed shortly...

5392 Q'S RACE TO THE TOP; 3 24.99 R
4567 SOCK PUPPET CHARADES; 8 24.99 A
6745 COLOR CLASH; 7 14.99 G
8624 SHUTTLES; 8 24.99 S
3495 COMPOUND IT ALL!; 9 29.95 R
4359 THE PIRATE FAIRY: PIXIE DUST; 5 10.00 A
5934 READING POWER PACK; 3 11.99 R
1524 DINOSAUR ESCAPE; 4 17.99 S
7146 MAKER STUDIO: WINCHES SET; 7 19.99 S
2358 THUMBS UP!; 6 14.99 S
9162 DIMENSION; 8 49.95 S
4112 TOY TESTER; 5 24.99 G
2341 TUGIE; 5 29.99 G
1622 CREATURE CLASH! CARD GAME; 6 14.99 S
5675 RUSH HOUR; 8 24.99 G
3271 UBONGO; 8 39.95 S
1327 TIMES TABLES CHAMPION; 5 30.00 R
2756 CREATE-A-MAZE; 5 29.99 R
8123 MATH POWER PACK; 5 11.99 R
1832 IQ CANDY; 7 14.99 S
9524 FRANKIE'S FOOD TRUCK FIASCO GAME; 4 21.99 S
Press any key to continue . . .

*/

/*************** OUTPUT 2 ********************/
/*
E:\code\cis26b\HW3\hw3\Debug>hw3.exe
Initializing hashed binary file....
Reading records into hashed binary file.....
AARDVARK ADVENTURE TALES cannot be inserted. Writing to rejected.txt...
Reading completed.

------------------------ TOY RECORD MANAGER ---------------------------
Hello! Welcome to the Toy Record Manager! Here you can search the
toy database, delete records from the database, and insert new
records either from a file or from your input. Please enter the
number to access the function you wish to perform.
1: Search
2: Delete
3: Insert
4: EXIT
-----------------------------------------------------------------------
Please enter your choice: 1
------------------------ TOY RECORD SEARCHER --------------------------
To search for a toy in the record database enter the ID, a 4
character key, when prompted. The ID must only contain digits. The
search will fail if it does not.

Example of valid input: 4112

Enter QUIT as the id to return to the main menu.

-----------------------------------------------------------------------
Please enter an ID: 4112
Record with key: 4112 : not found.
Please enter an ID: f25s
Record with key: f25s : not found.
Please enter an ID: QUIT

------------------------ TOY RECORD MANAGER ---------------------------
Hello! Welcome to the Toy Record Manager! Here you can search the
toy database, delete records from the database, and insert new
records either from a file or from your input. Please enter the
number to access the function you wish to perform.
1: Search
2: Delete
3: Insert
4: EXIT
-----------------------------------------------------------------------
Please enter your choice: 2
------------------------ TOY RECORD DELETER ---------------------------
To delete a toy from the record database enter the ID, a 4
character key, when prompted. The ID must only contain digits. The
deletion will fail if the toy is not in the record.

Example of valid input: 4112

Enter QUIT as the id to return to the main menu.

-----------------------------------------------------------------------
Please enter an ID: 4112
Record could not be found; not deleted.
Please enter an ID: fqf23
Record could not be found; not deleted.
Please enter an ID: QUIT

------------------------ TOY RECORD MANAGER ---------------------------
Hello! Welcome to the Toy Record Manager! Here you can search the
toy database, delete records from the database, and insert new
records either from a file or from your input. Please enter the
number to access the function you wish to perform.
1: Search
2: Delete
3: Insert
4: EXIT
-----------------------------------------------------------------------
Please enter your choice: 3
------------------------ TOY RECORD INSERTER --------------------------
You can either insert a single toy, from your input, or multiple
toys from a file. When inserting a single toy, make sure to follow
the proper format. Your toy record will not be inserted if it is
formatted properly!

1: Insert from file.
2: Insert single toy.
3: Back to MAIN MENU.
-----------------------------------------------------------------------
Please enter your choice: 1
Please enter the file of toy records you wish to insert: more_toys.txt
MIX UP! FIX UP! MIX UP! FIX UP! cannot be inserted. Writing to rejected.txt...
Please enter your choice: 1
Please enter the file of toy records you wish to insert: more_toys.txt
RUSH HOUR cannot be inserted. Writing to rejected.txt...
MIX UP! FIX UP! MIX UP! FIX UP! cannot be inserted. Writing to rejected.txt...
TIMES TABLES CHAMPION cannot be inserted. Writing to rejected.txt...
READING POWER PACK cannot be inserted. Writing to rejected.txt...
DINOSAUR ESCAPE cannot be inserted. Writing to rejected.txt...
DIMENSION cannot be inserted. Writing to rejected.txt...
Please enter your choice: 2
Please enter the record you wish to insert: 4112 test toy; 5 25.99 B
Invalid award B. Valid awards are G, S, R, and A.
Please enter your choice: 4112 test toy; 5 2995.99 G
Invalid menu choice.
Please enter your choice: 2
Please enter the record you wish to insert: 4112 test toy; 5 2995.99 G
Price is not valid. Price must be a number between 1.00 - 550.00.
Please enter your choice: 2
Please enter the record you wish to insert: 4112 test toy 5 25.99 G
Missing name separator! (;)
Please enter your choice: 2
Please enter the record you wish to insert: 4112 test t,oy; 5 25.99 G
Name contains invalid characters.
Please enter your choice: 2
Please enter the record you wish to insert: 4FF2 test toy; 5 25.99 G
Toy ID invalid. ID must consist entirely of numbers.
Please enter your choice: BACK
Please enter the record you wish to insert: 3
Missing name separator! (;)
Please enter your choice: 3
Invalid menu choice.

------------------------ TOY RECORD MANAGER ---------------------------
Hello! Welcome to the Toy Record Manager! Here you can search the
toy database, delete records from the database, and insert new
records either from a file or from your input. Please enter the
number to access the function you wish to perform.
1: Search
2: Delete
3: Insert
4: EXIT
-----------------------------------------------------------------------
Please enter your choice: 4
Exiting program... contents of the database will be printed shortly...

5392 Q'S RACE TO THE TOP; 3 24.99 R
4567 SOCK PUPPET CHARADES; 8 24.99 A
6745 COLOR CLASH; 7 14.99 G
8624 SHUTTLES; 8 24.99 S
3495 COMPOUND IT ALL!; 9 29.95 R
4359 THE PIRATE FAIRY: PIXIE DUST; 5 10.00 A
5934 READING POWER PACK; 3 11.99 R
1524 DINOSAUR ESCAPE; 4 17.99 S
7146 MAKER STUDIO: WINCHES SET; 7 19.99 S
1235 LASER MAZE; 6 29.99 G
2358 THUMBS UP!; 6 14.99 S
9162 DIMENSION; 8 49.95 S
2341 TUGIE; 5 29.99 G
1622 CREATURE CLASH! CARD GAME; 6 14.99 S
5675 RUSH HOUR; 8 24.99 G
3271 UBONGO; 8 39.95 S
1327 TIMES TABLES CHAMPION; 5 30.00 R
2756 CREATE-A-MAZE; 5 29.99 R
8123 MATH POWER PACK; 5 11.99 R
1832 IQ CANDY; 7 14.99 S
9524 FRANKIE'S FOOD TRUCK FIASCO GAME; 4 21.99 S
5219 CAULDRON QUEST; 6 19.99 S
Press any key to continue . . .


*/