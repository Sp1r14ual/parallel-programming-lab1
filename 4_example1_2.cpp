#include <iostream>
#include <vector>
#include <chrono>
#include <pthread.h>

using namespace std;
using namespace chrono;

const int NUM_TESTS = 1000; // Количество тестов для усреднения

// Функция потока с арифметическими операциями
void *thread_job(void *arg)
{
    if (arg == nullptr)
        pthread_exit(nullptr);

    int n = *(int *)arg;

    volatile int sum = 0; // volatile для предотвращения оптимизации

    for (int i = 0; i < n; ++i)
        sum += i; // Простая операция сложения

    return nullptr;
}

// Измерение времени создания пустого потока
double measure_thread_creation_time()
{
    pthread_t thread;
    auto start = high_resolution_clock::now();
    for (int i = 0; i < NUM_TESTS; ++i)
    {
        // auto start1 = high_resolution_clock::now();
        pthread_create(&thread, nullptr, thread_job, nullptr);
        pthread_join(thread, nullptr);
        // auto end1 = high_resolution_clock::now();
        // cout << "Время запуска потока " << i + 1 << ": " << duration_cast<microseconds>(end1 - start1).count() << " мкс" << endl;
    }
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count() / (double)NUM_TESTS;
}

// Измерение времени выполнения операций в потоке
double measure_work_time(int n)
{
    pthread_t thread;
    auto start = high_resolution_clock::now();
    pthread_create(&thread, nullptr, thread_job, &n);
    pthread_join(thread, nullptr);
    auto end = high_resolution_clock::now();
    return duration_cast<microseconds>(end - start).count();
}

int main()
{
    // Замер времени создания потока
    double avg_creation_time = measure_thread_creation_time();
    cout << "Среднее время создания потока: " << avg_creation_time << " мкс" << endl;

    // Поиск минимального N, где время работы > времени создания
    int N = 1;
    while (true)
    {
        double work_time = measure_work_time(N);
        if (work_time > avg_creation_time)
        {
            cout << "Минимальное N: " << N
                 << " (время работы: " << work_time << " мкс)" << endl;
            break;
        }
        N *= 2; // Экспоненциальный поиск для ускорения
        // N++;
    }

    return 0;
}