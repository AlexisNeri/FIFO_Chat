#include "chat.h"

int serverFd;

Options start();
bool Login();

bool Login()
{
    bool connected = FALSE;
    char userState = UNAVAILABLE;
    int action = login;
    char token = '-';
    int available = 0;
    char buffer[BUFFERSIZE];
    char name[NAMESIZE];
    printf("Escribe tu nombre de usuario.\n");
    scanf("%s",name);
    printf("Deseas aparecer disponible?\n" \
            "1. Sí.\n" \
            "2. No.\n");
    scanf("%d",&available);
    if(1 == available)
    {
        userState = AVAILABLE;
    }
    snprintf(buffer, BUFFERSIZE,"%s%c%d%c%c",name,token,action,token,userState);

    printf("%s\n", buffer);
    //escribir fifo server *connect*user*
    serverFd = open(SERVERFIFO,O_WRONLY);
    if (ERROROPEN == serverFd)
    {
        printf("Error while openning.\n");
    }
    else
    {
        write(serverFd,buffer,sizeof(buffer));
        //crear fifo con su nombre


        //leer fifo propio, checar conexion
    }
    return connected;
}
bool keepChat = TRUE;
Options DisplayOptions()
{
    bool exit = FALSE;
    int choice = 0;
    Options option = 0;
    while((option != disconnect) && (option != connect))
    {
        printf("Escoge una opción:\n"   \
               "1. Listar usuarios.\n"  \
               "2. Conectar.\n"         \
               "3. Salir.\n") ;
        scanf("%d", &choice);
        option = (Options)choice;

        switch (option)
        {
            case list:
            {
                //ListUsers();
                break;
            }
            case connect:
            {
                //Connect();
                break;
            }
            case disconnect:
            {
                //Exit();
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
{   bool connectionSuccess = FALSE;
    Options choice = init;

    connectionSuccess = Login();
    if(connectionSuccess)
    {
        choice = DisplayOptions();
    }
	int fifo_server,fifo_client;
	int PID = -1;
	char input[30];
	char output[30];


    if(choice == connect)
    {
        PID = fork();
        if(CHILDPROCESS == PID)
        {
            fifo_server = open("fifo_server",O_RDWR);
            while(keepChat)
            {
                read(fifo_server,input,sizeof(input));
                printf("%s\n",input);
                if (FALSE == strcmp(input, EXIT))
                {
                    printf("Chat terminated, press exit to return to server.\n");
                    keepChat = FALSE;
                }
            }
            close(fifo_server);
            //printf("Child process ended by server.\n");
            exit(CHILDPROCESS);
        }
        else if (PID >0)
        {
            fifo_client = open("fifo_client",O_RDWR);
            while(keepChat)
            {
                scanf("%s",output);

                write(fifo_client,output,sizeof(output));

                if (FALSE == strcmp(output, EXIT))
                {
                    printf("Chat terminated.\n");
                    keepChat = FALSE;
                }
            }
            close(fifo_client);
            printf("Child process ended by client.\n");
        }
        else
        {
            printf("Error al iniciar proceso.\n");
        }
    }
    printf("Goodbye.\n");
}
