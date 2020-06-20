#include <iostream>
#include <cstring>
#include <fstream>
#include <chrono>
#include <vector>
#include <dirent.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <omp.h>

using namespace std;

vector<string> lsRec(char *basePath)
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);
    vector<string> files;

    if (!dir) return files;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);

            if (!opendir(path)) {
                files.push_back(path);
            } else {
                vector<string> res = lsRec(path);
                files.insert(files.end(), res.begin(), res.end());
            }
        }
    }

    closedir(dir);
    return files;
}

bool stringmatch(string &text, string &pattern, int &start, int &end)
{
    int len = pattern.length();
    for (int i = start; i < end; i++) {
        bool t = true;
        for (int j = 0; j < len; j++) {
            t = text[i+j] == pattern[j];
            if (!t) break;
        }
        if (t) return t;
    }
    return false;
}

int subprocess(string filename, string pattern, int thread_counts) {
    string text;
    string line;
    ifstream file (filename);
    if (file.is_open())
    {
        while (getline(file,line))
        {
            text += line + '\n';
        }
        file.close();
    }
    else cout << "Unable to open file";
    int string_c_length = text.length()-pattern.length()+1;
    int inc = string_c_length / thread_counts;
    bool found = false;
    #pragma omp parallel for num_threads(thread_counts)
    for (int i = 0; i < thread_counts; i++)
    {
        int begin = i*inc;
        int last = begin+inc;
        if (i == thread_counts-1) {
            last = string_c_length;
        }
        bool temp = stringmatch(text, pattern, begin, last);
        #pragma omp critical
        {
            found = temp || (found);
        }
    }
    if (found) cout << filename << endl;
    return found;
}

int main(int argc, char *argv[])
{
    if (argc == 4) {
        int process_counts = stoi(argv[1]);
        int thread_counts = stoi(argv[2]);
        if (process_counts <= 0 || thread_counts <= 0) {
            cout << "Bad process/thread counts" << endl;
            return 0;            
        }

        int process_actives = 0;
        int file_found = 0;
        string pattern = argv[3];
        vector<string> files = lsRec(".");
        
        cout << "Starting string finding..." << endl;
        cout << "List of files containing the pattern will be print out below" << endl;
        chrono::time_point<chrono::system_clock> start, end;
        start = chrono::system_clock::now();
        for(string& path: files) {
            // Keep up the process counts, wait for another to finish to make new child process
            if (process_actives >= process_counts) {
                int status;
                wait(&status);
                // If exit status == 1, pattern is found in that file
                if (WEXITSTATUS(status) == 1) {
                    file_found++;
                }
                process_actives--;
            }
            pid_t id = fork();
            // Child process will do the file check
            if (id == 0) {
                return subprocess(path, pattern, thread_counts);
            }
            process_actives++;
        }
        // Make sure all processes finished and print the final result
        while (process_actives) {
            int status;
            wait(&status);
            // If exit status == 1, pattern is found in that file
            if (WEXITSTATUS(status) == 1) {
                file_found++;
            }
            process_actives--;
        }
        end = chrono::system_clock::now();
        chrono::duration<double> elapsed_seconds = end - start; 

        cout << endl << "Elapsed time: " << elapsed_seconds.count() << "s" << endl;
        cout << "Found '" << pattern << "' in " << file_found << " file(s)" << endl;
    }
    return 0;
}