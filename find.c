//Written by Jacob Zahn for Professor Yong Chen's CS4352 Operating Systems, Computer Science, Texas Tech University
//linux find utility like C implimentation as described in Project2.pdf
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

//while loop with inner recursive calls
void find_recursive(char *where_to_look, const char *args[3], char *optArgs[]);

//interpretation of find arguments
void checkCriteria(char *where_to_look, const char *args[3], char *optArgs[], char *temp_full_path, char *temp, struct stat f_stat);

//interpretation of action arguments
void execAction(char *where_to_look, char *optArgs[], char *temp_full_path);

int main(int argc, char *argv[])
{
    char *whereToLook, *name, *mmin, *inum;
    char buf[_PC_PATH_MAX];
    const char *args[3];
    args[0] = "";//criteria flag
    args[1] = "";//criteria
    args[2] = "";//action/delete
    char **actionArgs;
    //getopt flag
    int c;
    //arg flags
    int w = 0;
    int n = 0;
    int m = 0;
    int i = 0;
    int d = 0;
    int a = 0;
    int t = 0;
    
    //only find was called
    if(argc == 1)
    {
        if(getcwd(buf, sizeof(buf)));
        else{
            perror("getcwd() error");
            return -1;
        }
        
        find_recursive(buf, args, actionArgs);
    }
    else//find and args
    {
        //USAGE: find -w <whereToLook> criteria(-n/-m/-i) <name/mmin/inum> action(-d/-a) <optAction>
        while((c = getopt(argc,argv, "w:n:m:i:daht")) != -1){
            switch(c){
                case 'w':
                    w = 1;
                    whereToLook = optarg;
                    break;
                case 'n':
                    n = 1;
                    name = optarg;
                    break;
                case 'm':
                    m = 1;
                    mmin = optarg;
                    break;
                case 'i':
                    i = 1;
                    inum = optarg;
                    break;
                case 'd':
                    d = 1;
                    break;
                case 'a':
                    a = 1;
                    break;
                case 't':
                    t = 1;
                    break;
                case 'h':
                    printf("Usage: %s -w <whereToLook> criteria(-n/-m/-i) <name/mmin/inum> action(-d/-a) <optAction>\n",argv[0]);
                    return(0);
                default:
                    printf("Invalid option detected.\n");
            }
        }
        argc -= optind;
	    argv += optind;
        if(w==0){
            if(getcwd(buf, sizeof(buf))){
                whereToLook = buf;
            }
            else{
                perror("getcwd() error");
                return -1;
            }
        }
        if(n+m+i>1)
        {
            printf("Too many criteria flags provided!\n");
            return -1;
        }
        else if(n==1){
            args[0] = "-name";
            args[1] = name;
        }
        else if(m==1){
            args[0] = "-mmin";
            args[1] = mmin;
        }
        else if(i==1){
            args[0] = "-inum";
            args[1] = inum;
        }
        if(a+d>1){
            printf("Too many action flags provided!\nCannot delete and preform action!\n");
            return -1;
        }
        else if(d==1){
            args[2] = "-delete";
        }
        else if(a==1){
            args[2] = "-action";
            actionArgs = argv;
        }
        if(t==1){
            printf("whereToLook :%s\n",whereToLook);
            printf("args[0] :%s\n",args[0]);
            printf("args[1] :%s\n",args[1]);
            printf("args[2] :%s\n",args[2]);
            if (argc > 0) {
                printf("There are %d command-line arguments left to process:\n", argc);
                for (i = 0; i < argc; i++) {
                    printf("Argument %d: '%s'\n", i + 1, actionArgs[i]);
                }
            }
        }

        find_recursive(whereToLook, args, actionArgs);
    }
    return 0;
}

void find_recursive(char *where_to_look, const char *args[3], char *optArgs[])
{
    DIR *sub_dp = opendir(where_to_look);//open a directory stream
    struct dirent *sub_dirp;//define sub_dirp
	struct stat f_stat;//define a file status struct
    char temp1[] = ".";
    char temp2[] = "..";
    char temp3[] = "/";

    //check whether the directory stream is opened successfully
    if(sub_dp!=NULL)
    {
        //check criteria on current dir only if its a dir
        char *tempD ="";
        if(stat(where_to_look,&f_stat)==0){
            if((f_stat.st_mode & __S_IFMT) == __S_IFDIR)
                checkCriteria(where_to_look, args, optArgs, where_to_look, tempD, f_stat);
        }
        else{
            perror("stat error");
        }
        //then look at files/dirs in dir
        //read one entry at a time, and stop when none left
        while((sub_dirp=readdir(sub_dp))!=NULL)
        {
            //check whether the first entry is a subdirectory
            char * temp =sub_dirp->d_name;
            //to avoid recursively searching . and .. in the directory.
            if(strcmp(temp,temp1)!=0&&strcmp(temp,temp2)!=0)
            {
                //printf("temp: %s\n",temp);
                char *temp_full_path=malloc(sizeof(char)*2000);
                char *temp_sub=malloc(sizeof(char)*100);
                temp_sub=strcpy(temp_sub,temp3);
                temp_sub=strcat(temp_sub,temp);
                //now you add the / in front of the entry’s name
                //printf("temp_sub: %s\n",temp_sub);
                temp_full_path = strcpy(temp_full_path,where_to_look);
                temp_full_path = strcat(temp_full_path,temp_sub);
                //now you get a full path, e.g., testdir/dir1 or testdir/test1
                //printf("temp_full_path: %s\n",temp_full_path);
                
                //check criteria only if looking at regular file
                if(stat(temp_full_path,&f_stat)==0){
                    if((f_stat.st_mode & __S_IFMT) == __S_IFREG)
                        checkCriteria(where_to_look, args, optArgs, temp_full_path, temp, f_stat);
                }
                else{
                    perror("stat error");
                }
                
                // try to open
                DIR * subsubdp=opendir(temp_full_path);
                //if not null, means we find a subdirectory, otherwise, its just a file
                if(subsubdp!=NULL)
                {
                    //close the stream, because we will reopen it in the recursive call. 
                    closedir(subsubdp);
                    find_recursive(temp_full_path,args,optArgs);//call the recursive function call.
                }
                //free malloced memory
                free(temp_sub);
                free(temp_full_path);
            }
            
        }
        closedir(sub_dp);//close the stream
    }
    else{
        printf("cannot open directory\n");
        exit(2);
    }
    
}

void checkCriteria(char *where_to_look, const char *args[3], char *optArgs[], char* temp_full_path, char* temp, struct stat f_stat){
    //check if criteria is empty
    if(strcmp(args[0],"")==0)
    {
        //if third argument is empty 
        if(strcmp(args[2],"")==0)
        {
            //print the first entry, a file or a subdirectory
            printf("%s\n",temp_full_path);
        }
        //else if third argument is -delete
        else if(strcmp(args[2],"-delete")==0)
        {
            if(remove(temp_full_path)!=0){
                printf("find: cannot delete %s",temp_full_path);
            }
        }
        //if criteria empty dont perform action
    }
    //else check criteria
    else
    {
        if(strcmp(args[0],"-name")==0)
        {
            //check if name match
            if(strcmp(args[1],temp)==0)
            {
                //if third argument is empty 
                if(strcmp(args[2],"")==0)
                {
                    //print the first entry, a file or a subdirectory
                    printf("%s\n",temp_full_path);
                }
                //else if third argument is -delete
                else if(strcmp(args[2],"-delete")==0)
                {
                    if(remove(temp_full_path)!=0){
                        printf("find: cannot delete %s",temp_full_path);
                    }
                }
                //else if third argument is -action
                else if(strcmp(args[2],"-action")==0)
                {
                    execAction(where_to_look, optArgs, temp_full_path);
                }
            }
        }
        else if(strcmp(args[0],"-mmin")==0)
        {
            //printf("-mmin\n");
            //get num and sign
            char *num = malloc(sizeof(char)*200);
            num = strcpy(num,args[1]);
            char sign;
            if(num[0]=='-'){
                sign = '-';
                memmove(num, num+1, strlen(num));
            }
            else if(num[0]=='+'){
                sign = '+';
                memmove(num, num+1, strlen(num));
            }
            
            //get last modification time
            time_t lastModTime = f_stat.st_mtime;
            //get now
            time_t rawtime;
            time(&rawtime);
            //get difference in secs
            double diffsecs = difftime(lastModTime,rawtime);
            int diffmins = diffsecs/60;
            //check sign of time
            
            //printf("Sign value: %d\n",sign);
            //printf("Num value: %s\n",num);
            //printf("Last mod time: %ld\n",lastModTime);
            //printf("diffmins: %d\n",diffmins);
            if(sign=='-')
            {
                //if mins since modified less than num provided
                if(diffmins>atoi(num))
                {
                    //if third argument is empty 
                    if(strcmp(args[2],"")==0)
                    {
                        //print the first entry, a file or a subdirectory
                        printf("%s\n",temp_full_path);
                    }
                    //else if third argument is -delete
                    else if(strcmp(args[2],"-delete")==0)
                    {
                        if(remove(temp_full_path)!=0){
                            printf("find: cannot delete %s",temp_full_path);
                        }
                    }
                    //else if third argument is -action
                    else if(strcmp(args[2],"-action")==0)
                    {
                        execAction(where_to_look, optArgs, temp_full_path);
                    }
                }
                
            }
            else if(sign=='+')
            {
                //if mins since modified greater than num provided
                if(diffmins<atoi(num))
                {
                    //if third argument is empty 
                    if(strcmp(args[2],"")==0)
                    {
                        //print the first entry, a file or a subdirectory
                        printf("%s\n",temp_full_path);
                    }
                    //else if third argument is -delete
                    else if(strcmp(args[2],"-delete")==0)
                    {
                        if(remove(temp_full_path)!=0){
                            printf("find: cannot delete %s",temp_full_path);
                        }
                    }
                    //else if third argument is -action
                    else if(strcmp(args[2],"-action")==0)
                    {
                        execAction(where_to_look, optArgs, temp_full_path);
                    }
                }
            }
            free(num);
        }
        else if(strcmp(args[0],"-inum")==0)
        {
            //printf("strcmp return:  %d\n", strcmp(args[0],"-inum"));
            char *tempstr=malloc(sizeof(char)*200);
            sprintf(tempstr,"%lu", f_stat.st_ino);
            
            //printf("strcmp return:  %d\n", strcmp(args[1],tempstr)==0);
            if(strcmp(args[1],tempstr)==0)
            {
                //if third argument is empty 
                if(strcmp(args[2],"")==0)
                {
                    //print the first entry, a file or a subdirectory
                    printf("%s\n",temp_full_path);
                }
                //else if third argument is -delete
                else if(strcmp(args[2],"-delete")==0)
                {
                    if(remove(temp_full_path)!=0){
                        printf("find: cannot delete %s",temp_full_path);
                    }
                }
                //else if third argument is -action
                else if(strcmp(args[2],"-action")==0)
                {
                    execAction(where_to_look, optArgs, temp_full_path);
                }
            }
            //free malloced memory
            free(tempstr);
        }
    }
}

void execAction(char *where_to_look, char *optArgs[], char* temp_full_path){
    int status;
    //fork and if child then interpret action
    if(fork() == 0){
        char *command = malloc(sizeof(char)*2000);
        char *sp = " ";
        char *sl = "/";
        //check if command is supported eg. cat, rm, mv, ls, mkdir, touch
        //then interpret and run command
        if(strcmp(optArgs[0],"cat")==0){
            command = strcpy(command,optArgs[0]);
            command = strcat(command,sp);
            command = strcat(command, temp_full_path);
            system(command);
        }
        else if(strcmp(optArgs[0],"rm")==0){
            command = strcpy(command,optArgs[0]);
            command = strcat(command,sp);
            command = strcat(command, temp_full_path);
            system(command);
        }
        else if(strcmp(optArgs[0],"mv")==0){
            command = strcpy(command,optArgs[0]);
            command = strcat(command, sp);
            command = strcat(command, temp_full_path);
            command = strcat(command, sp);
            command = strcat(command, where_to_look);
            command = strcat(command, sl);
            command = strcat(command, optArgs[1]);
            system(command);
        }
        else if(strcmp(optArgs[0],"ls")==0){
            command = strcpy(command, optArgs[0]);
            command = strcat(command, sp);
            command = strcat(command, where_to_look);
            system(command);
        }
        else if(strcmp(optArgs[0],"mkdir")==0){
            command = strcpy(command, optArgs[0]);
            command = strcat(command, sp);
            command = strcat(command, where_to_look);
            command = strcat(command, sl);
            command = strcat(command, optArgs[1]);
            system(command);
        }
        else if(strcmp(optArgs[0],"touch")==0){
            command = strcpy(command, optArgs[0]);
            command = strcat(command, sp);
            command = strcat(command, where_to_look);
            command = strcat(command, sl);
            command = strcat(command, optArgs[1]);
            system(command);
        }
        //free malloced memory
        free(command);
        exit(0);
    }
    //if parent then wait
    else
        wait(&status);
}