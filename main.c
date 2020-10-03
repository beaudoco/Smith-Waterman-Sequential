/*********************************************************
This program takes two given file names and processes
them to find the highest match score between the two
strings. It outputs the resulting array to a text file
so it can be compared to other outputs.

@author: Collin Beaudoin
@version: October 2020
*********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

/*********************************************************
This section is used to declare the methods that shall be
used throughout the program.
*********************************************************/

void processFile();
void checkMatches();
void createCSV(int **matchArr, int mainArrLen, int compareArrLen);

/*********************************************************
This is the main function of the code, it is used to
accept the parameters that shall be used throughout the
program.

@parameter mainFile: This is used to select the file that
will be parsed against to see if it has any matches
@parameter compareFile: This is used to select the file
that to parse the main file with for matches
*********************************************************/

int main()
{
    //DECLARE VARS
    char mainFile[100];
    char compareFile[100];

    //FLUSH INPUT AND READ FILE NAME
    fflush(stdin);
    printf("Enter a file name to upload: ");
    gets(mainFile);

    //FLUSH INPUT AND READ OUTPUT FILE NAME
    fflush(stdin);
    printf("Enter a file name to compare similarity to: ");
    gets(compareFile);

    //BEGIN FILE PROCESSING
    processFile(mainFile, compareFile);

    return 0;
}

/*********************************************************
This function is used to take the given files and process
the data into usable arrays.

@parameter mainFile: The name of the main file to process
@parameter compareFile: The name of the secondary file
to process
@return: none
*********************************************************/

void processFile(char *mainFile, char *compareFile)
{
   //DECLARE VARS
   FILE *mainFp;
   FILE *compareFp;
   int mainArrLen = 255;
   int compareArrLen = 255;
   char *mainBuff = NULL;
   char *compareBuff = NULL;
   int i = 0;
   int mainTotalLen = 0;
   char ch = ' ';

   //OPEN FILE AND ALLOCATE MEMORY FOR READING FILE
   mainFp = fopen(mainFile, "r");
   mainBuff = realloc(mainBuff, sizeof(char) * mainArrLen);

   //DO UNTIL THE END OF MAIN FILE
    while (ch != EOF)
    {
        //GET THE CHARACTER
        ch = fgetc(mainFp);

        //CHECK IF THERE IS ENOUGH MEMORY TO WRITE TO MAIN BUFFER
        //IF NOT ALLOCATE MORE
        if (i >= mainArrLen)
            {
                mainArrLen = mainArrLen * 2;
                mainBuff = realloc(mainBuff, sizeof(char) * mainArrLen);
            }

        //FILTER OUT UNWANTED CHARACTERS
        if (isalpha(ch) || ch == '?')
        {
            mainBuff[i++] = tolower(ch);
        }
    }
    //END STRING FOR MAIN BUFFER
    mainBuff[i] = '\0';

    //RESET THE CHARACTER AND RESET THE INCREMENTER
    mainTotalLen = i;
    ch = ' ';
    i = 0;

    //OPEN FILE AND ALLOCATE MEMORY FOR READING FILE
    compareFp = fopen(compareFile, "r");
    compareBuff = realloc(compareBuff, sizeof(char) * compareArrLen);

    //DO UNTIL THE END OF COMPARE FILE
    while (ch != EOF)
    {
        //GET THE CHARACTER
        ch = fgetc(compareFp);

        //CHECK IF THERE IS ENOUGH MEMORY TO WRITE TO COMPARE BUFFER
        //IF NOT ALLOCATE MORE
        if (i >= compareArrLen)
            {
                compareArrLen = compareArrLen * 2;
                compareBuff = realloc(compareBuff, sizeof(char) * compareArrLen);
            }

        //FILTER OUT UNWANTED CHARACTERS
        if (isalpha(ch) || ch == '?')
        {
            compareBuff[i++] = tolower(ch);
        }
    }
    //END STRING OF COMPARE BUFFER
    compareBuff[i] = '\0';

    //CLOSE FILE READ
    fclose(mainFp);
    fclose(compareFp);

    //CHECK THE COMPLEXITY OF OUR INFORMATION
    checkMatches(mainBuff, mainTotalLen, compareBuff, i);

    //FREE MEMORY
    free(mainBuff);
    free(compareBuff);
}

/*********************************************************
This function is used to iterate over the main file's
data and compare it to the secondary file to check for
the highest matching score

@parameter mainBuff: The main file data to be processed.
@parameter mainArrLen: The overall size of the main file
@parameter compareBuff: The secondary file data to be
compared against the main
@parameter compareArrLen: The overall size of the
secondary file
@return: none
*********************************************************/

void checkMatches(char *mainBuff, int mainArrLen, char *compareBuff, int compareArrLen)
{
    //DECLARE VARS
    int **matchArr = {0};
    int i = 0, j = 0;

    //SET UP TIMER FOR FILE
    struct timespec begin, end;
    clock_gettime(CLOCK_REALTIME, &begin);

    //ALLOCATE MEMORY FOR COUNT
    matchArr = realloc(matchArr, sizeof(int*) * mainArrLen * 2);

    //ITERATE OVER THE SMALLER FILE AS THIS WILL BE THE COLUMN
    for (i = 0; i < compareArrLen + 1; i++)
    {
        //ALLOCATE ENOUGH MEMORY FOR THE ROW
        matchArr[i] = malloc(sizeof(int) * mainArrLen * 2);

        //ITERATE OVER THE MAIN FILE CHECKING FOR SMITH-WATERMAN MATCH SCORE
        for (j = 0; j < mainArrLen + 1; j++)
        {
            //IF IT IS THE FIRST ROW/COLUMN WE KNOW IT MUST BE 0
            if (i == 0 || j == 0)
            {
                matchArr[i][j] = 0;
            } else
            {
                //CHECK IF THE ROW AND COLUMN CHARACTER MATCH AND WRITE ITS SCORE
                if (compareBuff[i-1] == mainBuff[j-1] || compareBuff[i-1] == '?' || mainBuff[j-1] == '?')
                {
                    matchArr[i][j] = matchArr[i-1][j-1] + 1;
                } else
                {
                    //CHECK LEFT
                    int leftSide =  matchArr[i-1][j] - 2;
                    //CHECK UP
                    int upSide =  matchArr[i][j-1] - 2;
                    //CHECK UP LEFT
                    int upLeftSide = matchArr[i-1][j-1] - 1;

                    //COMPARE TO SEE WHAT THE LARGEST VALUE IS AND WRITE IT
                    if (leftSide > 0 || upSide > 0 || upLeftSide > 0)
                    {
                        if (leftSide > upSide)
                        {
                            if (leftSide > upLeftSide)
                            {
                                matchArr[i][j] = leftSide;
                            } else
                            {
                                matchArr[i][j] = upLeftSide;
                            }
                        } else if (upSide > upLeftSide)
                        {
                            matchArr[i][j] = upSide;
                        } else
                        {
                            matchArr[i][j] = upLeftSide;
                        }
                    } else
                    {
                        matchArr[i][j] = 0;
                    }
                }
            }
        }
    }

    //END CLOCK AND GET TIME
    clock_gettime(CLOCK_REALTIME, &end);

    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    printf("time taken %f\n",elapsed);

    //OUTPUT THE MATCH SCORE ARRAY
    createCSV(matchArr, mainArrLen, compareArrLen);

    //FREE MEMORY
    free(matchArr);
}

/*********************************************************
This function is used to output the processed match score.
This is for comparison purpose to ensure the program works

@parameter matchArr: The file data to be output.
@parameter mainArrLen: The overall row sizes
@parameter compareArrLen: The overall column sizes
@return: none
*********************************************************/

void createCSV(int **matchArr, int mainArrLen, int compareArrLen)
{
    //DECLARE VARS
    FILE *filep;
    int i = 0, j = 0;

    //OPEN FILE
    filep = fopen("test.txt", "w+");

    //WRITE RESULTS TO FILE
    for(i = 0; i < mainArrLen +1; i++)
    {
        for (j = 0; j < compareArrLen +1; j++)
        {
            fprintf(filep,"%d,", matchArr[j][i]);
        }
        fprintf(filep,"\n");
    }

    //CLOSE FILE
    fclose(filep);

    //LET USER KNOW PROGRAM IS DONE
    printf("file created \n");
}






































//
//    for (i = 0; i < mainArrLen + 1; i++)
//    {
//        for (j = 0; j < compareArrLen + 1; j++)
//        {
//            matchArr[j] = malloc(sizeof(int) * mainArrLen * 2);
//            if (i == 0)
//            {
//                matchArr[j][i] = 0;
//            } else
//            {
//                if (j == 0)
//                {
//                    matchArr[j][i] = 0;
//                } else
//                {
//                    //IF MATCH INCREMENT FROM TOP LEFT
//                    //printf("%c %c \n", compareBuff[j-1], mainBuff[i-1]);
//                    if (compareBuff[j-1] == mainBuff[i-1] || compareBuff[j-1] == '?' || mainBuff[j-1] == '?')
//                    {
//                        matchArr[j][i] = matchArr[j-1][i-1] + 1;
//                        printf("%d %d %d %d\n",j,i, matchArr[j][i],matchArr[j-1][i-1]);
//                    } else
//                    {
//                        //CHECK LEFT
//                        int leftSide =  matchArr[j-1][i] - 2;
//                        //CHECK UP
//                        int upSide =  matchArr[j][i-1] - 2;
//                        //CHECK UP LEFT
//                        int upLeftSide = matchArr[j-1][i-1] - 1;
//
//                        if (leftSide > 0 || upSide > 0 || upLeftSide > 0)
//                        {
//                            if (leftSide > upSide)
//                            {
//                                if (leftSide > upLeftSide)
//                                {
//                                    matchArr[j][i] = leftSide;
//                                } else
//                                {
//                                    matchArr[j][i] = upLeftSide;
//                                }
//                            } else if (upSide > upLeftSide)
//                            {
//                                matchArr[j][i] = upSide;
//                            } else
//                            {
//                                matchArr[j][i] = upLeftSide;
//                            }
//                        } else
//                        {
//                            matchArr[j][i] = 0;
//                        }
//                    }
//                }
//            }
//        }
//    }
