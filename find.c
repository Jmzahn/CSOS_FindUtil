#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

void find_recursive(char *where_to_look, const char *args[3]);

void checkCriteria(char *where_to_look, const char *args[3], char* temp_full_path, char* temp, struct stat f_stat);

int main(int argc, char *argv[])
{
    int pathGiven = 1;
    char *whereToLook;
    char buf[_PC_PATH_MAX];
    const char *args[3];
    args[0] = "";
    args[1] = "";
    args[2] = "";
    //only find was called
    if(argc == 1)
    {
        pathGiven = 0;
        
        if(getcwd(buf, sizeof(buf)));
        else{
            perror("getcwd() error");
            return -1;
        }
        
        find_recursive(buf, args);
    }
    //find and args
    else
    {
        for(int i=1; i<argc; i++)
        {
            //check for criteria
            if(strcmp(argv[i],"-name")==0||strcmp(argv[i],"-mmin")==0||strcmp(argv[i],"-inum")==0||strcmp(argv[i],"-delete")==0)
            {
                if(i==1)//if no path provided
                {
                    pathGiven=0;
                    //save current dir as path
                    if(getcwd(buf, sizeof(buf)));
                    else{
                        perror("getcwd() error");
                        return -1;
                    }
                    //if no criteria provided
                    if(strcmp(argv[i],"-delete")==0)
                    {
                        args[0] = "";
                        args[1] = "";
                        args[2] = argv[i];
                    }else{//if not -delete must be criteria
                        args[0] = argv[i];
                        i++;
                        args[1] = argv[i];
                    }
                }else{
                    if(strcmp(argv[i],"-delete")==0)
                    {
                        args[2] = argv[i];
                    }else{//if not -delete must be criteria
                        args[0] = argv[i];
                        i++;
                        args[1] = argv[i];
                    }
                }
            }else{//if not criteria, must be whereToLook or delete
                //check if whereToLook
                if(pathGiven)
                {
                    whereToLook = argv[i];
                }else{//we shouldnt reach this if user syntax is correct
                    fprintf (stderr, "USAGE: %s (where-to-look) criteria (-delete)\n", argv[0]);
		            exit (-1);
                }
            }
        }
        if(pathGiven)
            find_recursive(whereToLook, args);
        else
            find_recursive(buf, args);
    }
    return 0;
}

void find_recursive(char *where_to_look, const char *args[3])
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
                checkCriteria(where_to_look, args, where_to_look, tempD, f_stat);
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
                        checkCriteria(where_to_look, args, temp_full_path, temp, f_stat);
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
                    find_recursive(temp_full_path,args);//call the recursive function call.
                }
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

void checkCriteria(char *where_to_look, const char *args[3], char* temp_full_path, char* temp, struct stat f_stat){
    if(strcmp(args[0],"")==0)
    {
        //if -delete
        if(strcmp(args[2],"-delete")==0)
        {
            if(remove(temp_full_path)!=0){
                printf("find: cannot delete %s",temp_full_path);
            }
        }
        else
        {
            //print the filename
            printf("%s\n",temp_full_path);
        }
    }
    //else check criteria
    else
    {
        if(strcmp(args[0],"-name")==0)
        {
            //check if name match
            if(strcmp(args[1],temp)==0)
            {
                
                //if -delete
                if(strcmp(args[2],"-delete")==0)
                {
                    if(remove(temp_full_path)!=0){
                        printf("find: cannot delete %s",temp_full_path);
                    }
                }
                else
                {
                    //print the first entry, a file or a subdirectory
                    printf("%s\n",temp_full_path);
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
                    //if -delete
                    if(strcmp(args[2],"-delete")==0)
                    {
                        if(remove(temp_full_path)!=0){
                            printf("find: cannot delete %s",temp_full_path);
                        }
                    }
                    else
                    {
                        //print the first entry, a file or a subdirectory
                        printf("%s\n",temp_full_path);
                    }
                }
                
            }
            else if(sign=='+')
            {
                //if mins since modified greater than num provided
                if(diffmins<atoi(num))
                {
                    //if -delete
                    if(strcmp(args[2],"-delete")==0)
                    {
                        if(remove(temp_full_path)!=0){
                            printf("find: cannot delete %s",temp_full_path);
                        }
                    }
                    else
                    {
                        //print the first entry, a file or a subdirectory
                        printf("%s\n",temp_full_path);
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
                //if -delete
                if(strcmp(args[2],"-delete")==0)
                {
                    if(remove(temp_full_path)!=0){
                        printf("find: cannot delete %s",temp_full_path);
                    }
                }
                else
                {
                    //print the first entry, a file or a subdirectory
                    printf("%s\n",temp_full_path);
                }
            }
            free(tempstr);
        }
    }
}