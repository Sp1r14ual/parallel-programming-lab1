#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <vector> // Подключаем библиотеку для работы с векторами

using namespace std;

// Количество потоков
const int NUM_THREADS = 10;

/* Функция, которую будет исполнять созданный поток */
void *thread_job(void *arg)
{
    int thread_num = *(int *)arg;
    cout << "Thread " << thread_num << " is running..." << endl;
    pthread_exit(NULL);
}

int main()
{
    // Вектор для хранения идентификаторов потоков
    vector<pthread_t> threads(NUM_THREADS);

    // Вектор для хранения аргументов потоков
    vector<int> thread_args(NUM_THREADS);

    int err;

    // Создаём n потоков
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        thread_args[i] = i + 1; // Устанавливаем аргумент для потока
        err = pthread_create(&threads[i], NULL, thread_job, &thread_args[i]);

        // Если при создании потока произошла ошибка, выводим
        // сообщение об ошибке и прекращаем работу программы
        if (err != 0)
        {
            cout << "Cannot create thread " << i << ": " << strerror(err) << endl;
            exit(-1);
        }
    }

    // Ожидаем завершения всех созданных потоков перед завершением
    // работы программы
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    cout << "All threads have finished execution." << endl;

    return 0;
}