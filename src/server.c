#include <arpa/inet.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

sem_t x, y;
pthread_t threadNumber;
pthread_t readerthreads[1000];
int readercount = 0;

struct ClientInfo
{
    int id;
    int sequence;
    int temperature;
};

void *printer(struct ClientInfo *info)
{
    struct ClientInfo *clientInfo = info;
    sem_wait(&x);
    readercount++;

    if (readercount == 1)
        sem_wait(&y);

    sem_post(&x);

    printf("\n%d reader is inside\n", readercount);
    printf("\n Id: %d\n", clientInfo->id);
    printf("Sequence: %d\n", clientInfo->sequence);
    printf("Temperature: %d\n", clientInfo->temperature);

    sleep(1);

    sem_wait(&x);
    readercount--;

    if (readercount == 0)
        sem_post(&y);

    sem_post(&x);

    printf("\n%d Reader is leaving\n", readercount + 1);
    pthread_exit(NULL);
}

int main()
{
    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;

    socklen_t addr_size;
    sem_init(&x, 0, 1);
    sem_init(&y, 0, 1);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8989);
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (listen(serverSocket, 50) == 0)
        printf("Listening\n");
    else
        printf("Error\n");

    pthread_t threadNumber[100];

    int i = 0;

    while (1)
    {
        for (int i = 0; i < 10; i++)
        {
            addr_size = sizeof(serverStorage);

            newSocket = accept(serverSocket, (struct sockaddr *)&serverStorage, &addr_size);
            struct ClientInfo* info = (struct ClientInfo *)malloc(sizeof(struct ClientInfo));
            recv(newSocket, info, sizeof(info), 0);

            if (pthread_create(&readerthreads[i++], NULL, printer, info) != 0)
                printf("Failed to create thread\n");
        }
    }

    return 0;
}