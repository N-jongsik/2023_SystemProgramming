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

void bubble_sort(char**file,int n)//define bubble function
{
    int i,j;//initialize value
    for (i = 0; i < n - 1; i++)// Bubble sort the file names alphabetically
        {
            for (j = 0; j < n - i - 1; j++)
            {
                if (strcasecmp(file[j], file[j + 1]) > 0) //if files[j]>files[j+1]
                {
                    char* temp = file[j];
                    file[j] = file[j + 1];
                    file[j + 1] = temp;//Swap the previous and next elements of an array
                }//end of if
            }//end of for
        }//end of for
}
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
int main(int argc, char *argv[]) {
    DIR *dirp;
    struct dirent *entry;
    
    char *pattern;
    
    int file_match;
    int directory_match;

    char *file[1000];
    char *directory[1000];
    
    int i=0;
    struct stat buf;

    int file_count=0;
    int directory_count=0;

    // check wildcard in all argument vector
    int exist_wild[1024] = {0, };
    int exwild = 0;
    char ls_list[1024][1024]; // ls 할 파일들 묶어놓는 용도
    int ls_count = 0; // count ls_list
    for(i=optind;i<argc;i++)
    {
        for (int j=0; j<strlen(argv[i]); j++)
        {
            if((argv[i][j]=='*') || (argv[i][j]=='[') || (argv[i][j]==']') || (argv[i][j]=='?'))
            {
                exist_wild[i]++; // i번째 인자가 wildcard임을 알려줌
            }  
        }
        if (exist_wild[i] > 0){
            exwild++; // 인자 전체중에 wildcard가 하나라도 있으면 0보다 큼
        } 
    }

    if (exwild > 0){
        for(i=optind;i<argc;i++)
        {
            if (exist_wild[i]>0) // if argv[i] is wildcard or not
            {
                if (argv[i][0] == '/') //절대경로인지 상대경로인지 확인
                {
                    DIR *dirp;
                    struct dirent *entry;
                
                    int path_len=0;
                    char slash_path[1024];
                    for (int j = strlen(argv[i])-1; j > -1; j--)
                    {
                        if (argv[i][j] == '/')
                        {
                            path_len = j;
                            break;
                        }
                    }
                    char temp_slash[1024];
                    for(int i=0;i<path_len;i++)
                    {
                        slash_path[i]=argv[1][i];
                    }
                    slash_path[path_len] = '\0';
                
                    strcat(slash_path,"/");
                    
                    strcpy(temp_slash,slash_path);
                    dirp=opendir(temp_slash);
                    if(dirp != NULL)
                    {
                        while((entry=readdir(dirp))!=NULL)
                        {
                            
                            //printf("%s\n",slash_path);
                            strcpy(temp_slash,slash_path);
                            
                            strcat(temp_slash,entry->d_name);
                            lstat(temp_slash,&buf);
                            if(entry->d_name[0]!='.')
                            {   
                                if(file_check(buf)!='d')
                                {
                                    file_match=fnmatch(pattern,temp_slash,0);
                                    if(file_match==0)
                                    {
                                        file[file_count]=entry->d_name;
                                        file_count++;
                                    }
                                }
                        
                                else if(file_check(buf)=='d')
                                {
                                        directory_match=fnmatch(pattern,temp_slash,0);
                                        if(directory_match==0)
                                        {
                                            directory[directory_count]=entry->d_name;
                                            directory_count++;
                                        }
                                }
                            } 
                            memset(temp_slash,0,sizeof(temp_slash));
                        }
                            
                    }            
                }
                else if (argv[i][0] != '/')
                {
                    if ((dirp = opendir(".")) != NULL) 
                        {
                            while ((entry = readdir(dirp)) != NULL) 
                            {
                                if (entry->d_name[0] != '.')//do not print hidden files
                                {
                                    lstat(entry->d_name, &buf);
                                    if(file_check(buf)!='d')
                                    {
                                        file_match = fnmatch(pattern, entry->d_name, 0);
                                        if(file_match==0)
                                        {
                                            file[file_count]=entry->d_name;
                                            file_count++;    
                                        }
                                    }
                                    else if(file_check(buf)=='d')
                                    {
                                        directory_match = fnmatch(pattern, entry->d_name, 0);
                                        if(directory_match==0)
                                        {
                                            directory[directory_count]=entry->d_name;
                                            directory_count++;
                                        }
                                    }
                                }
                            }                
                        }
                }
            else if (exist_wild[i] == 0)
            {
                lstat(argv[i], &buf);
                if(file_check(buf)!= 'd')
                {
                    file[file_count]=argv[i];
                    file_count++;
                }
                else if(file_check(buf)=='d')
                {
                    directory[directory_count]=argv[i];
                    directory_count++;
                }
            }
        } 
        
        bubble_sort(file, file_count);
        bubble_sort(directory, directory_count); // sort
        // ls(ls_list);
        for (int i=0; i<file_count; i++)
        {
            printf("%s\n",file[i]);
        }
        for (int i=0; i<directory_count; i++)
        {
            DIR* a;
            struct dirent *b;
            char* c[1024];
            int count333 = 0;
            a = opendir(directory[i]);
            if (a != NULL){
                while ((b = readdir(a)) != NULL)
                {
                    if (b->d_name[0] != '.')
                    {
                        c[count333] = b->d_name;
                        count333++;
                    }
                }
                bubble_sort(c, count333);
                printf("%s : \n", directory[i]);
                for (int i=0; i<count333; i++)
                {
                    printf("%s\n", c[i]);
                }
            }
        }
    }
}
return 0;
}

    
