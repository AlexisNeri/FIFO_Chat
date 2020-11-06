#include "chat.h"

User client = {UNAVAILABLE, NULLSTRING, NULLSTRING, NULLSTRING, NULLSTRING, init, init, init, FALSE};
int clientFd;
int serverFd;

void Chat();
void Exit()
{
	/*Write exit action to server*/
    client.action = disconnect;
    serverFd = open(SERVERFIFO,O_WRONLY);
    write(serverFd, &client, sizeof(User));
    printf("Exit from server.\n");
    close(serverFd);
}

void UpdateStatus(char newStatus)
{
    client.status   = newStatus;
    client.action   = updateStatus;
    serverFd        = open(SERVERFIFO,O_WRONLY);
    write(serverFd, &client, sizeof(User));
    close(serverFd);
}

bool Login()
{
    bool connected = FALSE;
    int available = init;
    char name[NAMESIZE];
    client.action = login;
    /*Request user data*/
    printf("Escribe tu nombre de usuario.\n");
    scanf("%s",name);
    /*Fill user data*/
    strcpy(client.name, name);
    strcpy(client.myFifo, name);

    /*Request user status*/
    printf("Deseas aparecer disponible?\n" \
            "1. Sí.\n" \
            "2. No.\n");
    scanf("%i",&available);

    if(TRUE == available)
    {
        client.status = AVAILABLE;
    }
    //Open server FIFO
    serverFd = open(SERVERFIFO,O_WRONLY);
    if (ERROROPEN == serverFd)
    {
        printf("Error while openning.\n");
    }
    else
    {
        bool requestAnswered = FALSE;
        /*Send login request*/
        write(serverFd,&client,sizeof(User));
        close(serverFd);
        printf("Login request sent.\n");
        //Create own fifo
        mkfifo(client.myFifo, 0666);
        printf("Waiting for answer from server.\n");
        clientFd = open(client.myFifo, O_RDONLY);
        /*Wait for connection answer*/
        while(!requestAnswered)
        {
            read(clientFd, &client, sizeof(User));
            if(acceptConnection == client.action)
            {
                connected = TRUE;
                requestAnswered = TRUE;
                printf("Login accepted by server.\n");
            }
            else if(rejectConnection == client.action)
            {
                connected = FALSE;
                requestAnswered = TRUE;
                printf("Login rejected by server.\n");
            }
        }
        close(clientFd);
    }
    return connected;
}

void ListUsers()
{
    /*request action from server*/
    client.action = list;
    serverFd = open(SERVERFIFO,O_WRONLY);
    write(serverFd, &client, sizeof(User));
    close(serverFd);
    /*wait for server action*/
    char input[SIZELIST];
    clientFd = open(client.myFifo, O_RDONLY);
    /*Read input from server*/
    read(clientFd, input, SIZELIST);
    printf("%s\n",input);
    close(clientFd);
}

void AttendRequest()
{
	/*Attend connection request from another client*/
    printf("Attending request.\n");
    User tempUser;
    int choice = init;
    char input[SIZELIST];
    clientFd = open(client.myFifo, O_RDONLY);
    /*Read input from server*/
    read(clientFd, &tempUser, sizeof(tempUser));
    close(clientFd);
    bool connectionEstablished = FALSE;

    if(connection == tempUser.action)
    {
        printf("El usuario %s está intentando conectar. \n0. Aceptar.\n1. Rechazar.\n", tempUser.name);
        scanf("%d", &choice);
        if(REJECT == choice)
        {
            client.action = rejectConnection;
        }
        else if (ACCEPT == choice)
        {
			/*Confirm connection to the server*/
            connectionEstablished = TRUE;
            client.action = acceptConnection;
            /*Update client destination*/
            memcpy(client.destFifo,tempUser.myFifo,NAMESIZE);
            client.destId = tempUser.id;
        }
		/*Write response to server*/
        serverFd = open(SERVERFIFO, O_WRONLY);
        write(serverFd, &client, sizeof(client));
        close(serverFd);
    }
	/*Initiate chat if connected*/
    if(connectionEstablished)
    {
        Chat();
        UpdateStatus(AVAILABLE);
    }
}

void Chat()
{
    printf("Chat started.\n");
	pid_t ChildPid = -1;
    bool keepChat = TRUE;
    /*Create child process for real time chat*/
    ChildPid = fork();
    /*Child process reads from server*/
    if(CHILDPROCESS == ChildPid)
    {
        char input[BUFFERSIZE];
        bool keepChat = TRUE;
        User tempUser;
        while(keepChat)
        {
            int myFifo = open(client.myFifo,O_RDWR);
            read(myFifo,&tempUser,sizeof(User));
            printf("%s\n",tempUser.chatBuffer);
            if (FALSE == strcmp(tempUser.chatBuffer, EXIT))
            {
                printf("Chat terminated, press exit to return to server.\n");
                keepChat = FALSE;
                close(myFifo);
            }
        }
        //printf("Child process ended by server.\n");
        exit(CHILDPROCESS);
    }
    /*Parent process writes server*/
    else if (ChildPid >0)
    {
        char output[BUFFERSIZE];
        User tempUser;
        bool keepChat           = TRUE;
        client.action           = chat;

        while(keepChat)
        {
            serverFd = open(SERVERFIFO, O_RDWR);
            scanf("%s",client.chatBuffer);
            //fgets(client.chatBuffer, BUFFERSIZE, stdin);
            write(serverFd, &client, sizeof(User));
            if (FALSE == strcmp(client.chatBuffer, EXIT))
            {
                printf("Chat terminated.\n");
                keepChat = FALSE;
                close(serverFd);
                kill(ChildPid, SIGTERM);
            }

        }
    }
    else
    {
        printf("Error al iniciar proceso.\n");
    }
}

void AttemptConnection()
{
	/*Request connection to other client*/
    bool connectionEstablished = FALSE;
    int destId = 0;
    bool waitForConnection = TRUE;

    printf("Escoge el ID a conectar.\n");
    scanf("%d", &destId);
    client.action = connection;
    client.destId = destId;
    serverFd = open(SERVERFIFO,O_WRONLY);
    printf("Attempting connection.\n");
	/*Send request*/
    write(serverFd, &client, sizeof(User));
    close(serverFd);
	/*Wait for answer from server*/
    clientFd = open(client.myFifo, O_RDONLY);
    User tempUser;
	read(clientFd, &tempUser, sizeof(User));
	switch(tempUser.action)
	{
		case acceptConnection:
		{
			printf("Connection accepted, starting chat.\n");
			connectionEstablished = TRUE;
			break;
		}
		case rejectConnection:
		{
			printf("Connection rejected.\n");
			break;
		}
		case invalidClient:
		{
			printf("Connection invalid.\n");
			break;
		}
	}
    close(clientFd);
	/*Start chat if connected*/
    if(connectionEstablished)
    {
        Chat();
        UpdateStatus(AVAILABLE);
    }
}

Options DisplayOptions()
{
	/*Display menu*/
    bool exit = FALSE;
    int choice = 0;
    int dest = 0;
    Options option = 0;
    while((option != disconnect))
    {
        printf("Escoge una opción:\n"   \
               "1. Listar usuarios.\n"  \
               "2. Conectar.\n"         \
               "3. Salir.\n"            \
               "4. Esperar conexión.\n");
        scanf("%d", &choice);
        option = (Options)choice;
        //fflush(stdin);
        switch (option)
        {
            case list:
            {
                ListUsers();
                break;
            }
            case connection:
            {
                AttemptConnection();
                break;
            }
            case disconnect:
            {
                Exit();
                break;
            }
            case waitConnection:
            {
                AttendRequest();
                break;
            }
            default:
            {
                printf("Opción incorrecta.\n");
                break;
            }
        }
    }
    return option;
}
int main()
{
    bool connectionSuccess = FALSE;
    Options choice = init;
    connectionSuccess = Login();
    if(connectionSuccess)
    {
        choice = DisplayOptions();
    }
    printf("Goodbye.\n");
}