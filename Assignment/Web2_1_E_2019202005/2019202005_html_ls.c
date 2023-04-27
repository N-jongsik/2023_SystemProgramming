//////////////////////////////////////////////////////////////////////
//File:2019202005_html_ls.c                                         //
//Date:2023/04/19                                                   //
//OS:Ubuntu 16.04 LTS 64bits                                        //
//Author:Nam jongsik                                                //
//Student ID:2019202005                                             //
//-----------------------------------------------------             //
//Title:system programming assignment #2-1                          //
//Description:Implementing the Linux command ls with html	        //
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fnmatch.h>

FILE*file_html;//file pointer
int aflag=0, lflag=0 ,hflag=0, Sflag=0, rflag=0;//variable to count aflag, lfalg 

void ls(DIR* dirp,struct dirent* entry, char **file,int argc,char* argv[]);//ls function declaration
void ls_a(DIR* dirp,struct dirent* entry, char **file,int argc,char* argv[]);//ls -a function declaration
void ls_l(DIR* dirp,struct dirent* entry,int argc, char *argv[],char **file);//ls -l function declaration
void ls_al(DIR* dirp,struct dirent* entry,int argc, char *argv[],char **file);//ls -al function declaration
void bubble_sort(char**file,int n, struct stat buf[]);//bubble sort function declaration
char file_check(struct stat buf);//file check function declaration
void file_info(char cwd_copy[],int argc,int k,struct stat buf[],char *file[],struct passwd *pw ,struct group *grp, struct tm*time);//file info about directory function declaration
void file_info1(struct stat buf,char *file,struct passwd *pw ,struct group *grp, struct tm*time);//file info about file function declaration
void sort_files(char **names, off_t *sizes, int count, struct stat buf[]);//when use -S option
void sort_files_reverse(char **names, off_t *sizes, int count, struct stat buf[]);//when use -rS option
void bubble_sort_reverse(char**file,int n,struct stat buf[]);//when use -r option
void wildcard(int argc, char *argv[],DIR *dirp,struct dirent *entry,struct stat buf);//wildcard function when argument 1
void wildcard_many(int argc, char *argv[],DIR *dirp,struct dirent *entry,struct stat buf);//wildcard function many argument

///////////////////////////////////////////////////////////
//Bubble sort                                            //
//-------------------------------------------------------//
//Input:char**files:file in which the filename is stored //
//      int n:length of file                             //
//Purpose:Sorts the names of files and directories       //
///////////////////////////////////////////////////////////
//define bubble function
void bubble_sort1(char**file, int n) {
    int i,j; //initialize value
    // Bubble sort the file names alphabetically
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {//if files[j]>files[j+1]
            if (strcasecmp(file[j], file[j + 1]) > 0) {
                char* temp = file[j];
                file[j] = file[j + 1];
                file[j + 1] = temp; //Swap the previous and next elements of an array
            }//end of if
        }//end of for
    }//end of for
}

///////////////////////////////////////////////////////////////
//sort_files_reverse                                         //
//-------------------------------------------------------    //
//Input:char**files:file in which the filename is stored     //
//      int count:length of file ,struct stat buf[]:file size//
//      off_t *sizes                                         //
//Purpose:Sorts the sizes of files and directories in reverse//
///////////////////////////////////////////////////////////////
void sort_files_reverse(char **names, off_t *sizes, int count, struct stat buf[]) {
    int i, j;
    char *temp_name;
    struct stat temp_size;
    
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count-i-1; j++) {
            if (buf[j].st_size > buf[j+1].st_size) {//sort size in reverse
             
                temp_name = names[j];
                names[j] = names[j+1];
                names[j+1] = temp_name;//Swap the previous and next elements of an array
                
                temp_size = buf[j];
                buf[j] = buf[j+1];
                buf[j+1] = temp_size;//Swap the previous and next elements of an array
            }
            else if (buf[j].st_size == buf[j+1].st_size) {//when same size
                if (strcasecmp(names[j], names[j+1]) < 0) {//sort name in reverse
                    temp_name = names[j];
                    names[j] = names[j+1];
                    names[j+1] = temp_name;//Swap the previous and next elements of an array
                    
                    temp_size = buf[j];
                    buf[j] = buf[j+1];
                    buf[j+1] = temp_size;//Swap the previous and next elements of an array
                }
            }
        }
    }
}
///////////////////////////////////////////////////////////////
//sort_files                                                 //
//-------------------------------------------------------    //
//Input:char**files:file in which the filename is stored     //
//      int count:length of file ,struct stat buf[]:file size//
//      off_t *sizes                                         //
//Purpose:Sorts the sizes of files and directories           //
///////////////////////////////////////////////////////////////
void sort_files(char **names, off_t *sizes, int count, struct stat buf[]) 
{
    int i, j;
    char *temp_name;
    struct stat temp_size;
    
    for (i = 0; i < count - 1; i++) {
        for (j = 0; j < count-i-1; j++) {
            if (buf[j].st_size < buf[j+1].st_size) {//sort size in order
             
                temp_name = names[j];
                names[j] = names[j+1];
                names[j+1] = temp_name;//Swap the previous and next elements of an array
                
                temp_size = buf[j];
                buf[j] = buf[j+1];
                buf[j+1] = temp_size;//Swap the previous and next elements of an array
            }
            else if (buf[j].st_size == buf[j+1].st_size) {//when same size
                if (strcasecmp(names[j], names[j+1]) > 0) {//sort name in order
                    temp_name = names[j];
                    names[j] = names[j+1];
                    names[j+1] = temp_name;//Swap the previous and next elements of an array
                    
                    temp_size = buf[j];
                    buf[j] = buf[j+1];
                    buf[j+1] = temp_size;//Swap the previous and next elements of an array
                }
            }
        }
    }
}

///////////////////////////////////////////////////////////
//Bubble sort                                            //
//-------------------------------------------------------//
//Input:char**files:file in which the filename is stored //
//      int n:length of file ,struct stat buf[]:file size//
//Purpose:Sorts the names of files and directories       //
///////////////////////////////////////////////////////////

void bubble_sort(char**file, int n, struct stat buf[]) {//define bubble function
    int i,j; //initialize value
    struct stat temp_size; // Bubble sort the file names alphabetically
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) { //if files[j]>files[j+1]
            if (strcasecmp(file[j], file[j + 1]) > 0) {
                char* temp = file[j];
                file[j] = file[j + 1];
                file[j + 1] = temp; //Swap the previous and next elements of an array

                temp_size = buf[j];
                buf[j] = buf[j+1];
                buf[j+1] = temp_size;//Swap the previous and next elements of an array
            }//end of if
        }//end of for
    }//end of for
}

///////////////////////////////////////////////////////////////
//Bubble sort reverse                                        //
//-------------------------------------------------------    //
//Input:char**files:file in which the filename is stored     //
//      int n:length of file ,struct stat buf[]:file size    //
//Purpose:Sorts the names of files and directories in reverse//
///////////////////////////////////////////////////////////////

void bubble_sort_reverse(char**file, int n, struct stat buf[]) {//define bubble sort reverse function
    int i,j; //initialize value
    struct stat temp_size; // Bubble sort reverse
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {//if files[j]<files[j+1]
            if (strcasecmp(file[j], file[j + 1]) < 0) {
                char* temp = file[j];
                file[j] = file[j + 1];
                file[j + 1] = temp; //Swap the previous and next elements of an array

                temp_size = buf[j];
                buf[j] = buf[j+1];
                buf[j+1] = temp_size;//Swap the previous and next elements of an array
            }//end of if
        }//end of for
    }//end of for
}

/////////////////////////////////////////////////////////////////////////////////
//wildcard                                                                     //
//-------------------------------------------------------                      //
//Input:DIR*:directory pointer, struct dirent* :directory stream               //
//          int argc:store the number of input arguments                       //
//          char*argv[]:Store the input parameters as an array                 //
//          struct stat buf                                                    //
//Purpose:  When entering wildcard characters, execute the corresponding action//
/////////////////////////////////////////////////////////////////////////////////
void wildcard(int argc, char *argv[],DIR *dirp,struct dirent *entry,struct stat buf)//function about wildcard
{    
    char *pattern;//pattern
    int file_match;//file matched with pattern
    int directory_match;//directory matched with pattern

    char *file[4096];//to store pattern file
    char *directory[4096];//to store pattern directory
    
    int i=0;//initialize variable
    int file_count=0;//initialize variable
    int directory_count=0;//initialize variable

    char absolute_temp[4096];
    char absolute_path[4096];
    getcwd(absolute_temp,4096);//get current directory
    realpath(absolute_temp,absolute_path);//get absolute path
    char copy[4096];

    pattern = argv[1];//when current directory
    if (argv[1][0] != '/')//when not absolute path
    {
        if ((dirp = opendir(".")) != NULL)//open directory
        {
            while ((entry = readdir(dirp)) != NULL) //read directory
            {
                if (entry->d_name[0] != '.')//do not print hidden files
                {
                    lstat(entry->d_name, &buf);//get info 
                    if(file_check(buf)!='d')//when not directory
                    {
                        file_match = fnmatch(pattern, entry->d_name, 0);//check wildcard
                        if(file_match==0)//when wildcard
                        {
                            file[file_count]=entry->d_name;//store file matched with wildcard
                            file_count++;//count file    
                        }
                    }
                    else if(file_check(buf)=='d')//check directory
                    {
                        directory_match = fnmatch(pattern, entry->d_name, 0);//check wildcard
                        if(directory_match==0)//when wildcard
                        {
                            directory[directory_count]=entry->d_name;//store direcory matched with wildcard
                            directory_count++;//count directory
                        }
                    }
                }
            }
            bubble_sort1(file,file_count);//sort file
            bubble_sort1(directory,directory_count);//sort directory
            fprintf(file_html,"<br><table border=\"1\">");//make table
            fprintf(file_html,"<tr><th>File Name</th></tr>");//table row & header
            for(int i=0;i<file_count;i++)
            {
                if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html
                    continue;
                }
                strcpy(copy,absolute_path);
                strcat(copy,"/");
                strcat(copy,file[i]);
                fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td></tr>", copy, file[i]);//print the sorted directory or file
            }
            fprintf(file_html,"</table>");//close table

            for(int i=0;i<directory_count;i++)
            {
                int count = 0;//count = 0
                char* sort_name[4096];
                DIR *dirp_directory;
                struct dirent *entry_directory;
                dirp_directory=opendir(directory[i]);//open directory
                fprintf(file_html,"</br><B>%s : </B></br>", directory[i]);//print directory
                fprintf(file_html,"<br><table border=\"1\">");//make table
                fprintf(file_html,"<tr><th>File Name</th></tr>");//table row & header
                while((entry_directory = readdir(dirp_directory)) != NULL){//read directory
                    if (entry_directory->d_name[0] != '.'){//except hidden file
                        sort_name[count] = entry_directory->d_name;//store directory
                        count++;//count directory
                    }
                }
                for (int i=0; i<count; i++){
                    for (int j = 0; j<count-1-i; j++){
                        if (strcasecmp(sort_name[j], sort_name[j+1]) > 0){//sort directory
                            char* temp = sort_name[j];
                            sort_name[j] = sort_name[j+1];
                            sort_name[j+1] = temp;
                        }
                    }
                }
                for (int i=0; i<count; i++){
                    if(strcmp(sort_name[i], "html_ls.html") == 0){//do not print html_ls.html
                        continue;
                    }
                    strcpy(copy,absolute_path);
                    strcat(copy,"/");
                    strcat(copy,sort_name[i]);
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td></tr>",copy, sort_name[i]);//print the sorted directory or file
                }
                fprintf(file_html,"</table></br>");//close table
            }
            closedir(dirp);//close directory
        }
    }

    else if (argv[1][0] == '/')//when absolute path
    {
        DIR *dirp;//declare DIR *dirp
        struct dirent *entry;//declare struct dirent *entry
    
        int path_len=0;//initialize path length
        char slash_path[4096];//declare slash_path array
        for (int i = strlen(argv[1])-1; i > -1; i--)//to get wildcard location
        {
            if (argv[1][i] == '/')//when /
            {
                path_len = i;//get path length except wildcard
                break;
            }
        }

        char temp_slash[4096];//declare temp_slash array
        for(int i=0;i<path_len;i++)
        {
            
            slash_path[i]=argv[1][i];//slash_path = path
        }
       
        strcat(slash_path,"/");//get absolute path
    
        strcpy(temp_slash,slash_path);//temp_slash==slash_path
        dirp=opendir(temp_slash);//opden directory
        if(dirp != NULL)
        {
            while((entry=readdir(dirp))!=NULL)//read directory
            {
                strcpy(temp_slash,slash_path);//initialize
                
                strcat(temp_slash,entry->d_name);//absolute path + d_name
                lstat(temp_slash,&buf);//get info
                if(entry->d_name[0]!='.')//except hidden file
                {   
                    if(file_check(buf)!='d')//when not directory
                    {
                        file_match=fnmatch(pattern,temp_slash,0);//check wildcard
                        if(file_match==0)//when wildcard
                        {
                            file[file_count]=entry->d_name;//store file matched with wildcard
                            file_count++;//count file
                        }
                       
                    }
            
                    else if(file_check(buf)=='d')//when dierctory
                        {
                            if(!fnmatch(pattern, temp_slash, 0))
                            {
                                directory[directory_count]=entry->d_name;//store file matched directory wildcard
                                directory_count++;//count directory
                            }
                        }
                } 
                    memset(temp_slash,0,sizeof(temp_slash));//memory set
                }
            }
            
            bubble_sort1(file,file_count);//sort file
            bubble_sort1(directory,directory_count);//sort directory
        
            for(int i=0;i<file_count;i++)//get directory path
            {
                strcpy(temp_slash, slash_path);
                strcat(temp_slash, file[i]);
                fprintf(file_html,"<br>%s</br>",temp_slash);//print directory path
            }
            
            for(int i=0;i<directory_count;i++)
            {
                int count = 0;
                char* sort_name[4096];
                char temp_slash[4096]={};
                strcpy(temp_slash,slash_path);
                strcat(temp_slash,directory[i]);
                fprintf(file_html,"<br>%s</br> ",temp_slash);//print directory path
                DIR* aaa = opendir(temp_slash);
                struct dirent * bbb;
                
                while((bbb = readdir(aaa)) != NULL){//read directory
                    
                    if (bbb->d_name[0] != '.'){//do not print hiddne file 
                        sort_name[count] = malloc((strlen(bbb->d_name)+1) * sizeof(char));
                        strcpy(sort_name[count],bbb->d_name);
                        count++;
                    }
                }
               
                for (int i=0; i<count; i++){
                    for (int j = 0; j<count-1-i; j++){
                        if (strcasecmp(sort_name[j], sort_name[j+1]) > 0){//sort files
                            char* temp = sort_name[j];
                            sort_name[j] = sort_name[j+1];
                            sort_name[j+1] = temp;
                        }
                    }
                }
                for (int i=0; i<count; i++)
                {
                    if(strcmp(sort_name[i], "html_ls.html") == 0){//do not print html_ls.html
                        continue;
                    }
                    fprintf(file_html,"<br><table border=\"1\">");//make table
                    fprintf(file_html,"<tr><th>File Name</th></tr>");//print File Name
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td></tr>", absolute_path, sort_name[i]);//print the sorted directory or file
                    fprintf(file_html,"</table>");//close table
                }
            }
            closedir(dirp);//close directory
    }
}
//////////////////////////////////////////////////////////////////////////////////////////
//wildcard_many                                                                         //
//-------------------------------------------------------                               //
//Input:DIR*:directory pointer, struct dirent* :directory stream                        //
//          int argc:store the number of input arguments                                //
//          char*argv[]:Store the input parameters as an array                          //
//          struct stat buf                                                             //
//Purpose:  When entering lot's of wildcard characters, execute the corresponding action//
//////////////////////////////////////////////////////////////////////////////////////////
void wildcard_many(int argc, char *argv[],DIR *dirp,struct dirent *entry,struct stat buf)
{
    char *pattern;
    int file_match;
    int directory_match;
    char *file[4096];
    char *directory[4096];
    int i=0;
    int file_count=0;//count file
    int directory_count=0;//count directory

    int exist_wild[4096] = {0, };// check wildcard in all argument vector
    
    int exwild = 0;
    char ls_list[1024][1024];
    int ls_count = 0; // count ls_list

    char absolute_temp[4096];
    char absolute_path[4096];
    getcwd(absolute_temp,4096);//get current directory
    realpath(absolute_temp,absolute_path);//get absolute path
    char cwd_copy[4096];
    
    for(i=optind;i<argc;i++)
    {
        for (int j=0; j<strlen(argv[i]); j++)
        {
            if((argv[i][j]=='*') || (argv[i][j]=='[') || (argv[i][j]==']') || (argv[i][j]=='?'))
            {
                exist_wild[i]++; //where wild card exsist
            }  
        }
        if (exist_wild[i] > 0){
            exwild++; 
        } 
    }

    if (exwild > 0)
    {
        for(i=optind;i<argc;i++)
        {
            if (exist_wild[i]>0) // if argv[i] is wildcard or not
            {
                if (argv[i][0] == '/') //when absolute path
                {
                    DIR *dirp;
                    struct dirent *entry;
                    int path_len=0;
                    char slash_path[1024];
                    for (int j = strlen(argv[i])-1; j > -1; j--)
                    {
                        if (argv[i][j] == '/')
                        {
                            path_len = j;//to get length of path
                            break;
                        }
                    }
                    
                    char temp_slash[1024];
                    for(int i=0;i<path_len;i++)
                    {
                        slash_path[i]=argv[1][i];//get absolute path
                    }
                    
                    slash_path[path_len] = '\0';
                    strcat(slash_path,"/");
                    strcpy(temp_slash,slash_path);
                    dirp=opendir(temp_slash);//open directory
                    if(dirp != NULL)
                    {
                        while((entry=readdir(dirp))!=NULL)//read directory
                        {
                            strcpy(temp_slash,slash_path);
                            
                            strcat(temp_slash,entry->d_name);
                            lstat(temp_slash,&buf);
                            if(entry->d_name[0]!='.')//do not print hiddne file
                            {   
                                if(file_check(buf)!='d')//when file
                                {
                                    file_match=fnmatch(argv[i],temp_slash,0);//check wildcard
                                    if(file_match==0)//when wildcard
                                    {
                                        file[file_count]=entry->d_name;//store file
                                        file_count++;
                                        
                                    }
                                }
                                else if(file_check(buf)=='d')//when directory
                                {
                                        directory_match=fnmatch(argv[i],temp_slash,0);//check wild card
                                        if(directory_match==0)//when wildcard
                                        {
                                            directory[directory_count]=entry->d_name;//store directory
                                            directory_count++;
                                        }
                                }
                            } 
                            memset(temp_slash,0,sizeof(temp_slash));//memory set
                        }       
                    }            
                }
                else if (argv[i][0] != '/')//not absolute path
                {
                    if ((dirp = opendir(".")) != NULL) //open directory
                        {
                            while ((entry = readdir(dirp)) != NULL)//read directory
                            {
                                if (entry->d_name[0] != '.')//do not print hidden files
                                {
                                    lstat(entry->d_name, &buf);
                                    if(file_check(buf)!='d')//when file
                                    {
                                        file_match = fnmatch(argv[i], entry->d_name, 0);//check wildcard with file
                                        if(file_match==0)//when file
                                        {
                                            file[file_count]=entry->d_name;//store file
                                            file_count++;    
                                        }
                                    }
                                    else if(file_check(buf)=='d')//when directory
                                    {
                                        directory_match = fnmatch(argv[i], entry->d_name, 0);//check wildcard with directory
                                        if(directory_match==0)//when wildcard
                                        {
                                            directory[directory_count]=entry->d_name;//store directory
                                            directory_count++;
                                            
                                        }
                                    }
                                }
                            }                
                        }
                }   
            }
            else if (exist_wild[i] == 0)
            {
                if(file_check(buf)!= 'd')//when not directory
                {
                    file[file_count]=argv[i];//store file
                    file_count++;
                }
                else if(file_check(buf)=='d')//when directory
                {
                    directory[directory_count]=argv[i];//store directory
                    directory_count++;
                }
            }
        } 
        bubble_sort1(file, file_count);//sort about file
        bubble_sort1(directory, directory_count); // sort about directory

        fprintf(file_html,"<table border=\"1\">");
        fprintf(file_html,"<tr><th>File Name</th></tr>");
        
        for (int i=0; i<file_count; i++)
        {
            strcpy(cwd_copy,absolute_path);
            strcat(cwd_copy,"/");
            strcat(cwd_copy,file[i]);
            if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html file
                continue;
            }
            fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td></tr>",cwd_copy,file[i]);//print file name
        }
        fprintf(file_html,"</br></table>");//close table
        for (int i=0; i<directory_count; i++)
        {
            DIR* dir;
            struct dirent *ent;     
            char* arr[4096];
            int count_num = 0;
            dir = opendir(directory[i]);//open directory
            if (dir != NULL){
                while ((ent = readdir(dir)) != NULL)//read directory
                {
                    if (ent->d_name[0] != '.')//do not print hiddne file
                    {
                        arr[count_num] = ent->d_name;
                        count_num++;
                    }
                }
                bubble_sort1(arr, count_num);//sort 
                fprintf(file_html,"<p><B>%s:</B></p>", directory[i]);//print directory name
                fprintf(file_html,"<br><table border=\"1\">");//make table
                fprintf(file_html,"<tr><th>File Name</th></tr>");//table row & header
                for (int i=0; i<count_num; i++)
                {
                    strcpy(cwd_copy,absolute_path);
                    strcat(cwd_copy,"/");
                    strcat(cwd_copy,arr[i]);
                    if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html file
                        continue;
                    }
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td></tr>", cwd_copy, arr[i]);//print the sorted directory or file
                }
                fprintf(file_html,"</table>");//close table
            }
            closedir(dir);//close directory
        }
    }
}

////////////////////////////////////////////////////////////
//Main                                                    //
//------------------------------------------------------  //
//Input:int argc:store the number of input arguments      //
//    char*argv[]:Store the input parameters as an array  //
//Output:0->Return a value of 0 to terminate the program  //
//Purpose:Implemented by adding options and check wildcard//
////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    DIR* dirp;//pointer to directory stream
    struct dirent* entry;//Pointer to a dirent structure

    int i, n , j = 0;//initialize value
    char *file[4096];//file list
    int option=0;//option count value
    struct stat buf;//declare struct stat buf
    
    int flag=0;//initialize value
    file_html=fopen("html_ls.html","w");//file open
    if(file_html==0)//when null
    {
        fprintf(file_html,"error");//error
        return -1;
    }
   
    fprintf(file_html,"<body><h1>");//make body & header
    
    for(i=0;i<argc;i++)
    {
        fprintf(file_html,"%s ",argv[i]);//print command
    }
    fprintf(file_html,"</h1>");//close header
    char array[4096];
    getcwd(array,4096);
    fprintf(file_html,"<title>%s</title>",array);//print current directory

    while((option=getopt(argc,argv,"alhSr"))!=-1)//check option
    {
        switch(option)
        {
            case 'a'://when -a exsist
                    aflag++;//aflag+1
                    break;
            case 'l'://when -l exsist
                    lflag++;//lflag+1
                    break;
            case 'h'://when -h exsist
                    hflag++;
                    break;
            case 'S'://when -S exsist
                    Sflag++;
                    break;
            case 'r'://when -l exsist
                    rflag++;
                    break;
            case '?'://when not flag
                    break;
        }
    }

    for(i=optind;i<argc;i++)
    {
        if(argc==2)
        {
            for(j=0;j<strlen(argv[i]);j++)
            {
                if((argv[i][j]=='*') || (argv[i][j]=='[') || (argv[i][j]==']') || (argv[i][j]=='?'))//when wildcard exist
                {
                    flag=1;//flag=1
                    break;
                }
            }
            wildcard(argc,argv,dirp,entry,buf);//call wildcard function
        }
        else if(argc>2)
        {
            for(j=0;j<strlen(argv[i]);j++)
            {
                if((argv[i][j]=='*') || (argv[i][j]=='[') || (argv[i][j]==']') || (argv[i][j]=='?'))//when wildcard exist
                {
                    flag=1;//flag=1
                    break;
                }
            }
            wildcard_many(argc,argv,dirp,entry,buf);//call wildcard function
            break;
        }
    }

    if(aflag==0 && lflag==0 && (i == argc) && flag==0)//ls
    {   
        ls(dirp,entry,file,argc,argv);//call ls function
    }
    else if(aflag>=1 && lflag==0 && (i == argc))//ls-a
    {
        ls_a(dirp,entry,file,argc,argv);//call ls -a function
    }
    else if(aflag==0 && lflag>=1 && (i == argc))//ls-l
    {
        ls_l(dirp,entry,argc,argv,file);//call ls -l function   
    }
    else if(aflag>=1 && lflag>=1 && (i == argc))   
    {
        ls_al(dirp,entry,argc,argv,file);//call ls -al function
    }
    fprintf(file_html,"</body>");//close body
    fclose(file_html);//file close
    return 0;//quit main function   
}

//////////////////////////////////////////////////////////////////////////////
//ls                                                                        //
//-------------------------------------------------------                   //
//Input:DIR*:directory pointer, struct dirent* :directory stream            //
//          char**:file list int argc:store the number of input arguments   //
//          char*argv[]:Store the input parameters as an array              //
//Purpose:print directory & file names in directory                         //
//////////////////////////////////////////////////////////////////////////////

void ls(DIR* dirp,struct dirent* entry, char **file,int argc,char* argv[])//define ls function 
{
    struct stat buf[4096];//stat structure that stores information about a file or directory
    int i=0, n=0;//initialize value
    char cwd[4096];//for absolute path
    struct stat buf_file[4096];
    struct stat k;
    
    off_t sizes[4096];
    
    char absolute_temp[4096];
    char absolute_path[4096];
    getcwd(absolute_temp,4096);//get current directory
    realpath(absolute_temp,absolute_path);//get absolute path

    char copy[4096];
    char cwd_copy[4096];

    if(argc == optind)//When there is no file or directory as an argument
    {
        fprintf(file_html,"<table border=\"1\">");//make table
        fprintf(file_html,"<tr><th>File Name</th></tr>");//make table header
        if((dirp=opendir("."))!=NULL)//open current directory
        {
            while ((entry = readdir(dirp)) != NULL)
            {
                if (entry->d_name[0] == '.')//do not print hidden files
                {
                    continue;
                }
                file[i] = entry->d_name;//add the name of the file to the array
                lstat(file[i],&buf[i]);
                sizes[i] = buf[i].st_size;
                i++;
            }
            closedir(dirp);//close directory stream
            n = i; //number of files in the array

            if(rflag >= 1 && Sflag >= 1)//when -r -S option
            {
                sort_files_reverse(file, sizes, n, buf);//call sort_files_reverse funstion
                for (i = 0; i < n; i++)
                {
                    if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html file
                        continue;
                    }
                    strcpy(copy,absolute_path);
                    strcat(copy,"/");
                    strcat(copy,file[i]);
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, file[i]);//print the sorted directory or file
                }
            }
            else if(rflag >=1 && Sflag == 0)//when -r option
            {
                bubble_sort_reverse(file,n,buf);//call bubble_sort_reverse function
                for (i = 0; i < n; i++)
                {
                    if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html file
                        continue;
                    }
                    strcpy(copy,absolute_path);
                    strcat(copy,"/");
                    strcat(copy,file[i]);
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, file[i]);//print the sorted directory or file
                }
            }
            else if(rflag ==0 && Sflag >= 1)//when -S option
            {
                sort_files(file, sizes, n, buf);//call sort_files function
                for (i = 0; i < n; i++)
                {
                    if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html file
                        continue;
                    }
                    strcpy(copy,absolute_path);
                    strcat(copy,"/");
                    strcat(copy,file[i]);
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, file[i]);//print the sorted directory or file
                }
            }
            else if(rflag==0 && Sflag ==0)//when ls
            {
                bubble_sort(file,n,buf);//call bubble_sort function
                for (i = 0; i < n; i++)
                {
                    if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html file
                        continue;
                    }
                    strcpy(copy,absolute_path);
                    strcat(copy,"/");
                    strcat(copy,file[i]);
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, file[i]);//print the sorted directory or file
                }
            }
        }
        fprintf(file_html,"</table>");//close table
    }
    else if (argc!=optind)//When there is a file or directory as an argument
    {
        for(int i=optind;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check 
            if(check!='d')//not directory
            {
                if(check=='n')//no exsist file
                {
                    fprintf(file_html,"<p>cannot access %s: No such file or directory</P>",argv[i]);//Exception when file does not exist
                }
            }
        }
        int count_file = 0;//initialize count_file
        char* name_file[4096];//declare name_file

        for(int i=optind;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check 
            if(check!='d')//not directory
            {
                if(check=='n')//no exsist file
                {
                    continue;
                }
                else
                {
                    buf_file[count_file] = buf[i];
                    name_file[count_file] = argv[i];
                    count_file++;
                }
            }
        }
        
        if(Sflag >= 1 && rflag == 0)//when -S option
        {
            sort_files(name_file, sizes, count_file, buf_file);//call sort_files function
            for (int i=0; i<count_file; i++)
            {
                strcpy(copy,absolute_path);
                strcat(copy,"/");
                strcat(copy,name_file[i]);
                fprintf(file_html,"<table border=\"1\">");//make table
                fprintf(file_html,"<tr><th>File Name</th></tr>");//make table row and header
                fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, name_file[i]);//print the sorted directory or file
                fprintf(file_html,"</table>");//close table
            }
        }
        else if(Sflag >= 1 && rflag >=1 )//when -r -S option
        {
            sort_files_reverse(name_file, sizes, count_file, buf_file);//call sort_files_reverse function
            for (int i=0; i<count_file; i++){
                strcpy(copy,absolute_path);
                strcat(copy,"/");
                strcat(copy,name_file[i]);
                fprintf(file_html,"<table border=\"1\">");//make table
                fprintf(file_html,"<tr><th>File Name</th></tr>");//make table row and header
                fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, name_file[i]);//print the sorted directory or file
                fprintf(file_html,"</table>");//close table
            }   
        }
        else if(Sflag == 0 && rflag >=1 )//when -r option
        {
            bubble_sort_reverse(name_file,count_file,buf_file);//call bubble_sort_reverse function
            for (int i = 0; i < count_file; i++){
                strcpy(copy,absolute_path);
                strcat(copy,"/");
                strcat(copy,name_file[i]);
                fprintf(file_html,"<table border=\"1\">");//make table
                fprintf(file_html,"<tr><th>File Name</th></tr>");//make table row and header
                fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, name_file[i]);//print the sorted directory or file
                fprintf(file_html,"</table>");//close table
            }
        }
        else//when ls
        {
            bubble_sort(name_file,count_file,buf_file);//call bubble_sort function
            for (int i = 0; i < count_file; i++){
                strcpy(copy,absolute_path);
                strcat(copy,"/");
                strcat(copy,name_file[i]);
                fprintf(file_html,"<table border=\"1\">");//make table
                fprintf(file_html,"<tr><th>File Name</th></tr>");//make table row and header
                fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, name_file[i]);//print the sorted directory or file
                fprintf(file_html,"</table>");//close table
            }
        }

        struct stat stat_buf[4096];//declare stat_buf
        for(i=optind;i<argc;i++)
        {
            char argv_copy[4096];//declare argv_copy
            strcpy(argv_copy, argv[i]);//argv_copy=argv
            dirp=opendir(argv[i]);//open directory
            if (dirp!= NULL)
            {
                int j=0;//initialize value
                while ((entry = readdir(dirp)) != NULL)//read directory
                {
                    if (entry->d_name[0] == '.')//do not print hidden files
                    {
                        continue;
                    }
                    file[j] = entry->d_name;//add the name of the file to the array
                    j++;
                }
                
                closedir(dirp);//close directory stream
                n = j; //number of files in the array
                for (int i=0; i<n; i++){
                    char temp[4096];
                    //char cwd[1024];
                    strcat(temp, argv_copy);//temp=temp+argv_copy
                    strcat(temp, "/");//temp=temp+/
                    strcat(temp, file[i]);//tmep=temp+file
                    lstat(temp, &stat_buf[i]);//get info
                    memset(temp, 0, sizeof(temp));//memory set
                }
                struct stat stat_buf[4096];//declare stat_buf
                char cwd_copy[4096];//declare cwd_copy
                getcwd(cwd_copy, 4096);//cwd_copy->current directory
                strcat(cwd_copy,"/");//current directory+"/"
                strcat(cwd_copy,argv[i]);//current directory+"/"+argv[i]
                char* temp = malloc(4096*sizeof(char));//allocate moemory
                strcpy(temp, cwd_copy);//temp=temp+cwd_copy
                if (argv[i][0] != '/'){//when /
                    fprintf(file_html,"<br><B>Directory path: %s</B><br>",temp);//print directory path
                }
                else {
                    fprintf(file_html,"<br><B>Directory path : %s</B></br>", argv[i]);//print directory path
                } 
                if(Sflag >=1 && rflag == 0)//when -S option
                {
                    fprintf(file_html,"<table border=\"1\">");//make table
                    fprintf(file_html,"<tr><th>File Name</th></tr>");//make table row and header
                    sort_files(file, sizes, n, stat_buf);//call sort_files function
                    for (j = 0; j < n; j++)
                    {
                        strcpy(cwd_copy,temp);
                        strcat(cwd_copy,"/");
                        strcat(cwd_copy,file[j]);
                        if(strcmp(file[j], "html_ls.html") == 0){//do not print html_ls.html file
                            continue;
                        }
                        fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", cwd_copy, file[j]);//print the sorted directory or file
                    }
                    fprintf(file_html,"</table>");//close table
                }
                else if(Sflag >= 1 && rflag >= 1)//when -r -S option
                {
                    fprintf(file_html,"<table border=\"1\">");//make table
                    fprintf(file_html,"<tr><th>File Name</th></tr>");//make table row and header
                    sort_files_reverse(file, sizes, n,stat_buf);//call sort_files_reverse function
                    for (j = 0; j < n; j++)
                    {
                        strcpy(cwd_copy,temp);
                        strcat(cwd_copy,"/");
                        strcat(cwd_copy,file[j]);
                        if(strcmp(file[j], "html_ls.html") == 0){//do not print html_ls.html file
                            continue;
                        }
                        fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", cwd_copy, file[j]);//print the sorted directory or file
                    }
                    fprintf(file_html,"</table>");
                }
                else if(Sflag == 0 && rflag>=1)//when -r option
                {
                    fprintf(file_html,"<table border=\"1\">");//make table
                    fprintf(file_html,"<tr><th>File Name</th></tr>");//make table row and header
                    bubble_sort_reverse(file,n,stat_buf);//call bubble_sort_reverse function
                    for (j = 0; j < n; j++)
                    {
                        strcpy(cwd_copy,temp);
                        strcat(cwd_copy,"/");
                        strcat(cwd_copy,file[j]);
                        if(strcmp(file[j], "html_ls.html") == 0){//do not print html_ls.html file
                            continue;
                        }
                        fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", cwd_copy, file[j]);//print the sorted directory or file
                    }
                    fprintf(file_html,"</table>");
                }
                else if(Sflag == 0 && rflag == 0)//when ls
                {
                    fprintf(file_html,"<table border=\"1\">");//make table
                    fprintf(file_html,"<tr><th>File Name</th></tr>");//make table row and heade
                    bubble_sort(file,n,buf);//call bubble_sort function
                    for (j = 0; j < n; j++)
                    {
                        strcpy(cwd_copy,temp);
                        strcat(cwd_copy,"/");
                        strcat(cwd_copy,file[j]);
                        if(strcmp(file[j], "html_ls.html") == 0){//do not print html_ls.html file
                            continue;
                        }
                        fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", cwd_copy, file[j]);//print the sorted directory or file
                    }
                    fprintf(file_html,"</table>");//close table
                }   
                free(temp);//deallocate memory
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////
//ls_a                                                                   //
//-------------------------------------------------------                //
//Input:DIR*:directory pointer, struct dirent* :directory stream         //
//          char**:file list int argc:store the number of input arguments//
//          char*argv[]:Store the input parameters as an array           //
//Purpose:print directory & file names & hidden files in directory       //
///////////////////////////////////////////////////////////////////////////
void ls_a(DIR* dirp,struct dirent* entry, char **file,int argc,char* argv[])//define ls -a function
{
    int i=0, n=0;//initialize value
    struct stat buf[4096];//stat structure that stores information about a file or directory
    off_t sizes[4096];//declare sizes
    struct stat buf_file[4096];//declare buf_file

    char absolute_temp[4096];
    char absolute_path[4096];
    getcwd(absolute_temp,4096);
    realpath(absolute_temp,absolute_path);
    char copy[4096];

    if(argc == optind)//When there is no file or directory as an argument
    {
        fprintf(file_html,"<table border=\"1\">");//make table
        fprintf(file_html,"<tr><th>File Name</th></tr>");//make table row and heade
        if((dirp=opendir("."))!=NULL)//open current directory
        {
            while ((entry = readdir(dirp)) != NULL)//reaad current directory
            {
                file[i] = entry->d_name;//add the name of the file to the array
                lstat(file[i],&buf[i]);//get info
                sizes[i] = buf[i].st_size;//get sizes
                i++;
            }
            closedir(dirp);//close directory stream
            n = i; //number of files in the array
            
            if(rflag >= 1 && Sflag >= 1)//when -r -S option
            {
                sort_files_reverse(file, sizes, n, buf);//call sort_files)reverse function
                for (i = 0; i < n; i++)
                {
                    if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html
                        continue;
                    }
                    strcpy(copy,absolute_path);
                    strcat(copy,"/");
                    strcat(copy,file[i]);
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, file[i]);//print the sorted directory or file
                }
            }
            else if(rflag >=1 && Sflag == 0)//when -r option
            {
                bubble_sort_reverse(file,n,buf);//call bubble_sort_reverse function
                for (i = 0; i < n; i++)
                {
                    if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html
                        continue;
                    }
                    strcpy(copy,absolute_path);
                    strcat(copy,"/");
                    strcat(copy,file[i]);
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, file[i]);//print the sorted directory or file
                }
            }
            else if(rflag ==0 && Sflag >= 1)//when -S option
            {
                sort_files(file, sizes, n, buf);//call sort_files function
                for (i = 0; i < n; i++)
                {
                    if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html
                        continue;
                    }
                    strcpy(copy,absolute_path);
                    strcat(copy,"/");
                    strcat(copy,file[i]);
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, file[i]);//print the sorted directory or file
                }
            }
            else
            {//when ls -a
                bubble_sort(file,n,buf);//call bubble_sort function
                for (i = 0; i < n; i++)
                {
                    if(strcmp(file[i], "html_ls.html") == 0){//do not print html_ls.html
                        continue;
                    }
                    strcpy(copy,absolute_path);
                    strcat(copy,"/");
                    strcat(copy,file[i]);
                    fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", copy, file[i]);//print the sorted directory or file
                }
            }
        }
        fprintf(file_html,"</table>");//close table
    }
    
    else if(argc!=optind)//When there is a file or directory as an argument
    {
        for(int i=optind;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check
            if(check!='d')//not directory
            {
                if(check=='n')//no exsist file
                {
                    fprintf(file_html,"<p>cannot access %s: No such file or directory</p>",argv[i]);//Exception when file does not exist
                }
            }
        }
        int count_file = 0;//initialize count_file
        char* name_file[4096];//declare name_file
        
        for(int i=optind;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check
            if(check!='d')//not directory
            {
                if(check=='n')//no exsist file
                {
                    continue;
                }
                else
                {
                    buf_file[count_file] = buf[i];
                    name_file[count_file] = argv[i];
                    count_file++;
                }
            }
        }
        if(Sflag >= 1 && rflag == 0)//when -r -S option
        {
            sort_files(name_file, sizes, count_file, buf_file);//call sort_files function
            for (int i=0; i<count_file; i++)
            {
                fprintf(file_html,"<table border=\"1\">");//make table
                fprintf(file_html,"<tr><th>File Name</th></tr>");//table row and header
                fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", absolute_path, name_file[i]);//print the sorted directory or file
                fprintf(file_html,"</table>");//close table
            }
        }
        else if(Sflag >= 1 && rflag >=1 )//when -S option
        {
            sort_files_reverse(name_file, sizes, count_file, buf_file);//call sort_files_reverse function
            for (int i=0; i<count_file; i++){
                fprintf(file_html,"<table border=\"1\">");//make table
                fprintf(file_html,"<tr><th>File Name</th></tr>");//table row and header
                fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", absolute_path, name_file[i]);//print the sorted directory or file
                fprintf(file_html,"</table>");//close table
            }
        }
        else if(Sflag == 0 && rflag >=1 )//when -r option
        {
            bubble_sort_reverse(name_file,count_file,buf_file);//call bubble_sort_reverse function
            for (int i = 0; i < count_file; i++){
                fprintf(file_html,"<table border=\"1\">");//make table
                fprintf(file_html,"<tr><th>File Name</th></tr>");//table row and header
                fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", absolute_path, name_file[i]);//print the sorted directory or file
                fprintf(file_html,"</table>");
            }
        }
        else//when ls -a
        {
            bubble_sort(name_file,count_file,buf_file);//call bubble_sort function
            for (int i = 0; i < count_file; i++){
                fprintf(file_html,"<table border=\"1\">");//make table
                fprintf(file_html,"<tr><th>File Name</th></tr>");//table row and header
                fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", absolute_path, name_file[i]);//print the sorted directory or file
                fprintf(file_html,"</table>");//close table
            }
        } 

        for(i=optind;i<argc;i++)
        {
                char argv_copy[4096];
                strcpy(argv_copy, argv[i]);
                dirp=opendir(argv[i]);//open directory
                if (dirp!= NULL)
                {
                    int j=0;//initialize value
                    
                    while ((entry = readdir(dirp)) != NULL)//read directory
                    {
                        file[j] = entry->d_name;//add the name of the file to the array
                        j++;
                    }
                    closedir(dirp);//close directory stream
                    n = j; //number of files in the array
                    
                    struct stat stat_buf[4096];//declare stat_buf
                    char cwd_copy[4096];//declare ced_copy
                    ///absolute path
                    for (int i=0; i<n; i++){
                        char temp[4096];//declare temp
                        strcat(temp, argv_copy);//temp=temp+argv_copy
                        strcat(temp, "/");//temp=temp+/
                        strcat(temp, file[i]);//tmep=temp+file
                        lstat(temp, &stat_buf[i]);//get info
                        memset(temp, 0, sizeof(temp));//memory set
                    }
                    
                    getcwd(cwd_copy, 4096);//cwd_copy->current directory
                    strcat(cwd_copy,"/");//current directory+"/"
                    strcat(cwd_copy,argv[i]);//current directory+"/"+argv[i]
                    char* temp = malloc(4096*sizeof(char));
                    strcpy(temp, cwd_copy);
                    if (argv[i][0] != '/'){
                        fprintf(file_html,"<p><B>Directory path: %s</B></p>",temp);//print directory path
                    }
                    else {
                        fprintf(file_html,"<p><B>Directory path : %s</B></p>", argv[i]);////print directory path
                    }

                    if(Sflag >=1 && rflag == 0)//when -S option
                    {
                        fprintf(file_html,"<table border=\"1\">");//make table
                        fprintf(file_html,"<tr><th>File Name</th></tr>");//table row and header
                        sort_files(file, sizes, n, stat_buf);//call sort_files function
                        for (j = 0; j < n; j++)
                        {
                            strcpy(cwd_copy,temp);
                            strcat(cwd_copy,"/");
                            strcat(cwd_copy,file[j]);
                            if(strcmp(file[j], "html_ls.html") == 0){//do not print html_ls.html
                                continue;
                            }
                            fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", cwd_copy, file[j]);//print the sorted directory or file
                        }
                        fprintf(file_html,"</table>");
                    }
                    else if(Sflag >= 1 && rflag >= 1)//when -r -S option
                    {
                        fprintf(file_html,"<table border=\"1\">");//make table
                        fprintf(file_html,"<tr><th>File Name</th></tr>");//table row and header
                        sort_files_reverse(file, sizes, n,stat_buf);//call sort_files_reverse function
                        for (j = 0; j < n; j++)
                        {
                            strcpy(cwd_copy,temp);
                            strcat(cwd_copy,"/");
                            strcat(cwd_copy,file[j]);
                            if(strcmp(file[j], "html_ls.html") == 0){//do not print html_ls.html
                                continue;
                            }
                            fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", cwd_copy, file[j]);//print the sorted directory or file
                        }
                        fprintf(file_html,"</table>");
                    }
                    else if(Sflag == 0 && rflag>=1)//when -r option
                    {
                        
                        fprintf(file_html,"<table border=\"1\">");//make table
                        fprintf(file_html,"<tr><th>File Name</th></tr>");//table row and header
                        bubble_sort_reverse(file,n,stat_buf);//call bubble_sort_reverse function
                        for (j = 0; j < n; j++)
                        {
                            strcpy(cwd_copy,temp);
                            strcat(cwd_copy,"/");
                            strcat(cwd_copy,file[j]);
                            if(strcmp(file[j], "html_ls.html") == 0){//do not print html_ls.html
                                continue;
                            }
                            fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", cwd_copy, file[j]);//print the sorted directory or file
                        }
                        fprintf(file_html,"</table>");
                    }
                
                    else if(Sflag == 0 && rflag == 0)//when ls -a
                    {
                        fprintf(file_html,"<table border=\"1\">");//make table
                        fprintf(file_html,"<tr><th>File Name</th></tr>");//table row and header
                        bubble_sort(file,n,buf);//call bubble_sort function
                        for (j = 0; j < n; j++)
                        {
                            strcpy(cwd_copy,temp);
                            strcat(cwd_copy,"/");
                            strcat(cwd_copy,file[j]);
                            if(strcmp(file[j], "html_ls.html") == 0){//do not print html_ls.html
                                continue;
                            }
                            fprintf(file_html,"<tr><td><a href=\"file://%s\">%s</a></td>", cwd_copy, file[j]);//print the sorted directory or file
                        }
                        fprintf(file_html,"</table>");
                    } 
                    free(temp);//deallocate memory   
                }
            }
    }
}
///////////////////////////////////////////////////////////////////////////////
//ls_l                                                                       //
//-------------------------------------------------------                    //
//Input:DIR*:directory pointer, struct dirent* :directory stream             //
//          char**:file list int argc:store the number of input arguments    //
//          char*argv[]:Store the input parameters as an array               //
//Purpose:print directory & file names & file information in directory       //
///////////////////////////////////////////////////////////////////////////////
void ls_l(DIR* dirp,struct dirent* entry,int argc, char *argv[],char **file)//define ls -l function
{
    int i=0;//initialize value
    int n;//initialize value 
    int k = 0, total=0;//initialize value
    struct stat buf[4096];//stat structure that stores information about a file or directory
    struct stat buf_file[4096];//declare buf_file

    struct passwd *pw;//get user info
    struct group *grp;//get group info
    struct tm* time;// get time info
    char cwd[4096];//declare cwd
    char copy[4096];
    int file_size[4096];//to sort with block size
    char check;//value to check file
    char cwd_copy[4096];
    off_t sizes[4096];//declare sizes

    if(argc==optind)//When there is no file or directory as an argument
    {
        if((dirp=opendir("."))!=NULL)//open directory
        {
            while ((entry = readdir(dirp)) != NULL)//read directory
            {
                if (entry->d_name[0] == '.')//do not print hidden files
                {
                    continue;
                }
                else
                {
                    file[i] = entry->d_name;//store file names
                    lstat(file[i],&buf[i]);//get info
                    sizes[i] = buf[i].st_size;//get sizes
                    i++;
                }
            }
        
            for (int j = 0; j < i; j++)
            {
                total+=buf[j].st_blocks;//get block size
            }
            
            getcwd(cwd,sizeof(cwd));//cwd:current directory
            fprintf(file_html,"<br><B>Directory path: %s<B></br>",cwd);//print directory path
            fprintf(file_html,"<B>total : %d</B><br>",total/2);//print block size

            if(Sflag>=1 && rflag == 0)//when-S option
            {
                sort_files(file, sizes, i,buf);//call sort_files_function
                file_info(cwd_copy,argc,i,buf,file,pw ,grp,time);//print file info
            }
            else if (Sflag >=1 && rflag >= 1 )//when -r -S option
            {
                sort_files_reverse(file, sizes, i,buf);//call sort_files_reverse function
                file_info(cwd_copy,argc,i,buf,file,pw ,grp,time);//print file info
            }
            else if(Sflag == 0 && rflag >= 1)//when -r option
            {
                bubble_sort_reverse(file,i,buf);//call bubble_sort_reverse function
                file_info(cwd_copy,argc,i,buf,file,pw ,grp,time);//print file info
            }
            else if(Sflag==0 && rflag==0)//when ls -l
            {
                bubble_sort(file,i,buf);//call bubble_sort function
                file_info(cwd_copy,argc,i,buf,file,pw ,grp,time);//print file info
            }
        }
        closedir(dirp);//close directory
    }

    else if(argc!=optind)//When there is a file or directory as an argument
    {
        for(int i=optind;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            check=file_check(buf[i]);//type check
            if(check!='d')//not directory 
            {
                if(check=='n')//no exsist file
                {
                    fprintf(file_html,"<p>cannot access %s: No such file or directory</p>",argv[i]);//Exception when file does not exist
                }
            }
        }
        int count_file = 0;//initialize count_file
        char* name_file[4096];//declare name_file
        for(int i=optind;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            check=file_check(buf[i]);//type check
            if(check!='d' && check!='n')//not directory, exsist file
            {
                buf_file[count_file] = buf[i];
                name_file[count_file] = argv[i];
                count_file++;
            }
        }

        if (Sflag >= 1 && rflag == 0) { //when -S option
            sort_files(name_file, sizes, count_file, buf_file); //call sort_files function
            for (int i = 0; i < count_file; i++) {
                file_info1(buf_file[i], name_file[i], pw, grp, time); //print file info not directory
            }
        } else if (Sflag >= 1 && rflag >= 1) { //when -r -S option
            sort_files_reverse(name_file, sizes, count_file, buf_file); //call sort_files_reverse function
            for (int i = 0; i < count_file; i++) {
                file_info1(buf_file[i], name_file[i], pw, grp, time); //print file info not directory
            }
        } else if (Sflag == 0 && rflag >= 1) { //when -r option
            bubble_sort_reverse(name_file, count_file, buf_file); //call bubble_sort_reverse function
            for (int i = 0; i < count_file; i++) {
                file_info1(buf_file[i], name_file[i], pw, grp, time); //print file info not directory
            }
        } else { //when ls -l
            bubble_sort(name_file, count_file, buf_file); //call bubble_sort function
            for (int i = 0; i < count_file; i++) {
                file_info1(buf_file[i], name_file[i], pw, grp, time); //print file info not directory
            }
        }

        for(i=optind;i<argc;i++)
        {
            if((dirp=opendir(argv[i]))!=NULL);//open directory
            {
                for(i=optind;i<argc;i++)
                {   
                    dirp=opendir(argv[i]);//open directory
                    if(dirp!=NULL)
                    {
                        while ((entry = readdir(dirp)) != NULL)//read directory
                        {
                            if (entry->d_name[0] == '.')//do not print hidden files
                            {
                                continue;
                            }
                            else
                            {   
                                file[k] = entry->d_name;//store file name into file array
                                lstat(file[k],&buf[k]);
                                sizes[k] = buf[k].st_size;
                                k++;
                            }
                        }
                            
                            char cwd_copy[4096];
                            strcpy(cwd_copy, argv[i]);
                            strcat(cwd_copy, "/");
                            for (int q = 0; q < k; q++)
                            {
                                strcpy(cwd_copy, argv[i]);
                                strcat(cwd_copy, "/");
                                strcat(cwd_copy, file[q]);

                                lstat(cwd_copy, &buf[q]);//save file information into buf
                                total+=buf[q].st_blocks;//get block size
                                memset(cwd_copy, 0, sizeof(cwd_copy));
                            }

                            if(argv[i][0]=='/')
                            {     
                                fprintf(file_html,"<br><B>Directory Path : %s</B></br>", argv[i]);//print directory path        
                            }
                            
                            else
                            {
                                getcwd(cwd_copy,sizeof(cwd_copy));
                                strcat(cwd_copy,"/");
                                strcat(cwd_copy,argv[i]);
                                fprintf(file_html,"<br><B>Directory Path : %s<B></br>", cwd_copy);//print directory path
                            }
                            fprintf(file_html,"<B>total : %d</B>", total/2);//print the number of 1k block
                            total = 0;

                            if(Sflag >=1 && rflag == 0) //when -S option
                            {
                                sort_files(file, sizes, k,buf);
                                file_info(cwd_copy,argc,k,buf,file,pw ,grp,time);//print file info
                            }
                            else if(Sflag >= 1 && rflag >= 1) //when -r -S option
                            {
                                sort_files_reverse(file, sizes, k,buf);
                                file_info(cwd_copy,argc,k,buf,file,pw ,grp,time);//print file info
                            }
                            else if(Sflag == 0 && rflag>=1) //when -r option
                            {
                                bubble_sort_reverse(file,k,buf);//call bubble_sort function
                                file_info(cwd_copy,argc,k,buf,file,pw ,grp,time);//print file info
                            }
                            else if(Sflag==0 && rflag==0) //when ls -l
                            {
                                bubble_sort(file,k,buf);//call bubble_sort function
                                file_info(cwd_copy,argc,k,buf,file,pw ,grp,time);//print file info
                            }
                    }
                    k=0;//number of file initialize
                }
            }   
        }
        closedir(dirp);//close directory
}
}   


////////////////////////////////////////////////////////////////////////////////////////
//ls_al                                                                               //
//-------------------------------------------------------                             //
//Input:DIR*:directory pointer, struct dirent* :directory stream                      //
//          char**:file list int argc:store the number of input arguments             //
//          char*argv[]:Store the input parameters as an array                        //
//Purpose:print directory & file names & file information & hidden files in directory //
////////////////////////////////////////////////////////////////////////////////////////
void ls_al(DIR* dirp,struct dirent* entry,int argc, char *argv[],char **file)//define ls -alS function
{
    int i=0;//initialize value
    int n; //initialize value
    int total = 0;//initialize value
    struct stat buf[4096];//stat structure that stores information about a file or directory
    int options = 0;//initialize value
    struct passwd *pw;//get user info
    struct group *grp;//get group info
    struct tm* time;//get time info
    int k = 0;//initialize value
    char cwd[4096];
    char cwd_copy[4096];
    char check;
    int file_size[4096];
    off_t sizes[4096];

    struct stat buf_file[4096];

    if(argc==optind)//When there is no file or directory as an argument
    {
        if((dirp=opendir("."))!=NULL)
        {
            while ((entry = readdir(dirp)) != NULL)
            {
                file[i] = entry->d_name;
                lstat(file[i], &buf[i]);//save file information into buf
                sizes[i]=buf[i].st_size;
                i++;//i=how many files in directory
            }
            
            for (int j = 0; j < i; j++)
            {
                total+=buf[j].st_blocks;//gegt block size
            }
           
            getcwd(cwd,sizeof(cwd));//cwd:current directory
            fprintf(file_html,"<br><B>Directory path: %s<B></br>",cwd);//print directory path
            fprintf(file_html,"<B>total : %d</B><br>",total/2);//print block size

            if(Sflag>=1 && rflag == 0) //when -r option
            {
                sort_files(file, sizes, i,buf);//call sort_files function
                file_info(cwd_copy,argc,i,buf,file,pw ,grp,time);//print file info
            }
            else if (Sflag >=1 && rflag >=1 ) //when -r -S option
            {
                sort_files_reverse(file, sizes, i,buf);//call sort_files_reverse function
                file_info(cwd_copy,argc,i,buf,file,pw ,grp,time);//print file info
            }
            else if (Sflag ==0 && rflag >=1 ) //when -S option
            {
                bubble_sort_reverse(file,i,buf);//call bubble_sort function
                file_info(cwd_copy,argc,i,buf,file,pw ,grp,time);//print file info
            }
            else{ //when ls -al
                bubble_sort(file,i,buf);//call bubble_sort function
                file_info(cwd_copy,argc,i,buf,file,pw ,grp,time);//print file info
            }
        }
        closedir(dirp);//close directory
    }        
    else if(argc!=optind)//When there is a file or directory as an argument
    {
        for(i=optind;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check
            if(check!='d')//not directory
            {
                if(check=='n')//no exsist file
                {
                    fprintf(file_html,"<p>cannot access %s: No such file or directory</p>",argv[i]);//Exception when file does not exist
                }
            }
        }
        int count_file = 0;
        char* name_file[4096];
        for(int i=optind;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            check=file_check(buf[i]);//type check
            if(check!='d' && check!='n')//not directory, exsist file
            {
                buf_file[count_file] = buf[i];
                name_file[count_file] = argv[i];
                count_file++;
            }
        }
        if(Sflag >= 1 && rflag == 0)//when -S option
        {
            sort_files(name_file, sizes, count_file, buf_file);//call sort_files function
            for (int i=0; i<count_file; i++){
                file_info1(buf_file[i], name_file[i],pw ,grp,time);//print file info not directory
            }
        }

        else if(Sflag >= 1 && rflag >= 1)//when -r -S option
        {
            sort_files_reverse(name_file, sizes, count_file, buf_file);//call sort_files_reverse function
            for (int i=0; i<count_file; i++){
                file_info1(buf_file[i], name_file[i],pw ,grp,time);//print file info not directory
            }
        }
        else if(Sflag == 0 && rflag >= 1)//when -r option
        {
            bubble_sort_reverse(name_file,count_file,buf_file);//call bubble_sort_reverse function
            for (int i = 0; i < count_file; i++){
                file_info1(buf_file[i],name_file[i],pw ,grp,time);//print file info not directory
            }
        }

        else//when ls -al
        {
            bubble_sort(name_file,count_file,buf_file);//call bubble_sort function
            for (int i = 0; i < count_file; i++){
                file_info1(buf_file[i],name_file[i],pw ,grp,time);//print file info not directory
            }
        }

        for(i=optind;i<argc;i++)
        {
                dirp=opendir(argv[i]);//open directory
                if (dirp!= NULL)
                {
                    int j=0;//initialize value
                    for(i=optind;i<argc;i++)
                    {   
                        dirp=opendir(argv[i]);//open directory
                        if(dirp!=NULL)
                        {
                            while ((entry = readdir(dirp)) != NULL)//read directory
                            {
                                file[k] = entry->d_name;//store file name into file array
                                k++;
                            }
                            
                            if(dirp!=NULL)
                            {
                                strcpy(cwd_copy, argv[i]);
                                strcat(cwd_copy, "/");
                                for (int q = 0; q < k; q++)
                                {
                                    strcpy(cwd_copy, argv[i]);
                                    strcat(cwd_copy, "/");
                                    strcat(cwd_copy, file[q]);

                                    lstat(cwd_copy, &buf[q]);//save file information into buf
                                    total+=buf[q].st_blocks;//get block size
                                    memset(cwd_copy, 0, sizeof(cwd_copy));//reset cwd_copy
                                }
                                if(argv[i][0]=='/')
                                {      
                                    fprintf(file_html,"<br><B>Directory Path : %s</B></br>", argv[i]);//print directory path        
                                }
                
                                else
                                {
                                    getcwd(cwd_copy,sizeof(cwd_copy));
                                    strcat(cwd_copy,"/");
                                    strcat(cwd_copy,argv[i]);
                                    fprintf(file_html,"<B><br>Directory Path : %s</B></br>", cwd_copy);//print directory path
                                }
                                fprintf(file_html,"<B>total : %d</B></br>", total/2);//print the number of lk blocks
                                total = 0;

                                if(Sflag >= 1 && rflag == 0)//when -S option
                                {
                                    sort_files(file, sizes, k,buf);//call sort_files function
                                    file_info(cwd_copy,argc,k,buf,file,pw ,grp,time);//print file info
                                }
                                
                                else if( Sflag >= 1 && rflag >= 1)//when -r -S option
                                {
                                    sort_files_reverse(file, sizes, k,buf);//call sort_files_reverse function
                                    file_info(cwd_copy,argc,k,buf,file,pw ,grp,time);//print file info
                                }
                                else if( Sflag == 0 && rflag >= 1)//when -r option
                                {
                                    bubble_sort_reverse(file,k,buf);//call bubble_sort_reverse function
                                    file_info(cwd_copy,argc,k,buf,file,pw ,grp,time);//print file info
                                }
                                else{//when ls -al
                                    bubble_sort(file,k,buf);//call bubble_sort function
                                    file_info(cwd_copy,argc,k,buf,file,pw ,grp,time);//print file info
                                }
                            }
                        }
                        k=0;//number of file initialize
                    }
                }
                closedir(dirp);//close directory
        }   
    }    
}
///////////////////////////////////////////////////////////
//file_check                                             //
//-------------------------------------------------------//
//Input:struct stat buf: get file information            //
//Output: return file type ex)d:directory -:regular file //        
//Purpose:check file type ex)d:directory -:regular file  //
///////////////////////////////////////////////////////////
char file_check(struct stat buf)//define file check
{
   char check;//variabl to check 
   if (S_ISDIR(buf.st_mode)){
      check = 'd';//directory-> d
   }
   else if (S_ISLNK(buf.st_mode)){
      check = 'l';//symbolic link-> l
   }
   else if (S_ISCHR(buf.st_mode)){
      check = 'c';//character special-> c
   }
   else if (S_ISBLK(buf.st_mode)){
      check = 'b';//block special-> b
   }
   else if (S_ISSOCK(buf.st_mode)){
      check = 's';//socket-> s
   }
   else if (S_ISFIFO(buf.st_mode)){
      check = 'P';//FIFO -> p
   }
   else if (S_ISREG(buf.st_mode)){
      check = '-';//regular file-> -
   }
   else {
      check = 'n';//is not file -> n
   }
   return check;
}
/////////////////////////////////////////////////////////////////////////////////////////
//file_info                                                                            //
//-------------------------------------------------------                              //
//Input:int k: the number of files, struct stat: get file information                  //
//          char* file[]:file list int argc:store the number of input arguments        //
//          struct passwd *:get user info struct group *:get group info                //
//          struct tm*:get time info                                                   //
//Purpose:print directory's more information                                           //
/////////////////////////////////////////////////////////////////////////////////////////
void file_info(char cwd_copy[],int argc,int k,struct stat buf[],char *file[],struct passwd *pw ,struct group *grp, struct tm*time)//define file info function about directory
{
    char permission[]="----------";//initialize permission array
    char *Month[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};//initialize month array
    int n;//initialize value

    char absolute_temp[4096];
    char absolute_path[4096];
    char copy[4096];
    getcwd(absolute_temp,4096);//get current directory
    realpath(absolute_temp,absolute_path);//get absolute path
    
    strcpy(copy,absolute_path);

    fprintf(file_html,"<table border=\"1\">");//make table
    fprintf(file_html,"<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>");//make header
    for (n=0;n<k;n++)
            {  
                strcpy(copy,absolute_path); 
                if (S_ISREG(buf[n].st_mode)) {
                    permission[0] = '-';}//regular file-> -
                else if (S_ISDIR(buf[n].st_mode)) {
                    permission[0] = 'd';//directory-> d
                }
                else if (S_ISCHR(buf[n].st_mode)) {
                    permission[0] = 'c';//character special-> c
                }
                else if (S_ISBLK(buf[n].st_mode)) 
                    permission[0] = 'b';//block special-> b
                else if (S_ISFIFO(buf[n].st_mode)) 
                    permission[0] = 'p';//FIFO -> p
                else if (S_ISLNK(buf[n].st_mode)) 
                    permission[0] = 'l';
                else if (S_ISSOCK(buf[n].st_mode)) 
                    permission[0] = 's';//socket-> s
                
                if (buf[n].st_mode & S_IRUSR){
                        permission[1] = 'r';//owner has read permission
                    }
                else {
                    permission[1] = '-';//owner has no read permission
                }
                if (buf[n].st_mode & S_IWUSR){
                    permission[2] = 'w';//owner has write permission
                }
                else {
                    permission[2] = '-';//owner has no write permission
                }
                if (buf[n].st_mode & S_IXUSR){
                    permission[3] = 'x';//owner has execute permission
                }
                else {
                    permission[3] = '-';//owner has no execute permission
                }
                if (buf[n].st_mode & S_IRGRP){
                        permission[4] = 'r';//group has read permission
                }
                else {
                    permission[4] = '-';//group has no read permission
                }
                if (buf[n].st_mode & S_IWGRP){
                        permission[5] = 'w';//group has write permission
                }
                else {
                    permission[5] = '-';//group has no write permission
                }   
                if (buf[n].st_mode & S_IXGRP){
                    permission[6] = 'x';//group has execute permission
                }
                else {
                    permission[6] = '-';//group has no execute permission
                }
                if (buf[n].st_mode & S_IROTH){
                        permission[7] = 'r';//others have read permission
                }
                else {
                    permission[7] = '-';//others have no read permission
                    }
                if (buf[n].st_mode & S_IWOTH){
                    permission[8] = 'w';//others have write permission
                }
                else {
                    permission[8] = '-';//others have no write permission
                }
                if (buf[n].st_mode & S_IXOTH){
                    permission[9] = 'x';//others have execute permission
                }
                else {
                    permission[9] = '-';//others have no execute permission
                }

                pw = getpwuid(buf[n].st_uid);//get File Owner's Information
                grp  = getgrgid(buf[n].st_gid);//get the information of the file's owning group
                const time_t *modiTime = &buf[n].st_mtime;//to get the last modified time of that file
                time = localtime(modiTime);//function for Parsing Time Information
                
                if(strcmp(file[n], "html_ls.html") == 0){//do not print html_ls.html file
                        continue;
                }
                if(argc==optind)
                {
                    strcat(copy,"/");
                    strcat(copy,file[n]);
                }
                else
                {
                    strcpy(copy,cwd_copy);
                    strcat(copy,"/");
                    strcat(copy,file[n]);                   
                }
                
                
                if(hflag>=1)//-h
                {
                    if (buf[n].st_size >= 1000LL * 1000LL * 1000LL * 10LL)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }
                    }
                    else if (buf[n].st_size < 1000LL * 1000LL * 1000LL * 10LL && buf[n].st_size >= 1000*1000*1000)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, (double)buf[n].st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, (double)buf[n].st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, (double)buf[n].st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }                       
                    }
                    else if (buf[n].st_size < 1000*1000*1000 && buf[n].st_size >= 1000*1000*10)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }  
                    }
                    else if(buf[n].st_size < 1000*1000*10 && buf[n].st_size >= 1000*1000)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, (double)buf[n].st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, (double)buf[n].st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, (double)buf[n].st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }  
                    }
                    else if(buf[n].st_size < 1000*1000 && buf[n].st_size >= 1000*10)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }     
                    }
                    else if(buf[n].st_size < 1000*10 && buf[n].st_size >= 1000)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, (double)buf[n].st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, (double)buf[n].st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, (double)buf[n].st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }  
                    }
                    else
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }  
                    }
                }
                else
                {
                    if(permission[0]=='d')
                    {
                        fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                    }
                    else if(permission[0]=='l')
                    {
                        fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                    }
                    else{
                        fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                    }
                }
            }
            fprintf(file_html,"</table>");//close table
}
/////////////////////////////////////////////////////////////////////////////////////////
//file_info1                                                                           //
//-------------------------------------------------------                              //
//Input: struct stat: get file information                                             //
//          char* file[]:file list int argc:store the number of input arguments        //
//          struct passwd *:get user info struct group *:get group info                //
//          struct tm*:get time info                                                   //
//Purpose:print file's more information                                                //
/////////////////////////////////////////////////////////////////////////////////////////
void file_info1(struct stat buf,char *file,struct passwd *pw ,struct group *grp, struct tm*time)//define file1 info function about file
{
    char absolute_temp[4096];
    char absolute_path[4096];
    getcwd(absolute_temp,4096);//get current directory
    realpath(absolute_temp,absolute_path);//get absolute path

    char copy[4096];    
    strcpy(copy,absolute_path);
    strcpy(copy,"/");
    strcpy(copy,file);

    fprintf(file_html,"<table border=\"1\">");//make table
    fprintf(file_html,"<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>");//make header
    char permission[]="----------";//initialize permission array
    char *Month[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};//initialize month array
                if (S_ISREG(buf.st_mode)) {
                    permission[0] = '-';}//regular file-> -
                else if (S_ISDIR(buf.st_mode)) {
                    permission[0] = 'd';//directory-> d
                }
                else if (S_ISCHR(buf.st_mode)) {
                    permission[0] = 'c';//character special-> c
                }
                else if (S_ISBLK(buf.st_mode)) 
                    permission[0] = 'b';//block special-> b
                else if (S_ISFIFO(buf.st_mode)) 
                    permission[0] = 'p';//FIFO -> p
                else if (S_ISLNK(buf.st_mode)) 
                    permission[0] = 'l';//symbolic link-> l
                else if (S_ISSOCK(buf.st_mode)) 
                    permission[0] = 's';//socket-> s
                
                if (buf.st_mode & S_IRUSR){
                    permission[1] = 'r';//owner has read permission
                }
                else {
                    permission[1] = '-';//owner has no read permission
                }
                if (buf.st_mode & S_IWUSR){
                    permission[2] = 'w';//owner has write permission
                }
                else {
                    permission[2] = '-';//owner has no write permission
                }
                if (buf.st_mode & S_IXUSR){
                    permission[3] = 'x';//owner has execute permission
                }
                else {
                    permission[3] = '-';//owner has no execute permission
                }
                if (buf.st_mode & S_IRGRP){
                    permission[4] = 'r';//group has read permission
                }
                else {
                    permission[4] = '-';//group has no read permission
                }
                if (buf.st_mode & S_IWGRP){
                    permission[5] = 'w';//group has write permission
                }
                else {
                    permission[5] = '-';//group has no write permission
                }
                if (buf.st_mode & S_IXGRP){
                    permission[6] = 'x';//group has execute permission
                }
                else {
                    permission[6] = '-';//group has no execute permission
                }
                if (buf.st_mode & S_IROTH){
                    permission[7] = 'r';//others have read permission
                }
                else {
                    permission[7] = '-';//others have no read permission
                }
                if (buf.st_mode & S_IWOTH){
                    permission[8] = 'w';//others have write permission
                }
                else {
                    permission[8] = '-';//others have no write permission
                }
                if (buf.st_mode & S_IXOTH){
                    permission[9] = 'x';//others have execute permission
                }
                else {
                    permission[9] = '-';//others have no execute permission
                }
                pw = getpwuid(buf.st_uid);//get File Owner's Information
                grp  = getgrgid(buf.st_gid);//get the information of the file's owning group
                const time_t *modiTime = &buf.st_mtime;//to get the last modified time of that file
                time = localtime(modiTime);//function for Parsing Time Information             
                if(hflag>=1)//-h
                {
                    if (buf.st_size >= 1000LL * 1000LL * 1000LL * 10LL)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }
                    }
                    else if (buf.st_size < 1000LL * 1000LL * 1000LL * 10LL && buf.st_size >= 1000*1000*1000)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, (double)buf.st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, (double)buf.st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fG</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, (double)buf.st_size/1000000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        } 
                    }
                    else if (buf.st_size < 1000*1000*1000 && buf.st_size >= 1000*1000*10)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }   
                    }
                    else if(buf.st_size < 1000*1000*10 && buf.st_size >= 1000*1000)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, (double)buf.st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, (double)buf.st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fM</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, (double)buf.st_size/1000000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }   
                    }
                    else if(buf.st_size < 1000*1000 && buf.st_size >= 1000*10)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%liK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }  
                    }
                    else if(buf.st_size < 1000*10 && buf.st_size >= 1000)
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, (double)buf.st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, (double)buf.st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%.1fK</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, (double)buf.st_size/1000, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        }  
                    }
                    else
                    {
                        if(permission[0]=='d')
                        {
                            fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                        }
                        else if(permission[0]=='l')
                        {
                            fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                        }
                        else{
                            fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                        } 
                    }
                }
                else
                {
                    if(permission[0]=='d')
                    {
                        fprintf(file_html,"<tr style=\"color:blue\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//directory blue color
                    }
                    else if(permission[0]=='l')
                    {
                        fprintf(file_html,"<tr style=\"color:green\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//link green color
                    }
                    else{
                        fprintf(file_html,"<tr style=\"color:red\"><td><a href=\"file://%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>",copy, file, permission, buf.st_nlink, pw->pw_name, grp->gr_name, buf.st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min);//file red color
                    }
                }
                fprintf(file_html,"</table>");//close table
}