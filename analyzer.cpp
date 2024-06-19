/*
    TODO: 
        Implement multithreading alg 1
            Speed up alg
            Add support for running on less than 4 threads
        Implement multithreading alg 2
        Implement multithreading alg 3
*/

#include "analyzer.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <thread>
#include <vector>
#include <functional>
#include <utility>
#include <mutex>

using namespace std;

namespace filesys = std::filesystem;



// Init class consts
const filesys::path Analyzer::book_dir_path = filesys::current_path() / "Books";

const std::unordered_map<std::string, ALG> Analyzer::STRING_TO_ALG = {
    { "single_thread", ALG::SINGLE_THREAD },
    { "multi_thread_1", ALG::MULTI_THREAD_1 },
    { "multi_thread_2", ALG::MULTI_THREAD_2 },
    { "multi_thread_3", ALG::MULTI_THREAD_3 }
};

const std::unordered_map<ALG, std::string> Analyzer::ALG_TO_STRING  = {
    { ALG::SINGLE_THREAD, "single_thread" },
    { ALG::MULTI_THREAD_1, "multi_thread_1" },
    { ALG::MULTI_THREAD_2, "multi_thread_2" },
    { ALG::MULTI_THREAD_3, "multi_thread_3" }
};



Analyzer::Analyzer() {
    num_threads = 2;
    algorithm = ALG::SINGLE_THREAD;
}

AnalyzerData* Analyzer::alg_single_thread() {
    AnalyzerData* data = new AnalyzerData();
    unordered_map<string, int> word_frequencies;
    word_frequencies.reserve(109000);   // 108917 unique words, so we reserve buckets in advance

    // Get raw data per file
    for (const auto& book_path_it : filesys::directory_iterator(book_dir_path)) {
        ifstream book(string(book_path_it.path()), std::ios::in);
        if(!book) {
            //cout << "\nERROR: file at " << book_path_it.path() << " could not be read.\n\n";
            continue;
        }

        //cout << "\nProcessing file at " << book_path_it.path() << ", please wait...";

        // Actual data retrieval occurs here; rest is file management
        string line;
        while(getline(book, line)) {
            string current_word = "";
            for(char ch : line) {
                if (!isalpha(ch)) {
                    if (!current_word.empty()) {
                        data->average_word_length += current_word.length();   // division is performed once after all files have been read
                        ++data->total_words;
                        if (data->longest_word.length() < current_word.length()) {
                            data->longest_word = current_word;
                        }

                        // New elements will be inserted into the map with a value of zero, so this works
                        ++word_frequencies[current_word];

                        current_word = "";
                    }
                }
                else {
                    current_word += ch;
                }
            }
        }
    }

    // Process data
    data->average_word_length /= double(data->total_words);
    auto most_common_word_it = max_element(word_frequencies.begin(), word_frequencies.end(), [](const pair<string, int>& a, const pair<string, int>& b) { return a.second < b.second; });
    if(most_common_word_it != word_frequencies.end()) {
        data->most_common_word = most_common_word_it->first;
        data->most_common_word_occurences = most_common_word_it->second;
    }
    data->num_unique_words = word_frequencies.size();

    return data;
}

// Each thread runs a different test on each file
AnalyzerData* Analyzer::alg_multi_thread_1() {
    AnalyzerData* data = new AnalyzerData();
    unordered_map<string, int> word_frequencies;
    word_frequencies.reserve(109000);   // 108917 unique words, so we reserve buckets in advance
    
    // create threads
    vector<thread> threads(num_threads - 1);

    for(int i = 0; i < min(num_threads - 1, 3); ++i) {
        // create new thread
        TASK t = TASK(i);
        threads[i] = thread(&Analyzer::alg_mt1_thread, this, t, data, ref(word_frequencies));
    }

    // this thread will work on getting the average word length
    alg_mt1_thread(TASK::AVG_WORD_LENGTH, data, ref(word_frequencies));

    // join threads
    std::for_each(threads.begin(), threads.end(), mem_fn(&thread::join));

    // Process data
    data->average_word_length /= double(data->total_words);
    auto most_common_word_it = max_element(word_frequencies.begin(), word_frequencies.end(), [](const pair<string, int>& a, const pair<string, int>& b) { return a.second < b.second; });
    if(most_common_word_it != word_frequencies.end()) {
        data->most_common_word = most_common_word_it->first;
        data->most_common_word_occurences = most_common_word_it->second;
    }
    data->num_unique_words = word_frequencies.size();

    return data;
}

void Analyzer::alg_mt1_thread(TASK task, AnalyzerData* out_data, std::unordered_map<std::string, int>& word_frequencies) {
    //cout << "\nThread started.\n";
    for (const auto& book_path_it : filesys::directory_iterator(book_dir_path)) {
        ifstream book(string(book_path_it.path()), std::ios::in);
        if(!book) {
            //cout << "\nERROR: file could not be read.\n\n";
            continue;
        }

        //cout << "\nProcessing file, please wait...";

        // Actual data retrieval occurs here; rest is file management
        string line;
        while(getline(book, line)) {
            string current_word = "";
            for(char ch : line) {
                if (!isalpha(ch)) {
                    if (!current_word.empty()) {
                        switch(task) {
                            case(TASK::AVG_WORD_LENGTH):
                                out_data->average_word_length += current_word.length();   // division is performed once after all files have been read
                            break;
                            case(TASK::FREQUENCY):
                                // New elements will be inserted into the map with a value of zero, so this works
                                ++word_frequencies[current_word];
                            break;
                            case(TASK::LONGEST_WORD):
                                if (out_data->longest_word.length() < current_word.length()) {
                                    out_data->longest_word = current_word;
                                }   
                            break;
                            case(TASK::TOTAL_WORDS):
                                ++out_data->total_words;
                            break;
                        }

                        current_word = "";
                    }
                }
                else {
                    current_word += ch;
                }
            }
        }
    }

    //cout << "\nThread finished.\n";
}

// Wrapper for running correct alg function and determining processing time
const AnalyzerData* Analyzer::run_analysis() {
    AnalyzerData* data = nullptr;

    const auto start = chrono::steady_clock::now();

    // Use user-specified algorithm
    switch(algorithm) {
        case(ALG::SINGLE_THREAD):
            data = alg_single_thread();
        break;
        case(ALG::MULTI_THREAD_1):
            data = alg_multi_thread_1();
        break;
        default:
            data = alg_single_thread();
        break;
    }

    // Calculate time spent running algorithm in seconds
    const auto end = chrono::steady_clock::now();
    const auto elapsed = chrono::duration_cast<chrono::duration<double>>(end - start);
    data->processing_time = elapsed.count();

    return data;
}

const std::string Analyzer::get_alg() {
    return ALG_TO_STRING.at(algorithm);
}

// returns true if assignment was successful
bool Analyzer::set_alg(const std::string& alg_str) {
    if (STRING_TO_ALG.find(alg_str) != STRING_TO_ALG.end()) {
        algorithm = STRING_TO_ALG.at(alg_str);
        return true;
    }

    return false;
}

int Analyzer::get_threads() {
    return num_threads;
}

// returns true if assignment was successful
bool Analyzer::set_threads(int desired_threads) {
    int hardware_limit = max(thread::hardware_concurrency(), 2u);
    num_threads = clamp(desired_threads, 2, hardware_limit);
    return num_threads == desired_threads;
}