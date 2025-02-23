#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <vector> // Подключаем библиотеку для работы с векторами

using namespace std;

// Количество потоков
const int NUM_THREADS = 5;

/* Функция, которую будет исполнять созданный поток */
void *thread_job(void *arg)
{
    int thread_num = *(int *)arg;
    cout << "Thread " << thread_num << " is running..." << endl;
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    // Некорректное число параметров
    if (argc != 6)
    {
        cout << "Wrong number of arguments" << endl;
        exit(-1);
    }

    // Вектор для хранения идентификаторов потоков
    vector<pthread_t> threads(NUM_THREADS);

    // Вектор для хранения аттрибутов потоков
    vector<pthread_attr_t> thread_attributes(NUM_THREADS);

    // Вектор для хранения аргументов потоков
    vector<int> thread_args(NUM_THREADS);

    // Наполняем вектор с аргументами потоков внешними параметрами
    for (int i = 0; i < NUM_THREADS; i++)
        thread_args[i] = atoi(argv[i + 1]);

    int err;

    // Создаём n потоков
    for (int i = 0; i < NUM_THREADS; ++i)
    {

        pthread_attr_t thread_attr;

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

        thread_attributes[i] = thread_attr; // Сохраняем аттрибут потока в вектор
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
        pthread_attr_destroy(&thread_attributes[i]);
        pthread_join(threads[i], NULL);
    }

    cout << "All threads have finished execution." << endl;

    return 0;
}