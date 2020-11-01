#include "chat.h"

User userList[NUMUSERS]= {{'*',"Pablo",		0,0,0,0,0},
                          {'*',"Isma",		0,0,0,0,0},
                          {'<',"Alexis",	0,0,0,0,0},
                          {'<',"Humberto",	0,0,0,0,0},
                          {'<',"Laura",		0,0,0,0,0},
                          {'X',"Juan",		0,0,0,0,0},
                          {'X',"Robert",	0,0,0,0,0},
                          {'X',"Zaira",		0,0,0,0,0},
                          {'<',"Arturo",	0,0,0,0,0},
                          {'<',"David",		0,0,0,0,0}};
int usersConnected = 0;

int serverFd;
void Connect();
void UpdateUsers(User userList[NUMUSERS]);

void ListUsers()
{
     /*while(i<maxusers)
        if(arreglo[i].status != 'X')
            print user ("Isma *")
            */
}


void ReadData()
{
     //serverFd= open("fifo_server",O_RDONLY);
     //printf("fd in RD: %d\n", serverFd);
}

int main()
{
    bool keepAwake = TRUE;


    char input[BUFFERSIZE];
    mkfifo(SERVERFIFO, 0666);
    /*while(usersConnected > 0 || keepAwake == TRUE)
    {
        serverFd= open(SERVERFIFO,O_RDONLY);
        read(serverFd,input,sizeof(input));
        if(EXIT == input)
        {
            keepAwake = FALSE;
        }
        printf("%s\n",input);
    }*/
    ListUsers();
    printf("Server down\n");
    close(serverFd);
    //printf("fd in main: %d\n", serverFd);
    return 0;
}