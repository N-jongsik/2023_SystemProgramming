//////////////////////////////////////////////////////////
//File:simple_ls.c				        //
//Date:2023/03/26					//
//OS:Ubuntu 16.04 LTS 64bits		                //
//Author:Nam jongsik					//
//Student ID:2019202005					//
//-----------------------------------------------------	//
//Title:system programming assignment #1-1		//
//Description:Implementing the Linux command ls	        //
//////////////////////////////////////////////////////////

#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>

///////////////////////////////////////////////////////////
//Bubble sort                                            //
//-------------------------------------------------------//
//Input:char**files:file in which the filename is stored //
//      int n:length of file                             //
//Purpose:Sorts the names of files and directories in    //
//        alphabetical order				 //
///////////////////////////////////////////////////////////

void bubble_sort(char**files,int n){
    int i,j;
    for (i = 0; i < n - 1; i++)// Bubble sort the file names alphabetically
        {
            for (j = 0; j < n - i - 1; j++)
            {
                if (strcasecmp(files[j], files[j + 1]) > 0) //if files[j]>files[j+1]
                {
                    char* temp = files[j];
                    files[j] = files[j + 1];
                    files[j + 1] = temp;
                }//end of if
            }//end fo for
        }//end of for
}

//////////////////////////////////////////////////////////
//Main                                                  //
//------------------------------------------------------//
//Input:int argc:store the number of input arguments	//
//    char*argv[]:Store the input parameters as an array//
//Output:0->Return a value of 0 to terminate the program//
//Purpose:Use the bubble_sort function and              //
//        proceed with exception handling               //
//////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    DIR* dirp;//pointer to directory stream
    struct dirent* entry;//Pointer to a dirent structure
    char* file_name[1000];// Array to store file names
    int i, n , j = 0;

    char* dirp_path;//diretory path
    
    if (argc == 2)
    {
        dirp_path = argv[1];//path is file received as argument
    }//end of if
    else if(argc == 1)
    {
        dirp_path = ".";//path is current
    }//end of else if
    else
    {
	    printf("simple_ls: only one directory path can be processed\n");//When more than one file path is entered
	    return 0;
    }

    if ((dirp = opendir(dirp_path)) != NULL)// open directory stream
    {
        while ((entry = readdir(dirp)) != NULL)//read directory stream
        {
            file_name[i] = entry->d_name;//add the name of the file to the array
            i++;
        }//end of while
        closedir(dirp);//close directory stream
        n = i; //number of files in the array

        bubble_sort(file_name,n);//call bubble_sort function

        for (i = 0; i < n; i++)
        {
            printf("%s\n", file_name[i]);//print the sorted directory or file
        }//end of for
     
    }//end of if
    else
    {
        printf("simple_ls:cannot access %s :no such directory\n",argv[1]);//If a file is entered rather than a directory & If a directory that does not exist is entered
        return 0;
    }//end of else

    return 0;
}
