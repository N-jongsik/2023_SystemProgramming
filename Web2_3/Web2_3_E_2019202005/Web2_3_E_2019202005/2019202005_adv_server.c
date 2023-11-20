//////////////////////////////////////////////////////////////////////
//File:2019202005_adv_server.c                                      //
//Date:2023/05/10                                                   //
//OS:Ubuntu 16.04 LTS 64bits                                        //
//Author:Nam jongsik                                                //
//Student ID:2019202005                                             //
//-----------------------------------------------------             //
//Title:system programming assignment #2-3                          //
//Description:Implementing the multi access web server         	    //
//////////////////////////////////////////////////////////////////////
#define _GNU_SOURCE
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
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>

#define URL_LEN 256
#define BUFSIZE 1024
#define PORTNO 40000

char temp[BUFSIZE*BUFSIZE] = {
    0,
}; // temporary store response message

void file_info(int lfalg, int aflag, char *argv, int k, struct stat buf[], char *file[]); // define file info function
void bubble_sort(char **file, int n, struct stat buf[]);                                  // define bubble sort
void ls_l(char *argv, DIR *dirp, struct dirent *entry, char **file);                      // define ls -l function
void ls_al(char *argv, DIR *dirp, struct dirent *entry, char **file);                     // define ls -al function
void print_clients_info();//define print_clients_info function
void handle_signal(int sig);//define handle_signal function

struct client_info
{
    int num;
    int pid;
    int port;
    char addr[30];
    time_t connect_time;
};//struct client info
struct client_info information[10];//define struct array

int recent_count = 0;
int total_count = 0;

////////////////////////////////////////////////////////////
//Main                                                    //
//------------------------------------------------------  //
//Output:0->Return a value of 0 to terminate the program  //
//Purpose:Implement Multi-Access Web Server               //
////////////////////////////////////////////////////////////
int main()
{
    struct sockaddr_in server_addr, client_addr; //server address, client address
    int socket_fd, client_fd; //socket descriptor, client descriptor
    int len, len_out;
    int opt = 1;
    char cwd[4096];// get current directory
    int ip_check=1;
    
    int total = 0; //total size
    int byte_read; //return read function 
    int file_fd; //file descriptor

    DIR *dirp;             // pointer to directory stream
    struct dirent *entry;  // Pointer to a dirent structure
    char *file[4096];      // file list
    struct stat buf[4096]; // stat structure that stores information about a file or directory

    char MIME_type[BUFSIZE]; //check MIME_type


    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0) //Call the socket function to create a socket file descriptor
    {
        printf("Server : Can't open stream socket\n"); //exception
        return 0;
    }
    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); //to enable reuse of previously used port numbers

    bzero((char*)&server_addr , sizeof(server_addr)); //Initialize server_addr structurea
    server_addr.sin_family = AF_INET; 
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //Accept client's connection request
    server_addr.sin_port = htons(PORTNO);

    if (bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) //bind socket with ip, port
    {
        printf("Server : Can't bind local address\n"); //exception
        return 0;
    }
    listen(socket_fd, 5); //wait client socket

    signal(SIGALRM, handle_signal);//signal handler
    alarm(10);//timer

    char url[URL_LEN] = {
            0,
        }; //url
        
    char wow[4096] = {0, };
    getcwd(cwd, sizeof(cwd)); // get current working directory
    sprintf(wow, "%s%s", cwd, url); //wow=cwd+url53

    if (!opendir(wow)) //not directory
    {
        if (access(wow, F_OK) >= -1) //no exist file
            {
                sprintf(temp, "<h3><br>Not Found</br></h3>The request URL %s was not found on this server<p>HTTP 404 -Page Not Found</p>", url); //print 404 error
            }
    }

    while (1)
    {
        struct in_addr inet_client_address;
        char buf[BUFSIZE] = {
            0,
        }; 
        char tmp[BUFSIZE] = {
            0,
        };
        char response_header[BUFSIZE] = {
            0,
        }; //response header
        char response_message[BUFSIZE] = {
            0,
        }; //response message
        char url[URL_LEN] = {
            0,
        }; //url
        char method[20] = {
            0,
        }; //method
        char *tok = NULL;
    
        len = sizeof(client_addr); //size of client address
        client_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &len); //to get the socket file descriptor associated with the client
        
        if (client_fd < 0)
        {
            printf("Server : accept failed\n"); //exception
            return 0;
        }
        pid_t pid;
        
        if((pid=fork())==0)//child
        {
            inet_client_address.s_addr = client_addr.sin_addr.s_addr; //store ip address
            byte_read = read(client_fd, buf, BUFSIZE); //read client descriptor
            if (byte_read == 0) //exception client was connected
            {
                continue;
            }

            strcpy(tmp, buf); //tmp<-buf
            puts("=============== New Client ================");
            printf("IP : %s\n",inet_ntoa(inet_client_address));//print ip
            printf("Port : %d\n",client_addr.sin_port);//print port
            puts("===========================================\n\n");

            tok = strtok(tmp, " ");
            strcpy(method, tok); //Copy HTTP method information
            if (strcmp(method, "GET") == 0) //when HTTP method == "GET"
            {
                tok = strtok(NULL, " ");  
                strcpy(url, tok); //Copy url information
            }

            char wow[4096] = {0, };
            getcwd(cwd, sizeof(cwd)); // get current working directory
            sprintf(wow, "%s%s", cwd, url); //wow=cwd+url

            if(strcmp(wow,"favicon.ico")==0)
            {
                continue;
            }

            FILE* ip_info = fopen("accessible.usr", "r");//open file
            char ip_list[1024];

            while (fgets(ip_list, sizeof(ip_list), ip_info))//read file
            {
                size_t len = strlen (ip_list);//get size
                if(len>0 && ip_list[len-1]=='\n')
                {
                    ip_list[len-1]='\0';//remove null
                    --len;
                }

                if (fnmatch(ip_list, inet_ntoa(inet_client_address), 0) == 0)//when ip exist in file
                {   
                    ip_check=0;//control flag
                    break;
                }
            }

            if(ip_check!=0)//when no exist in file
            {
                sprintf(response_header, "HTTP/1.1 404 not found\r\n"
                                            "Content-Type:text/html;charset=UTF-8\r\n\r\n"); //print response header
                sprintf(response_message, "<h4>Access denied!</h4>""<br><h5>Your IP:%s</h5></br>""<br><h6>You have no permission to access this web server.</h6></br>""<br><h7>HTTP 403.6 - Forbidden: IP address rejected</h7></br>" ,inet_ntoa(inet_client_address));

                    
                write(client_fd, response_header, strlen(response_header)); //write response message
                write(client_fd, response_message, strlen(response_message)); //write response message
                close(client_fd); //close socket
            }
            
            struct stat check_buf;
            int exsist_file = lstat(wow,&check_buf); //get file info
            
            if (opendir(wow)) //open directory
            {
                if (strcmp(url, "/") == 0)
                {
                    ls_l(url, dirp, entry, file); // call ls -l function
                }
                else
                {
                    ls_al(url, dirp, entry, file); // call ls -al function
                }
            }

            lstat(wow,&check_buf); //get file info
            long size; //lont type size

            if (S_ISDIR(check_buf.st_mode)) //when directory
            {
        
                strcpy(MIME_type, "text/html"); //MIME_type=text/html
                size=strlen(response_message); //get size of response message
                strcpy(response_message, temp); //response message=temp
                memset(temp, 0, sizeof(temp)); //initailize temp
                sprintf(response_header, "HTTP/1.0 200 OK\r\n"
                                        "Server:2023 simple web server \r\n"
                                        "Content-length:%lu\r\n"
                                        "Content-type:%s\r\n\r\n",
                                        size, MIME_type); //print response header
                
                write(client_fd, response_header, strlen(response_header)); //write response header
                write(client_fd, response_message, strlen(response_message)); //write response message
            }
            else if((fnmatch("*.jpg",wow, FNM_CASEFOLD)==0)||
            (fnmatch("*.png",wow, FNM_CASEFOLD)==0)||
            (fnmatch("*.jpeg",wow, FNM_CASEFOLD)==0)) //when image file
            {
                char buffer[BUFSIZE] = {0, };
                FILE* file_open = fopen(wow, "rb"); //file open
                
                fseek(file_open, 0, SEEK_END); //get file
                long int size = ftell(file_open);
                fseek(file_open, 0, SEEK_SET);

                sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: image/*\r\nContent-Length:%ld\r\n\r\n",size);//save message
                write(client_fd, response_header, strlen(response_header)); //write response header

                int file_read = 0;
                while ((file_read = fread(buffer, 1, BUFSIZE, file_open)) > 0) //file read and write 
                {
                    write(client_fd, buffer, file_read);
                }
                
                fclose(file_open); //close files
            }

            else if (S_ISREG(check_buf.st_mode)) //when file
            {
                char buffer[BUFSIZE] = {0, };
                FILE* file_open = fopen(wow, "rb"); //file open
                
                fseek(file_open, 0, SEEK_END); //get file
                long int size = ftell(file_open);
                fseek(file_open, 0, SEEK_SET);

                sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length:%ld\r\n\r\n",size);//save message
                write(client_fd, response_header, strlen(response_header)); //write response header

                int num_read = 0;
                while ((num_read = fread(buffer, 1, BUFSIZE, file_open)) > 0) //file read and write
                {
                    write(client_fd, buffer, num_read);
                }

                fclose(file_open); //close files
            }
        
            printf("=========== Disconnected Client ===========\n");
            printf("IP : %s\n",inet_ntoa(inet_client_address));//print ip
            printf("Port : %d\n",client_addr.sin_port);//print port
            printf("===========================================\n\n");

            close(client_fd); //close cient socket descriptor
            exit(0);
        }
        else if(pid>0)
        {
            struct client_info client;
            total_count++;
            client.num = total_count;//get total reguest
            client.pid = pid;//get pid
            inet_ntop(AF_INET, &(client_addr.sin_addr), client.addr, INET_ADDRSTRLEN);//get client ip
            client.port = client_addr.sin_port;//get client port
            client.connect_time = time(NULL);//get connection time

            if (recent_count < 10) //store client information
            {
                information[recent_count] = client;
                recent_count++;
            } 
            else {
                for (int i = 1; i < 10; i++) //store maximum 10
                {
                    information[i-1] = information[i];
                }
                information[9] = client;
            }
            int status;
            while(waitpid(-1,&status,WNOHANG)>0);//wait until child die
        }
    }

    close(socket_fd); //close socket descriptor
    return 0;
}
////////////////////////////////////////////////////////////
//print_clients_info                                      //
//------------------------------------------------------  //
//Purpose:print client information                        //
////////////////////////////////////////////////////////////
void print_clients_info() {
    printf("\n=================== Connection History ===================\n");
    printf("Number of request(s) : %d\n",total_count);
    printf("No.\tIP\t\tPID\tPort\tTime\n");
    for (int i = 0; i < recent_count; i++) {
        printf("%d\t%s\t%d\t%d\t%s", information[i].num, information[i].addr, information[i].pid,  information[i].port, ctime(&(information[i].connect_time)));
    }
}

////////////////////////////////////////////////////////////
//handle_signal                                           //
//------------------------------------------------------  //
//Purpose:call print_clients_info function and set alarm  //
////////////////////////////////////////////////////////////
void handle_signal(int sig)
{
    if (sig == SIGALRM) 
    {
        print_clients_info();
    }
    alarm(10);//timer
}
//////////////////////////////////////////////////////////////////////////////////////////
// file_info                                                                            //
//-------------------------------------------------------                               //
// Input:int lflag,aflag, int k: the number of files, struct stat: get file information //
//           char* file[]:file list int argc:store the number of input arguments        //
//           struct passwd *:get user info struct group *:get group info                //
//           struct tm*:get time info                                                   //
// Purpose:print directory's more information                                           //
//////////////////////////////////////////////////////////////////////////////////////////
void file_info(int lflag, int aflag, char *argv, int k, struct stat buf[], char *file[]) // define file info function
{
    char permission[] = "----------";                                                                     // initialize permission array
    char *Month[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}; // initialize month array
    int n;                                                                                                // initialize value
    char cwd[4096];

    struct passwd *pw; //passwd structure
    struct group *grp; //group stucture
    struct tm *time; //time structure
    getcwd(cwd, 4096); // get current directory

    sprintf(temp,"%s<title>%s</title>",temp,cwd); //print title current directory
    sprintf(temp, "%s<table border=\"1\">", temp); // make table
    sprintf(temp, "%s<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>", temp); // make header

    for (n = 0; n < k; n++)
    {
        if (S_ISREG(buf[n].st_mode))
            permission[0] = '-'; // regular file-> - 
        else if (S_ISDIR(buf[n].st_mode))
            permission[0] = 'd'; // directory-> d
        else if (S_ISCHR(buf[n].st_mode))
            permission[0] = 'c'; // character special-> c
        else if (S_ISBLK(buf[n].st_mode))
            permission[0] = 'b'; // block special-> b
        else if (S_ISFIFO(buf[n].st_mode))
            permission[0] = 'p'; // FIFO -> p
        else if (S_ISLNK(buf[n].st_mode))
            permission[0] = 'l';
        else if (S_ISSOCK(buf[n].st_mode))
            permission[0] = 's'; // socket-> s

        if (buf[n].st_mode & S_IRUSR)
        {
            permission[1] = 'r'; // owner has read permission
        }
        else
        {
            permission[1] = '-'; // owner has no read permission
        }
        if (buf[n].st_mode & S_IWUSR)
        {
            permission[2] = 'w'; // owner has write permission
        }
        else
        {
            permission[2] = '-'; // owner has no write permission
        }
        if (buf[n].st_mode & S_IXUSR)
        {
            permission[3] = 'x'; // owner has execute permission
        }
        else
        {
            permission[3] = '-'; // owner has no execute permission
        }
        if (buf[n].st_mode & S_IRGRP)
        {
            permission[4] = 'r'; // group has read permission
        }
        else
        {
            permission[4] = '-'; // group has no read permission
        }
        if (buf[n].st_mode & S_IWGRP)
        {
            permission[5] = 'w'; // group has write permission
        }
        else
        {
            permission[5] = '-'; // group has no write permission
        }
        if (buf[n].st_mode & S_IXGRP)
        {
            permission[6] = 'x'; // group has execute permission
        }
        else
        {
            permission[6] = '-'; // group has no execute permission
        }
        if (buf[n].st_mode & S_IROTH)
        {
            permission[7] = 'r'; // others have read permission
        }
        else
        {
            permission[7] = '-'; // others have no read permission
        }
        if (buf[n].st_mode & S_IWOTH)
        {
            permission[8] = 'w'; // others have write permission
        }
        else
        {
            permission[8] = '-'; // others have no write permission
        }
        if (buf[n].st_mode & S_IXOTH)
        {
            permission[9] = 'x'; // others have execute permission
        }
        else
        {
            permission[9] = '-'; // others have no execute permission
        }

        pw = getpwuid(buf[n].st_uid);              // get File Owner's Information
        grp = getgrgid(buf[n].st_gid);             // get the information of the file's owning group
        const time_t *modiTime = &buf[n].st_mtime; // to get the last modified time of that file
        time = localtime(modiTime);                // function for Parsing Time Information

        if (lflag == 1 && aflag == 0) //when root directory
        {
            if (permission[0] == 'd')
            {
                sprintf(temp, "%s<tr style=\"color:blue\"><td><a href=\"%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>", temp, file[n], file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min); // direcotory blue color
            }
            else if (permission[0] == 'l')
            {
                sprintf(temp, "%s<tr style=\"color:green\"><td><a href=\"%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>", temp, file[n], file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min); // link green color
            }
            else
            {
                sprintf(temp, "%s<tr style=\"color:red\"><td><a href=\"%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>", temp, file[n], file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min); // file red color
            }
        }
        else if (lflag == 0 && aflag == 1) //when sub directory
        {
            if (argv[strlen(argv) - 1] == '/') // When the '/' is at the end of the address
            {
                if (permission[0] == 'd')
                {
                    sprintf(temp, "%s<tr style=\"color:blue\"><td><a href=\"%s%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>", temp, argv, file[n], file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min); // directory blue color
                }
                else if (permission[0] == 'l')
                {
                    sprintf(temp, "%s<tr style=\"color:green\"><td><a href=\"%s%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>", temp, argv, file[n], file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min); // link green color
                }
                else
                {
                    sprintf(temp, "%s<tr style=\"color:red\"><td><a href=\"%s%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>", temp, argv, file[n], file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min); // file red color
                }
            }
            else //When there is no '/' at the end of the address
            {
                if (permission[0] == 'd')
                {
                    sprintf(temp, "%s<tr style=\"color:blue\"><td><a href=\"%s/%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>", temp, argv, file[n], file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min); // directory blue color
                }
                else if (permission[0] == 'l')
                {
                    sprintf(temp, "%s<tr style=\"color:green\"><td><a href=\"%s/%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>", temp, argv, file[n], file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min); // link green color
                }
                else
                {
                    sprintf(temp, "%s<tr style=\"color:red\"><td><a href=\"%s/%s\">%s</a></td><td>%s</td><td>%li</td><td>%s</td><td>%s</td><td>%li</td><td>%s\t%-d\t%02d : %02d</td></tr>", temp, argv, file[n], file[n], permission, buf[n].st_nlink, pw->pw_name, grp->gr_name, buf[n].st_size, Month[time->tm_mon], time->tm_mday, time->tm_hour, time->tm_min); // file red color
                }
            }
        }
    }
    sprintf(temp, "%s</table>", temp); //close table
}
///////////////////////////////////////////////////////////
//Bubble sort                                            //
//-------------------------------------------------------//
//Input:char**files:file in which the filename is stored //
//      int n:length of file                             //
//      struct stat buf[]: get information               //
//Purpose:Sorts the names of files and directories       //
///////////////////////////////////////////////////////////
void bubble_sort(char **file, int n, struct stat buf[]) // define bubble function
{                          
    int i, j;              // initialize value
    struct stat temp_size; 
    for (i = 0; i < n - 1; i++) // Bubble sort the file names alphabetically
    {
        for (j = 0; j < n - i - 1; j++) // if files[j]>files[j+1]
        { 
            if (strcasecmp(file[j], file[j + 1]) > 0)
            {
                char *temp = file[j];
                file[j] = file[j + 1];
                file[j + 1] = temp; // Swap the previous and next elements of an array

                temp_size = buf[j];
                buf[j] = buf[j + 1];
                buf[j + 1] = temp_size; // Swap the previous and next elements of an array
            } // end of if
        } // end of for
    } // end of for
}
///////////////////////////////////////////////////////////////////////////////
//ls_l                                                                       //
//-------------------------------------------------------                    //
//Input:DIR*:directory pointer, struct dirent* :directory stream             //
//          char**:file list int argc:store the number of input arguments    //
//          char*argv:url                                                    //
//Purpose:print directory & file names & file information                    //
///////////////////////////////////////////////////////////////////////////////
void ls_l(char *argv, DIR *dirp, struct dirent *entry, char **file) // define ls -l function
{
    int lflag = 1;
    int aflag = 0;
    int i = 0;             // initialize value
    int total = 0;         // initialize value
    struct stat buf[4096]; // stat structure that stores information about a file or directory

    char cwd[4096];           // declare cwd
    getcwd(cwd, sizeof(cwd)); // cwd:current directory
    char cwd_copy[4096];
    char cwd_copy_copy[4096];
    strcpy(cwd_copy, cwd); // copy current directory

    if ((dirp = opendir(cwd)) != NULL) // open directory
    {
        while ((entry = readdir(dirp)) != NULL) // read directory
        {
            if (entry->d_name[0] == '.') // do not print hidden files
            {
                continue;
            }
            else
            {
                file[i] = entry->d_name; // store file names
                i++;
            }
        }

        for (int j = 0; j < i; j++)
        {
            strcpy(cwd_copy_copy, cwd_copy); //cwd copy copy=current directory+url
            strcat(cwd_copy_copy, "/"); //cwd copy copy=current directory+url+/
            lstat(strcat(cwd_copy_copy, file[j]), &buf[j]); //get info about current directory+url+/+file
            total += buf[j].st_blocks; // get block size
            memset(cwd_copy_copy, 0, sizeof(cwd_copy_copy)); //initialize cwd_copy_copy
        }

        sprintf(temp, "<h1>Welcome to System Programming Http</h1><br>"
                      "<B>Directory path : %s<br>"
                      "total %d</B><br>",
                cwd, total / 2); //print directory path & total size
        bubble_sort(file, i, buf);                   // call bubble_sort function
        file_info(lflag, aflag, argv, i, buf, file); // print file info
    }
    closedir(dirp); // close directory
}
///////////////////////////////////////////////////////////////////////////////
//ls_al                                                                      //
//-------------------------------------------------------                    //
//Input:DIR*:directory pointer, struct dirent* :directory stream             //
//          char**:file list int argc:store the number of input arguments    //
//          char*argv:url                                                    //
//Purpose:print directory & file names & file information with hidden files  //
///////////////////////////////////////////////////////////////////////////////
void ls_al(char *argv, DIR *dirp, struct dirent *entry, char **file) // define ls -al function
{
    int aflag = 1;
    int lflag = 0;
    int i = 0;             // initialize value
    int total = 0;         // initialize value
    struct stat buf[4096]; // stat structure that stores information about a file or directory
    struct stat check_buf;

    char cwd[4096];           // declare cwd
    getcwd(cwd, sizeof(cwd)); // cwd:current directory
    char cwd_copy[4096];
    char cwd_copy_copy[4096];
    strcpy(cwd_copy, cwd); //cwd copy=curernt directory
    strcat(cwd_copy, argv); //cwd copy=current directory+url

    if ((dirp = opendir(cwd_copy)) != NULL) //open directory
    {
        while ((entry = readdir(dirp)) != NULL) //read directory
        {
            file[i] = entry->d_name; //store file list
            i++; // i=how many files in directory
        }

        for (int j = 0; j < i; j++)
        {
            strcpy(cwd_copy_copy, cwd_copy); //cwd copy copy=current directory+url
            strcat(cwd_copy_copy, "/"); //cwd copy copy=current directory+url+/
            lstat(strcat(cwd_copy_copy, file[j]), &buf[j]); //get info about current directory+url+/+file
            total += buf[j].st_blocks; // get block size
            memset(cwd_copy_copy, 0, sizeof(cwd_copy_copy)); //initialize cwd_copy_copy
        }

        sprintf(temp, "<h2>System Programming Http</h2><br>"
                      "<B>Directory path : %s<br>"
                      "total %d</B><br>",
                        cwd_copy, total / 2); //print directory path & toatl size

        bubble_sort(file, i, buf);                   // call bubble_sort function
        file_info(lflag, aflag, argv, i, buf, file); // print file info
    }
    closedir(dirp); // close directory
}
