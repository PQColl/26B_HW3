/*
 
 Binary Files
 
 There are a number of errors (about 6) in the following program.
 Locate all errors, fix them (as shown below), run the program and save its output.
 
 Here is an example:
 
 FILE fp; // <== Error: Comment the line and write the correct line below
 
 // FILE fp; // Error #1
 FILE *fp;
 
 NAME: Patrick Collins
 IDE: Visual Studio
 */
#include <stdio.h>
#include <stdlib.h>  // exit
#include <string.h>
#include <ctype.h>

#define FILENAME "out.dat"

#define SHOW_RECORDS(x) rewind(fp); \
fread(temp, sizeof(RECORD), x, fp); \
for (k = 0; k < x; k++ ) printf("%s  %s\n", temp[k].id, temp[k].name); \
rewind(fp);

typedef struct
{
    //char name[32];	// ERROR 1
    //char id[4];		// ERROR 2
	char id[5];
	char name[32];
} RECORD;

int main(void)
{
    FILE *fp;
    RECORD list[5] = {{"1111", "One"}, {"2222", "Two"}, {"3333", "Three"}, {"4444", "Four"}, {"5555","Five"}};
    RECORD myList[3] = {{"7777", "Seven"}, {"8888", "Eight"}, {"9999", "Nine"}};
    //RECORD temp[5], item = {"6666", "Six"}, newItem;	// ERROR 3
	RECORD temp[9], item = { "6666", "Six" }, newItem;
    long pos, cnt;
	int k, n = 5;
    
    
    if ((fp = fopen(FILENAME, "w+b")) == NULL)
    {
        printf("Could not open %s.\n", FILENAME);
        exit(1);
    }
    //fwrite(list, 5, sizeof(RECORD), fp);	// ERROR 4
	fwrite(list, sizeof(RECORD), 5, fp);
    SHOW_RECORDS(5);
    
    // calculate the number of records in the file
    fseek(fp, 0, SEEK_END);
    pos = ftell(fp);
    printf("\nThere are %ld records in this file.\n", pos/sizeof(RECORD) );
    
    // append one item
    fseek(fp, 0, SEEK_END);
    //fwrite(item, sizeof(RECORD), 1, fp);	// ERROR 5
	fwrite(&item, sizeof(RECORD), 1, fp);
    
    // calculate the number of records in the file
    fseek(fp, 0, SEEK_END);
    pos = ftell(fp);
    printf("\nNow there are %ld records in this file.\n", pos/sizeof(RECORD) );
    
    // Display the last record
    //fseek(fp, -1 * sizeof(RECORD), SEEK_END);	// ERROR 6
	fseek(fp, -1 * (signed) sizeof(RECORD), SEEK_END);
    fread(&newItem, sizeof(RECORD), 1, fp);
    printf("\nLast item is %s.\n", newItem.name);
	
    // append myList
    fseek(fp, 0, SEEK_END);
    fwrite(myList, sizeof(RECORD), 3, fp);
	
    // calculate the number of records in the file
    fseek(fp, 0, SEEK_END);
    pos = ftell(fp);
    //cnt =  pos/sizeof("RECORD");	// ERROR 7
	cnt = pos / sizeof(RECORD);
	
    printf("\n\nThere are %ld records in this file.\n", cnt );
	
    SHOW_RECORDS(cnt);

    // replace the n-th element by {"0000", "ZERO"} // newItem
   // fseek(fp, n * sizeof(RECORD), SEEK_SET); // ERROR 8
	fseek(fp, (n - 1) * sizeof(RECORD), SEEK_SET);
    strcpy(item.id, "0000");
    strcpy(item.name, "ZERO");
    fwrite(&item, sizeof(RECORD), 1, fp);
    //fseek(fp, -1 * sizeof(RECORD), SEEK_END);	// ERROR 9
	fseek(fp, -1 * (signed) sizeof(RECORD), SEEK_CUR);
    fread(&newItem, sizeof(RECORD), 1, fp);
    printf("\n\nThe %d-th record has changed: %s %s.\n", n, newItem.id, newItem.name);
    SHOW_RECORDS(cnt);
    
    fclose(fp);
	system("pause");
    return 0;
}

/*	================= Sample Output ================= */
/*	Results:
 
 1111  One
2222  Two
3333  Three
4444  Four
5555  Five

There are 5 records in this file.

Now there are 6 records in this file.

Last item is Six.


There are 9 records in this file.
1111  One
2222  Two
3333  Three
4444  Four
5555  Five
6666  Six
7777  Seven
8888  Eight
9999  Nine


The 5-th record has changed: 0000 ZERO.
1111  One
2222  Two
3333  Three
4444  Four
0000  ZERO
6666  Six
7777  Seven
8888  Eight
9999  Nine
Press any key to continue . . .

 */
