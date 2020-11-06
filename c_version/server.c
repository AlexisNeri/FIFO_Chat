#include "chat.h"
User userList[NUMUSERS];
int usersConnected = 0;
bool keepAwake = TRUE;

int serverFd;
void Connect(User * client);
void UpdateUsers(User userList[NUMUSERS]);
void Chat(User * client);
void Login(User * user);
void RemoveUser(User *user);

void InitUsers()
{
	for(int index = 0; index < NUMUSERS; index++)
	{
		userList[index].status		= EMPTYSTATUS;
		userList[index].id			= init;
		userList[index].destId		= init;
		userList[index].action		= init;
	}
}

void UpdateStatus(User * client)
{
    printf("Status updated.\n");
	/*Update status in client list*/
	userList[client -> id].status = client -> status;
}

void ListUsers(User * client)
{
    printf("List users called.\n");
    char line[SIZELIST]= {"ID  Name\tStatus\n"};
    char currentLine[BUFFERSIZE];
    for(int i = 0; i < NUMUSERS; i++)
    {
		/*List valid users only*/
        if(userList[i].status != EMPTYSTATUS)
        {
            snprintf(currentLine, sizeof(currentLine),"%i   %s\t%c\n",i,userList[i].name,userList[i].status);
            strcat(line, currentLine);
        }
    }
	/*Write data to client*/
    int clientFd = open(client -> myFifo, O_WRONLY);
    write(clientFd, line, SIZELIST);
    close(clientFd);
}

void Login(User * user)
{
    printf("Login called.\n");
    int clientFifo = open(user -> myFifo, O_WRONLY);
	/*Verify the number of clients connected*/
    if(usersConnected < NUMUSERS)
    {
        /*Look for an empty position in the array*/
        for(int i = 0; i < NUMUSERS; i++)
        {
            if(EMPTYSTATUS == userList[i].status)
            {
                user -> action = acceptConnection;
                user -> id = i;
                userList[i] = *user;
                /*Update users list*/
                usersConnected++;
                write(clientFifo, user, sizeof(User));
                i = NUMUSERS;
            }
        }
    }
    else
    {
        /*Error notification to the client*/
        user -> action = rejectConnection;
        write(clientFifo, user, sizeof(User));
    }
    close(clientFifo);
}

void RemoveUser(User *user)
{
    printf("User exit called.\n");
    /*Removing user*/
    userList[user -> id].status = EMPTYSTATUS;
    usersConnected--;
	/*En program when no users are connected*/
    if(init == usersConnected)
    {
        keepAwake = FALSE;
    }
}

void Chat(User * source)
{
    printf("Chat service called.\n");
	/*Send data to destination client*/
    int destFd = open(userList[source -> destId].myFifo, O_WRONLY);
    write(destFd, source, sizeof(User));
    close(destFd);
}

void Connect(User * source)
{
	/*Connection request made by any client*/
    printf("Connection attempt called.\n");
    int destFd;
    char response[BUFFERSIZE];
	/*Check valid conditions in order to connect*/
    if((source -> id != source -> destId) && (AVAILABLE == userList[source -> destId].status))
    {
        source -> action = connection;
        destFd = open(userList[source -> destId].myFifo, O_WRONLY);
        write(destFd, source, sizeof(User));
        close(destFd);
    }
    /*Invalid or unavailable user*/
    else
    {
        source -> action = invalidClient;
        snprintf(source -> chatBuffer, BUFFERSIZE,"El usuario %d no se encuentra disponible o no existe.\n", source -> destId);
        destFd = open(source -> myFifo, O_WRONLY);
        write(destFd, source, sizeof(User));
        close(destFd);
    }
}

void AnswerConnection(User *source)
{
    printf("Connection answered.\n");
    /*Update values in user table*/
	if(acceptConnection == source -> action)
	{
		userList[source -> id].status		= UNAVAILABLE;
		userList[source -> destId].status	= UNAVAILABLE;
	}
    int destFd = open(source -> destFifo, O_WRONLY);
    write(destFd, source, sizeof(User));
    close(destFd);
}
void ReadData()
{
	/*Monitor requests by clients*/
    User tempUser;
    serverFd= open(SERVERFIFO,O_RDONLY);
    read(serverFd,&tempUser,sizeof(User));
    close(serverFd);
    switch(tempUser.action)
    {
        case login:
        {
            Login(&tempUser);
            break;
        }
        case disconnect:
        {
            RemoveUser(&tempUser);
            break;
        }
        case list:
        {
            ListUsers(&tempUser);
            break;
        }
        case connection:
        {
            Connect(&tempUser);
            break;
        }
        case chat:
        {
            Chat(&tempUser);
            break;
        }
        case acceptConnection:
        case rejectConnection:
        {
            AnswerConnection(&tempUser);
            break;
        }
        case updateStatus:
        {
            UpdateStatus(&tempUser);
            break;
        }
        default:
		{
            break;
		}
    }
}

int main()
{
    printf("Server running\n");
	InitUsers();
    mkfifo(SERVERFIFO, 0666);
    while(usersConnected > 0 || keepAwake == TRUE)
    {
        ReadData();
    }
    printf("Server down\n");
    return 0;
}