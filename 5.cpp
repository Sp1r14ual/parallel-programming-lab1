#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <vector>
#include <sys/resource.h> // Для работы с приоритетами

using namespace std;

const int NUM_THREADS = 10;

void *thread_job(void *arg)
{
    int thread_num = (int)(intptr_t)arg;
    cout << "Thread " << thread_num
         //  << " (stack size: " << pthread_getattr_default_np()->stacksize / 1024 << "KB)"
         << " is running..." << endl;
    pthread_exit(NULL);
}

int main()
{
    vector<pthread_t> threads(NUM_THREADS);
    vector<pthread_attr_t> thread_attrs(NUM_THREADS);
    int err;

    // Создаём и настраиваем атрибуты для каждого потока
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_attr_init(&thread_attrs[i]);

        // Настраиваем размер стека
        size_t stack_size;
        if (i % 2 == 0)
        {
            stack_size = 2 * PTHREAD_STACK_MIN; // 2x минимальный размер
        }
        else
        {
            stack_size = 4 * PTHREAD_STACK_MIN; // 4x минимальный размер
        }
        pthread_attr_setstacksize(&thread_attrs[i], stack_size);

        // Настраиваем политику планирования
        sched_param param;
        pthread_attr_getschedpolicy(&thread_attrs[i], &param.sched_priority);

        if (i < 5)
        {
            pthread_attr_setschedpolicy(&thread_attrs[i], SCHED_OTHER);
        }
        else
        {
            pthread_attr_setschedpolicy(&thread_attrs[i], SCHED_RR);
        }

        // Создаём поток с указанными атрибутами
        int thread_num = i + 1;
        err = pthread_create(&threads[i], &thread_attrs[i], thread_job,
                             (void *)(intptr_t)thread_num);

        if (err != 0)
        {
            cerr << "Cannot create thread " << i << ": " << strerror(err) << endl;
            exit(EXIT_FAILURE);
        }
    }

    // Ожидаем завершения потоков
    for (int i = 0; i < NUM_THREADS; ++i)
    {
        pthread_join(threads[i], NULL);
        pthread_attr_destroy(&thread_attrs[i]); // Уничтожаем атрибуты
    }

    cout << "All threads have finished execution." << endl;
    return 0;
}