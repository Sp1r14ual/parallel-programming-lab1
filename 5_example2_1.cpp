#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>

using namespace std;

/* Функция, которую будет исполнять созданный поток */
void *thread_job(void *arg)
{
    cout << "Thread is running..." << endl;
}

int main()
{
    // Определяем переменные
    pthread_t thread;
    // Идентификатор потока
    pthread_attr_t thread_attr;
    // Атрибуты потока

    int err;
    // Код ошибки
    // Инициализируем переменную для хранения атрибутов потока
    err = pthread_attr_init(&thread_attr);
    if (err != 0)
    {
        cout << "Cannot create thread attribute: " << strerror(err) << endl;
        exit(-1);
    }

    // Устанавливаем минимальный размер стека для потока (в байтах)
    err = pthread_attr_setstacksize(&thread_attr, 5 * 1024 * 1024);
    if (err != 0)
    {
        cout << "Setting stack size attribute failed: " << strerror(err) << endl;
        exit(-1);
    }

    // Создаём поток
    err = pthread_create(&thread, &thread_attr, thread_job, NULL);
    if (err != 0)
    {
        cout << "Cannot create a thread: " << strerror(err) << endl;
        exit(-1);
    }
    // Освобождаем память, занятую под хранение атрибутов потока
    pthread_attr_destroy(&thread_attr);
    //
    pthread_exit(NULL);
}