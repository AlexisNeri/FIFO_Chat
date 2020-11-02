#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/types.h>

#define NUMUSERS        10
#define NAMESIZE        10
#define BUFFERSIZE      50
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

typedef enum Options
{
    init = 0,
    list ,
    connect,
    disconnect,
    exitProgram,
    success,
    error,
    login,
    acceptConnection,
    rejectConnection,
}Options;

typedef struct User
{
    unsigned char status;
    unsigned char name[NAMESIZE];
    unsigned char myFifo[20];
    unsigned char destFifo[20];
    unsigned char chatBuffer[BUFFERSIZE];
    unsigned int id;
    Options  action;
}User;




