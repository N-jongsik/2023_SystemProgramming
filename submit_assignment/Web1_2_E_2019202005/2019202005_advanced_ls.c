//////////////////////////////////////////////////////////////////////
//File:simple_ls.c                                                //
//Date:2023/03/26                                               //
//OS:Ubuntu 16.04 LTS 64bits                                      //
//Author:Nam jongsik                                           //
//Student ID:2019202005                                           //
//-----------------------------------------------------               //
//Title:system programming assignment #1-1                          //
//Description:Implementing the Linux command ls and variable options//
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

int aflag=0, lflag=0;//variable to count aflag, lfalg 

void ls(DIR* dirp,struct dirent* entry, char **file,int argc,char* argv[]);//ls function declaration
void ls_a(DIR* dirp,struct dirent* entry, char **file,int argc,char* argv[]);//ls -a function declaration
void ls_l(DIR* dirp,struct dirent* entry,int argc, char *argv[],char **file);//ls -l function declaration
void ls_al(DIR* dirp,struct dirent* entry,int argc, char *argv[],char **file);//ls -al function declaration
void bubble_sort(char**files,int n);//bubble sort function declaration
char file_check(struct stat buf);//file check function declaration
void file_info(int k,struct stat buf[],char *file[],struct passwd *pw ,struct group *grp, struct tm*time);//file info about directory function declaration
void file_info1(struct stat buf,char *file,struct passwd *pw ,struct group *grp, struct tm*time);//file info about file function declaration

//////////////////////////////////////////////////////////
//Main                                                  //
//------------------------------------------------------//
//Input:int argc:store the number of input arguments    //
//    char*argv[]:Store the input parameters as an array//
//Output:0->Return a value of 0 to terminate the program//
//Purpose:Implemented by adding options such as         //
//        -a -l -al to the ls command                   //
//////////////////////////////////////////////////////////

int main(int argc, char* argv[]) {
    DIR* dirp;//pointer to directory stream
    struct dirent* entry;//Pointer to a dirent structure

    int i, n , j = 0;//initialize value
    char *file[1000];//file list
    int option=0;//option count value
    
    while((option=getopt(argc,argv,"al"))!=-1)//check option
    {
        switch(option)
        {
            case 'a'://when -a exsist
                    aflag++;//aflag+1
                    break;
            case 'l'://when -l exsist
                    lflag++;//lflag+1
                    break;
            case '?'://when not -a && not -l
                    break;
        }
    }

    if(aflag==0 && lflag==0)//ls
    {
        ls(dirp,entry,file,argc,argv);//call ls function
    }
    else if(aflag>=1 && lflag==0)//ls-a
    {
        ls_a(dirp,entry,file,argc,argv);//call ls -a function
    }
    else if(aflag==0 && lflag>=1)//ls-l
    {
        ls_l(dirp,entry,argc,argv,file);//call ls -l function
    }
    else if(aflag>=1 && lflag>=1)//ls-al or ls-al or ls- l -a
    {
        ls_al(dirp,entry,argc,argv,file);//call ls -al function
    }
    return 0;//quit main function   
}

///////////////////////////////////////////////////////////
//Bubble sort                                            //
//-------------------------------------------------------//
//Input:char**files:file in which the filename is stored //
//      int n:length of file                             //
//Purpose:Sorts the names of files and directories in    //
//        alphabetical order                             //
///////////////////////////////////////////////////////////

void bubble_sort(char**files,int n)//define bubble function
{
    int i,j;//initialize value
    for (i = 0; i < n - 1; i++)// Bubble sort the file names alphabetically
        {
            for (j = 0; j < n - i - 1; j++)
            {
                if (strcasecmp(files[j], files[j + 1]) > 0) //if files[j]>files[j+1]
                {
                    char* temp = files[j];
                    files[j] = files[j + 1];
                    files[j + 1] = temp;//Swap the previous and next elements of an array
                }//end of if
            }//end of for
        }//end of for
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
    struct stat buf[1000];//stat structure that stores information about a file or directory
    int i=0, n=0;//initialize value
    char cwd[1000];//for absolute path

    if(argc == 1)//When there is no file or directory as an argument
    {
        if((dirp=opendir("."))!=NULL)//open current directory
        {
            while ((entry = readdir(dirp)) != NULL)
            {
                if (entry->d_name[0] == '.')//do not print hidden files
                {
                    continue;
                }
                file[i] = entry->d_name;//add the name of the file to the array
                i++;
            }
            closedir(dirp);//close directory stream
            n = i; //number of files in the array
            bubble_sort(file,n);//call bubble_sort function
            for (i = 0; i < n; i++)
            {
                printf("%s\n", file[i]);//print the sorted directory or file
            }
        }
    }
    
    else//When there is a file or directory as an argument
    {
        for(int i=1;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check 
            if(check!='d')//not directory
            {
                if(check=='n')//no exsist file
                {
                    printf("cannot access %s: No such file or directory\n",argv[i]);//Exception when file does not exist
                }
            }
        }
        for(int i=1;i<argc;i++)
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
                    printf("%s\n",argv[i]);//print file not directory
                }
            }
        }
        
        for(i=1;i<argc;i++)
        {
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
                    bubble_sort(file,n);//call bubble_sort function
                    
                    char cwd_copy[1000];
                    getcwd(cwd_copy,sizeof(cwd_copy));//cwd_copy->current directory
                    strcat(cwd_copy,"/");//current directory+"/"
                    strcat(cwd_copy,argv[i]);//current directory+"/"+argv[i]
                    printf("Directory path: %s\n",cwd_copy);//print directory path
                    memset(cwd_copy,0,sizeof(cwd_copy));//reset cwd_copy
                    for (j = 0; j < n; j++)
                    {
                        printf("%s\n", file[j]);//print the sorted directory or file
                    }   
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
    struct stat buf[1000];//stat structure that stores information about a file or directory

    if(argc == 2)//When there is no file or directory as an argument
    {
        if((dirp=opendir("."))!=NULL)//open current directory
        {
            while ((entry = readdir(dirp)) != NULL)//reaad current directory
            {
                file[i] = entry->d_name;//add the name of the file to the array
                i++;
            }
            closedir(dirp);//close directory stream
            n = i; //number of files in the array
            bubble_sort(file,n);//call bubble_sort function
            for (i = 0; i < n; i++)
            {
                printf("%s\n", file[i]);//print the sorted directory or file
            }
        }
    }
    
    else if(argc>2)//When there is a file or directory as an argument
    {
        int options = 0;//initialize value
        for (int i = 1; i < argc; i++)/////space option
        {
            if (argv[i][0] == '-')
            {
                options++;//when '-' exsist options+1
            }
        }
        /////print no such file
        for(int i=1+options;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check
            if(check!='d')//not directory
            {
                if(check=='n')//no exsist file
                {
                    printf("cannot access %s: No such file or directory\n",argv[i]);//Exception when file does not exist
                }
            }
        }
        
        for(int i=1;i<argc;i++)
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
                    printf("%s\n",argv[i]);//print file not directory
                }
            }
        }

    for(i=1;i<argc;i++)
    {
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
                bubble_sort(file,n);//call bubble_sort function
                /////absolute path
                char cwd_copy[1000];
                getcwd(cwd_copy,sizeof(cwd_copy));//cwd_copy->current directory
                strcat(cwd_copy,"/");//current directory+"/"
                strcat(cwd_copy,argv[i]);//current directory+"/"+argv[i]
                printf("Directory path: %s\n",cwd_copy);//print directory path
                memset(cwd_copy,0,sizeof(cwd_copy));//reset cwd_copy
                for (j = 0; j < n; j++)
                {
                    printf("%s\n", file[j]);//print the sorted directory or file
                }   
            }
        }
    }
}
///////////////////////////////////////////////////////////
//ls_l                                                    //
//-------------------------------------------------------//
//Input:DIR*:directory pointer, struct dirent* :directory stream //
//          char**:file list int argc:store the number of input arguments        //
//          char*argv[]:Store the input parameters as an array
//Purpose:print directory & file names & file information in directory                   //
///////////////////////////////////////////////////////////
void ls_l(DIR* dirp,struct dirent* entry,int argc, char *argv[],char **file)//define ls -l function
{
    int i=0;//initialize value
    int n;//initialize value 
    int k = 0, total=0;//initialize value
    struct stat buf[1000];//stat structure that stores information about a file or directory

    struct passwd *pw;//get user info
    struct group *grp;//get group info
    struct tm* time;// get time info
    char cwd[1000];
    
    char permission[]="----------";
    char *Month[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
    char check;////value to check file
    if(argc==2)//When there is no file or directory as an argument
    {
        if((dirp=opendir("."))!=NULL)
        {
            while ((entry = readdir(dirp)) != NULL)
            {
                if (entry->d_name[0] == '.')//do not print hidden files
                {
                    continue;
                }
                else
                {
                    file[i] = entry->d_name;
                    i++;//i=how many files in directory
                }
            }
            bubble_sort(file,i);//call bubble_sort function
           
            for (int j = 0; j < i; j++)
            {
                lstat(file[j], &buf[j]);//save file information into buf
                total+=buf[j].st_blocks;//get block size
            }
            getcwd(cwd,sizeof(cwd));//cwd:current directory
            printf("Directory path: %s\n",cwd);//print directory path
            printf("total : %d\n",total/2);//print block size

            file_info(i,buf,file,pw ,grp,time);//print file info   
        }
        closedir(dirp);//close directory
    }              

    else if(argc>2)//When there is a file or directory as an argument
    {
        int options = 0;//initialize value
        for (int i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-')
            {
                options++;//when '-' exsist options+1
            }
        }
        
        for(int i=1+options;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check
            if(check!='d')//not directory 
            {
                if(check=='n')//no exsist file
                {
                    printf("cannot access %s: No such file or directory\n",argv[i]);//Exception when file does not exist
                }
            }
        }
        
        for(int i=2;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check
            if(check!='d' && check!='n')//not directory, exsist file
            {
                file_info1(buf[i],argv[i],pw ,grp,time);//print file info not directory
            }
        }
        
        for(i=1;i<argc;i++)
        {
            dirp=opendir(argv[i]);//open directory
            if (dirp!= NULL)
            {
                int j=0;//initialize value
                
                for(i=2;i<argc;i++)
                {   
                    dirp=opendir(argv[i]);//open directory
                    if(dirp!=NULL)
                    {
                        while ((entry = readdir(dirp)) != NULL)
                        {
                            if (entry->d_name[0] == '.')//do not print hidden files
                            {
                                continue;
                            }
                            else
                            {   
                                file[k] = entry->d_name;//store file name into file array
                                k++;
                            }
                        }
                        if(dirp!=NULL)
                        {
                            bubble_sort(file,k);//call bubble_sort function
                            char cwd_copy[1000];
                            strcpy(cwd_copy, argv[i]);
                            strcat(cwd_copy, "/");
                            for (int q = 0; q < k; q++)
                            {
                                strcpy(cwd_copy, argv[i]);//
                                strcat(cwd_copy, "/");
                                strcat(cwd_copy, file[q]);

                                lstat(cwd_copy, &buf[q]);//save file information into buf
                                total+=buf[q].st_blocks;//get block size
                                memset(cwd_copy, 0, sizeof(cwd_copy));
                            }
                            if(argv[i][0]=='/')
                            {
                                printf("Directory Path : %s\n", argv[i]);           
                            }
                        
                            else
                            {
                                char cwd_copy[1000];
                                getcwd(cwd_copy,sizeof(cwd_copy));
                                strcat(cwd_copy,"/");
                                strcat(cwd_copy,argv[i]);
                                printf("Directory Path : %s\n", cwd_copy);
                                memset(cwd_copy, 0, sizeof(cwd_copy));
                            }
                            // k = how many files in this directory    
                            printf("total : %d\n", total/2);//print the number of 1k block
                            total = 0;

                            file_info(k,buf,file,pw ,grp,time);//print file info
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
//ls_al                                                    //
//-------------------------------------------------------//
//Input:DIR*:directory pointer, struct dirent* :directory stream //
//          char**:file list int argc:store the number of input arguments        //
//          char*argv[]:Store the input parameters as an array
//Purpose:print directory & file names & file information & hidden files in directory                   //
///////////////////////////////////////////////////////////
void ls_al(DIR* dirp,struct dirent* entry,int argc, char *argv[],char **file)//define ls -al function
{
    int i=0;//initialize value
    int n; //initialize value
    int total = 0;//initialize value
    struct stat buf[1000];//stat structure that stores information about a file or directory
    int options = 0;//initialize value
    struct passwd *pw;//get user info
    struct group *grp;//get group info
    struct tm* time;//get time info
    int k = 0;//initialize value
    char cwd[1000];

    char permission[]="----------";
    char *Month[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

    for (int i = 1; i < argc; i++)
    {
        if (argv[i][0] == '-')
        {
            options++;//when '-'exsist, options+1
        }
    }

    if(argc==1+options)//When there is no file or directory as an argument
    {
        if((dirp=opendir("."))!=NULL)
        {
            while ((entry = readdir(dirp)) != NULL)
            {
                file[i] = entry->d_name;
                i++;//i=how many files in directory
                
            }
            bubble_sort(file,i);//call bubble_sort function
           
            for (int j = 0; j < i; j++)
            {
                lstat(file[j], &buf[j]);//save file information into buf
                total+=buf[j].st_blocks;//gegt block size
            }
            getcwd(cwd,sizeof(cwd));
            printf("Directory path: %s\n",cwd);
            printf("total : %d\n",total/2);

            file_info(i,buf,file,pw ,grp,time);//print file info   
        }
        closedir(dirp);//close directory
    }        
   else if(argc>2)//When there is a file or directory as an argument
    {
        int options = 0;//initialize value
        for (i = 1; i < argc; i++)
        {
            if (argv[i][0] == '-')
            {
                options++;//when '-' exsist, options+1
            }
        }
        
        for(i=1+options;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check
            if(check!='d')//not directory
            {
                if(check=='n')//no exsist file
                {
                    printf("cannot access %s: No such file or directory\n",argv[i]);//Exception when file does not exist
                }
            }
        }
       
        for(i=2;i<argc;i++)
        {
            lstat(argv[i], &buf[i]);//save file information into buf
            char check=file_check(buf[i]);//type check
            if(check!='d' && check!='n')//not directory, exsist file
            {
                file_info1(buf[i],argv[i],pw ,grp,time);//print file info not directory
            }
        }
        
        for(i=1;i<argc;i++)
        {
                dirp=opendir(argv[i]);//open directory
                if (dirp!= NULL)
                {
                    int j=0;//initialize value
                    for(i=2;i<argc;i++)
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
                                bubble_sort(file,k);//call bubble_sort function
                                char cwd_copy[1000];
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
                                    printf("Directory Path : %s\n", argv[i]);//print directory path           
                                }
                
                                else
                                {
                                    getcwd(cwd_copy,sizeof(cwd_copy));
                                    strcat(cwd_copy,"/");
                                    strcat(cwd_copy,argv[i]);
                                    printf("Directory Path : %s\n", cwd_copy);//print directory path
                                    memset(cwd_copy, 0, sizeof(cwd_copy));//reset cwd_copy
                                }

                                // k = how many files in this directory    
                                printf("total : %d\n", total/2);//print the number of lk blocks
                                total = 0;

                                file_info(k,buf,file,pw ,grp,time);//print file info
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
void file_info(int k,struct stat buf[],char *file[],struct passwd *pw ,struct group *grp, struct tm*time)//define file info function about directory
{
    char permission[]="----------";//initialize permission array
    char *Month[]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};//initialize month array
    int n;//initialize value
    for (n=0;n<k;n++)
            {  
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
                    
                printf("%s\t",permission);//print permission
            
                printf("%li\t", buf[n].st_nlink);//print count of link

                pw = getpwuid(buf[n].st_uid);//get File Owner's Information
                grp  = getgrgid(buf[n].st_gid);//get the information of the file's owning group
                printf("%s\t", pw->pw_name); //print file owner
                printf("%s\t", grp->gr_name);//print group that owns file
                printf("%li\t", buf[n].st_size);//print file size

                const time_t *modiTime = &buf[n].st_mtime;//to get the last modified time of that file
                time = localtime(modiTime);//function for Parsing Time Information
                printf("%s\t", Month[time->tm_mon]);//print month
                printf("%-d\t", time->tm_mday);//print day
                printf("%02d : %02d\t", time->tm_hour, time->tm_min);//print time
                    
                printf("%s\n",file[n]);//print file name
            }
}
/////////////////////////////////////////////////////////////////////////////////////////
//file_info1                                                                            //
//-------------------------------------------------------                              //
//Input: struct stat: get file information                  //
//          char* file[]:file list int argc:store the number of input arguments        //
//          struct passwd *:get user info struct group *:get group info                //
//          struct tm*:get time info                                                   //
//Purpose:print file's more information                                                //
/////////////////////////////////////////////////////////////////////////////////////////
void file_info1(struct stat buf,char *file,struct passwd *pw ,struct group *grp, struct tm*time)//define file1 info function about file
{
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
                    
                printf("%s\t",permission);//print permission
               
                printf("%li\t", buf.st_nlink);//print count of link

                pw = getpwuid(buf.st_uid);//get File Owner's Information
                grp  = getgrgid(buf.st_gid);//get the information of the file's owning group
                printf("%s\t", pw->pw_name);//print file owner
                printf("%s\t", grp->gr_name);//print group that owns file
                printf("%li\t", buf.st_size);//print file size

                const time_t *modiTime = &buf.st_mtime;//to get the last modified time of that file
                time = localtime(modiTime);//function for Parsing Time Information
                printf("%s\t", Month[time->tm_mon]);//print month
                printf("%-d\t", time->tm_mday);//print day
                printf("%02d : %02d\t", time->tm_hour, time->tm_min);//print time
                    
                printf("%s\n",file);//print file name           
}