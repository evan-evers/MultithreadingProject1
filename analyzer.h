/*
    A class for running the program's analysis algorithms and storing the resulting data.
*/

#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <mutex>

// Keeps track of which algorithm is being used.
enum class ALG {
    SINGLE_THREAD,
    MULTI_THREAD_1,
    MULTI_THREAD_2,
    MULTI_THREAD_3
};

// Different tasks a thread in the multithreading alg can take on.
enum class TASK {
    TOTAL_WORDS = 0,
    FREQUENCY = 1,
    LONGEST_WORD = 2,
    AVG_WORD_LENGTH = 3
};

// For returning data to the CLI.
struct AnalyzerData {
    double average_word_length; // Words are defined as non-whitespace characters w/ whitespace chars on either side
    double processing_time;
    int total_words;
    int num_unique_words;
    int most_common_word_occurences;
    std::string most_common_word;
    std::string longest_word;
    AnalyzerData() {
        average_word_length = 0;
        processing_time = 0;
        total_words = 0;
        longest_word = "";
        most_common_word = "N/A";
        most_common_word_occurences = 0;
    }
};

class Analyzer {
public:
    Analyzer();
    const AnalyzerData* run_analysis();
    const std::string get_alg();
    bool set_alg(const std::string&);
    int get_threads();
    bool set_threads(int);
private:
    AnalyzerData* alg_single_thread();
    AnalyzerData* alg_multi_thread_1();
    void alg_mt1_thread(TASK, AnalyzerData*, std::unordered_map<std::string, int>&);
    // static void alg_mt1_thread(AnalyzerData*, std::vector<std::ifstream>, std::unordered_map<std::string, int>*, TASK);

    int num_threads;
    ALG algorithm;

    std::mutex data_mutex;

    static const std::filesystem::path book_dir_path;
    static const std::unordered_map<std::string, ALG> STRING_TO_ALG;
    static const std::unordered_map<ALG, std::string> ALG_TO_STRING;
};