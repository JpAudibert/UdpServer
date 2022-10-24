#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

struct ClientInfo
{
    int id;
    int sequence;
    double temperature;
};

void *clientThread(void* args)
{
    struct ClientInfo *client_request = (struct ClientInfo *) args;
    int network_socket;

    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(8989);

    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    if (connection_status < 0)
    {
        puts("Error\n");
        return 0;
    }

    if(client_request->id == 1)
        printf("Connection established\n");

    send(network_socket, &client_request, sizeof(client_request), 0);

    close(network_socket);
    pthread_exit(NULL);

    return 0;
}

int main()
{
    pthread_t tid[10];

    while (1)
    {
        for (int i = 0; i < 10; i++)
        {
            struct ClientInfo *info = (struct ClientInfo *)malloc(sizeof(struct ClientInfo));
            info->id = i + 1;
            info->sequence = i + 1;
            info->temperature = rand() % 40;

            pthread_create(&tid[i], NULL, clientThread, (void *) (info));

            pthread_join(tid[i], NULL);
        }
        sleep(5);
    }
}