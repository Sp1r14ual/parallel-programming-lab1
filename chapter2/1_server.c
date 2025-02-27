#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int request_count = 0; // Глобальный счетчик запросов
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Мьютекс для синхронизации доступа к счетчику

// Функция для обработки запроса в отдельном потоке
void* handle_request(void* arg) {
    int client_socket = *(int*)arg;
    free(arg); // Освобождаем память, выделенную для сокета

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    // Чтение запроса от клиента
    read(client_socket, buffer, BUFFER_SIZE - 1);

    // Увеличиваем счетчик запросов с синхронизацией
    pthread_mutex_lock(&mutex);
    request_count++;
    int current_request = request_count;
    pthread_mutex_unlock(&mutex);

    // Формируем ответ
    snprintf(response, BUFFER_SIZE,
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain\r\n"
             "Connection: close\r\n\r\n"
             "Request number %d has been processed", current_request);

    // Отправляем ответ клиенту
    write(client_socket, response, strlen(response));

    // Закрываем соединение
    close(client_socket);

    return NULL;
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);
    pthread_t thread_id;

    // Создаем сокет
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }

    // Настраиваем адрес сервера
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Привязываем сокет к адресу
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка при привязке сокета");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Начинаем слушать входящие соединения
    if (listen(server_socket, 5) < 0) {
        perror("Ошибка при прослушивании");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d...\n", PORT);

    while (1) {
        // Принимаем входящее соединение
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket < 0) {
            perror("Ошибка при принятии соединения");
            continue;
        }

        // Выделяем память для сокета клиента (для передачи в поток)
        int* client_socket_ptr = malloc(sizeof(int));
        *client_socket_ptr = client_socket;

        // Создаем поток для обработки запроса
        if (pthread_create(&thread_id, NULL, handle_request, client_socket_ptr) != 0) {
            perror("Ошибка при создании потока");
            free(client_socket_ptr);
            close(client_socket);
        }

        // Отсоединяем поток, чтобы он мог завершиться самостоятельно
        pthread_detach(thread_id);
    }

    // Закрываем серверный сокет (этот код никогда не выполнится в данном примере)
    close(server_socket);
    return 0;
}