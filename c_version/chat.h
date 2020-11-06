#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/types.h>

#define SIZELIST        300
#define NUMUSERS        10
#define NAMESIZE        10
#define BUFFERSIZE      100
#define CHILDPROCESS    0
#define TRUE            1
#define FALSE           0
#define ERROROPEN       -1
#define EXIT            "exit"
#define SERVERFIFO      "serverfifo"
#define AVAILABLE       '*'
#define UNAVAILABLE     '<'
#define TOKEN           '-'
#define EMPTYSTATUS     'X'
#define NULLSTRING      '\0'
#define ACCEPT          0
#define REJECT          1

typedef enum Options
{
    init = 0,
    list ,
    connection,
    disconnect,
    waitConnection,
    exitProgram,
    chat,
    success,
    error,
    login,
    acceptConnection,
    rejectConnection,
    invalidClient,
    updateStatus,
}Options;

typedef struct User
{
    char status;
    unsigned char name[NAMESIZE];
    unsigned char myFifo[NAMESIZE];
    unsigned char destFifo[NAMESIZE];
    unsigned char chatBuffer[BUFFERSIZE];
    unsigned int id;
    unsigned int destId;
    Options  action;
}User;




