#include <iostream>
#include <pthread.h>
#include <vector>

using namespace std;

// Тип для функции обработки элемента
typedef void (*ProcessFunc)(int &);

// Структура для передачи данных в поток
struct ThreadData
{
    int *array;       // Указатель на массив
    int start;        // Начальный индекс
    int end;          // Конечный индекс (не включая)
    ProcessFunc func; // Функция для обработки
};

// Функция потока
void *thread_worker(void *arg)
{
    ThreadData *data = static_cast<ThreadData *>(arg);
    for (int i = data->start; i < data->end; ++i)
    {
        data->func(data->array[i]);
    }
    delete data;
    return nullptr;
}

// Доступные функции обработки
void square(int &x) { x *= x; }
void increment(int &x) { x += 1; }
void double_val(int &x) { x *= 2; }

int main()
{
    // Ввод параметров
    int size, num_threads, choice;
    cout << "Enter array size: ";
    cin >> size;
    cout << "Enter number of threads: ";
    cin >> num_threads;

    // Инициализация массива
    int *array = new int[size];
    for (int i = 0; i < size; ++i)
    {
        array[i] = i + 1;
    }

    // Выбор функции
    cout << "Select function:\n"
         << "1. Square\n2. Increment\n3. Double\nChoice: ";
    cin >> choice;

    ProcessFunc selected_func = nullptr;
    switch (choice)
    {
    case 1:
        selected_func = square;
        break;
    case 2:
        selected_func = increment;
        break;
    case 3:
        selected_func = double_val;
        break;
    default:
        cerr << "Invalid choice!\n";
        delete[] array;
        return 1;
    }

    // Создание потоков
    vector<pthread_t> threads(num_threads);
    int base_chunk = size / num_threads;
    int extra = size % num_threads;
    int current_start = 0;

    for (int i = 0; i < num_threads; ++i)
    {
        ThreadData *data = new ThreadData{
            array,
            current_start,
            current_start + base_chunk + (i < extra ? 1 : 0),
            selected_func};

        if (pthread_create(&threads[i], NULL, thread_worker, data) != 0)
        {
            cerr << "Failed to create thread " << i << endl;
            delete data;
        }
        current_start = data->end;
    }

    // Ожидание завершения потоков
    for (auto &thread : threads)
    {
        pthread_join(thread, NULL);
    }

    // Вывод результатов
    cout << "Result array:\n";
    for (int i = 0; i < size; ++i)
    {
        cout << array[i] << " ";
    }
    cout << endl;

    // Освобождение памяти
    delete[] array;
    return 0;
}