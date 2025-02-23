#include <cstdlib>
#include <iostream>
#include <cstring>
#include <pthread.h>
#include <vector>
#include <sstream>
#include <stdexcept>

using namespace std;

struct ThreadParams
{
    int id;
    double value;
};

void *thread_job(void *arg)
{
    ThreadParams *params = static_cast<ThreadParams *>(arg);
    cout << "Thread " << params->id
         << " processed value: " << params->value
         << endl;
    delete params;
    pthread_exit(NULL);
}

double parseDouble(const string &arg)
{
    istringstream iss(arg);
    double value;
    if (!(iss >> value))
    {
        throw invalid_argument("Invalid numeric value: " + arg);
    }
    return value;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0]
             << " <value1> <value2> <value3> ...\n"
             << "Example: " << argv[0] << " 3.14 2.718 1.618\n";
        return EXIT_FAILURE;
    }

    const int NUM_THREADS = argc - 1;
    vector<pthread_t> threads(NUM_THREADS);

    try
    {
        for (int i = 0; i < NUM_THREADS; ++i)
        {
            ThreadParams *params = new ThreadParams;
            params->id = i + 1;
            params->value = parseDouble(argv[i + 1]);

            int err = pthread_create(&threads[i], NULL, thread_job, params);
            if (err != 0)
            {
                cerr << "Thread creation failed: " << strerror(err) << endl;
                delete params;
                throw runtime_error("Critical error");
            }
        }

        for (int i = 0; i < NUM_THREADS; ++i)
        {
            pthread_join(threads[i], NULL);
        }
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    cout << "\nAll " << NUM_THREADS << " threads completed successfully!" << endl;
    return EXIT_SUCCESS;
}