#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#define BUFF 1024
#define BLUE "\x1b[34m"
#define GREEN "\x1b[32m"
#define BRIGHT "\x1b[1m"
#define DEFAULT "\x1b[0m"

/*login@path*/
void path();
char *login();
/*functions*/
void help();
void cd(char txt[], char *tab[]);
void touch();
void rm(char *tab[]);
void mkdir_(char *tab[]);
/*input_and_output*/
int input(char txt[], char *tab[]);
void other_cases(char *tab[]);


int main(int argc, char *argv[])
{
    char ch[BUFF];
    char prev[BUFF];
    getcwd(prev,sizeof(prev));
    char *txt;
    char *tab[255];

    while(1)
    {
        path();
        fgets(ch,BUFF,stdin);
        txt=ch;
        ch[strlen(ch)-1]='\0'; /*change \n to \0*/

        int count = input(txt,tab);
        if(tab[0]==NULL){}
        else if(!strcmp(tab[0],"help"))
            help();
        else if(!strcmp(tab[0],"exit"))
            exit(0);
        else if(!strcmp(tab[0],"cd"))
        {
            if(count<2){
                if(tab[1]!=NULL)
                    if(strcmp(tab[1],"-"))
                        getcwd(prev,sizeof(prev));
                cd(prev,tab);
            }
            else
                printf("Too many arguments\n");
        }
        else if(!strcmp(tab[0],"touch"))
            touch(tab);
        else if(!strcmp(tab[0],"mkdir"))
            mkdir_(tab);
        else if(!strcmp(tab[0],"rm"))
            rm(tab);
        else if(strcmp(tab[0],"clear") == 0) /*\033-stands for octal escape*/
            printf("\033c"); /*c-resets the device to the initial state*/
        else
            other_cases(tab);
    }
}

void path(){
    char cwd[BUFF];
    getcwd(cwd,sizeof(cwd));
    printf(GREEN BRIGHT "[%s@%s]~$ " DEFAULT,login(),cwd);
}

char *login(){
    char *log=getenv("USER");
    return log;
}

void help(){
    printf(BLUE BRIGHT "List of available commands:\n");
    printf("-> cd\n-> touch\n-> mkdir\n-> rm\n-> help\n-> clear\n-> exit\n-> other linux commands...\n" DEFAULT);
}

void cd(char txt[], char *tab[]){
    if(tab[1]==NULL || !strcmp(tab[1],"~"))
        chdir(getenv("HOME"));
    else if(!strcmp(tab[1],"-"))
        chdir(txt);
    else if(!strcmp(tab[1],"."))
        chdir(".");
    else if(!strcmp(tab[1],".."))
        chdir("..");
    else if(!strcmp(tab[1],"/"))
        chdir("/");
    else{
        if(chdir(tab[1])==-1)
            printf("%s\n", strerror(errno));
        chdir(tab[1]);
    }
}

void touch(char *tab[]){
    if(open(tab[1],O_RDONLY)!=-1)
        printf("File '%s' already exists!\n",tab[1]);
    else
        creat(tab[1],0666);
}

void mkdir_(char *tab[]){
    int file = mkdir(tab[1],0777);
    if(file)
        printf("%s\n",strerror(errno));
}

void rm(char *tab[]){
    if(remove(tab[1])!=0)
        printf("%s\n",strerror(errno));
    else
        remove(tab[1]);
}

int input(char txt[], char *tab[]){
    int i;
    char divider[] = " \n\t";
    char *ptr = strtok(txt,divider); /*ptr gives pointer on first null, which replaced divider*/

    for(i=0;ptr!=NULL;i++){
        *(tab+i) = ptr;
        ptr = strtok(NULL,divider);
    }
    tab[i] = NULL;
    return i-1; /*return arguments count*/
}

void other_cases(char *tab[]){
    if(fork()==0)
        exit(execvp(tab[0],tab));
    else{
        int status = 0;
        wait(&status);

        if(status==65280)
            printf("Command not found\n");
        else if(status!=0)
            printf("Error_code: %d\n", status);
    }
}