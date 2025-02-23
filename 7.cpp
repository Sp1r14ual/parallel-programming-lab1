#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <vector>

using namespace std;

const int NUM_THREADS = 5;

// Структура для передачи параметров в поток
struct ThreadParams
{
    int thread_id;        // Идентификатор потока (пользовательский)
    pthread_attr_t *attr; // Атрибуты потока (если используются)
    size_t stack_size;    // Размер стека
    bool is_detached;     // Отсоединённый поток?
};

// Функция для получения строкового описания политики планирования
const char *get_sched_policy(int policy)
{
    switch (policy)
    {
    case SCHED_FIFO:
        return "SCHED_FIFO";
    case SCHED_RR:
        return "SCHED_RR";
    case SCHED_OTHER:
        return "SCHED_OTHER";
    default:
        return "UNKNOWN";
    }
}

void *thread_job(void *arg)
{
    if (!arg)
    {
        cerr << "Error: Nullptr passed to thread!" << endl;
        pthread_exit(nullptr);
    }

    ThreadParams *params = static_cast<ThreadParams *>(arg);

    // Получаем идентификатор потока внутри функции
    pthread_t self_id = pthread_self();

    // Выводим всю информацию
    cout << "Thread Details:\n"
         << " - User ID: " << params->thread_id << "\n"
         << " - System ID: " << self_id << "\n"
         << " - Stack Size: " << params->stack_size << " bytes\n"
         << " - Detached: " << (params->is_detached ? "Yes" : "No") << "\n";

    // Если атрибуты были переданы, выводим политику планирования
    if (params->attr)
    {
        int policy;
        pthread_attr_getschedpolicy(params->attr, &policy);
        cout << " - Scheduling Policy: " << get_sched_policy(policy) << endl;
    }

    delete params; // Освобождаем память
    pthread_exit(nullptr);
}

int main()
{
    vector<pthread_t> threads(NUM_THREADS);

    // Устанавливаем кастомные атрибуты для примера
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 1024 * 1024); // 1MB стек
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setschedpolicy(&attr, SCHED_OTHER);

    for (int i = 0; i < NUM_THREADS; ++i)
    {
        // Создаём параметры для каждого потока
        ThreadParams *params = new ThreadParams{
            i,     // thread_id
            &attr, // attr
            0,     // stack_size
            false  // is_detached
        };

        // Получаем параметры из атрибутов
        pthread_attr_getstacksize(&attr, &params->stack_size);
        int detach_state;
        pthread_attr_getdetachstate(&attr, &detach_state);
        params->is_detached = (detach_state == PTHREAD_CREATE_DETACHED);

        int err = pthread_create(
            &threads[i],
            &attr,
            thread_job,
            params);

        if (err != 0)
        {
            cerr << "Cannot create thread " << i << ": " << strerror(err) << endl;
            delete params;
            exit(-1);
        }
    }

    // Ожидаем завершения потоков
    for (auto &thread : threads)
    {
        pthread_join(thread, nullptr);
    }

    // Уничтожаем атрибуты
    pthread_attr_destroy(&attr);

    cout << "All threads finished." << endl;
    return 0;
}