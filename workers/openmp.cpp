#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <omp.h>

using namespace std;

bool stringmatch(string &text, string &pattern, int &start, int &end)
{
    for (int i = start; i < end; i++) {
        if (!text.compare(i, pattern.length(), pattern))
            return true;
    }
    return false;
}

int main(int argc, char *argv[])
{
    if (argc == 4) {
        int thread_counts = stoi(argv[1]);
        string pattern = argv[2];
        string text;
        string line;
        ifstream file (argv[3]);
        if (file.is_open())
        {
            while (getline(file,line))
            {
                text += line + '\n';
            }
            file.close();
        }
        else cout << "Unable to open file";
        omp_set_num_threads(thread_counts);
        bool found = false;
        #pragma omp parallel for num_threads(thread_counts) shared(text)
        for (int i = 0; i < thread_counts; i++)
        {
            int string_c_length = text.length()-pattern.length()+1;
            int inc = string_c_length / thread_counts;
            //cout << argv[3] << endl;
            int begin = i*inc;
            int last = begin+inc;
            if (i == thread_counts-1) {
                last = string_c_length;
            }
            found = (stringmatch(text, pattern, begin, last)) || (found);
        }
        return found;
    }
    return 0;
}