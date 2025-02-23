#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>

using namespace std;

void *thread_job(void *arg)
{
    // Преобразуем указатель на параметр к правильному типу
    int *param = (int *)arg;

    // Выводим параметр на экран
    cout << "Thread is running... Parameter is " << *param << endl;
}

int main(int argc, char *argv[])
{
    // Определяем переменные
    pthread_t thread;
    int param; // Переменная для хранения параметра, передаваемого потоку
    int err;

    // Некорректное число параметров
    if (argc != 2)
    {
        cout << "Wrong number of arguments" << endl;
        exit(-1);
    }

    param = atoi(argv[1]);

    // Создаем поток
    err = pthread_create(&thread, NULL, thread_job, (void *)&param);
    if (err != 0)
    {
        cout << "Cannot create a thread: " << strerror(err) << endl;
        exit(-1);
    }

    // Ожидаем завершения работы потока
    pthread_join(thread, NULL);
    if (err != 0)
    {
        cout << "Cannot join a thread: " << strerror(err) << endl;
        exit(-1);
    }
}