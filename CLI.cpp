#include "CLI.h"

#include <iostream>
#include <string>
#include <vector>
#include <ctype.h>
#include <unordered_map>
#include <thread>

using namespace std;

/*
TODO: Fix seg fault on quit
*/

// Init class consts
const unordered_map<string, COMMAND> CLI::STRING_TO_COMMAND = {
    { "help", COMMAND::HELP },
    { "explain", COMMAND::EXPLAIN },
    { "show_settings", COMMAND::SHOW_SETTINGS },
    { "set_alg", COMMAND::SET_ALG },
    { "set_threads", COMMAND::SET_THREADS },
    { "run", COMMAND::RUN },
    { "quit", COMMAND::QUIT }
};

Analyzer CLI::analyzer;

CLI::CLI() {
    loop_is_running = true;
}

// access the one instance of this class
CLI& CLI::get_instance() {
    static CLI instance;
    return instance;
}

void CLI::core_loop() {
    cout <<
    "\nWelcome to the application!\n" <<
    "This application was authored by Evan Evers.\n" <<
    "\nPlease enter a command. To see a list of available commands, type -> help.\n\n";

    while(loop_is_running) {
        string input;
        cout << "-> ";
        getline(cin, input);
        vector<string> tokens = create_tokens(input);
        parse_tokens(tokens);
    }
}

// Lexical analyzer. Analyzes input text and returns a vector of tokens
vector<string> CLI::create_tokens(const string& input) {
    vector<string> tokens;
    string current_token = "";

    // Add tokens separated by whitespace
    for(char ch : input) {
        if(isspace(ch)) {
            if(!current_token.empty()) {
                tokens.push_back(current_token);
                current_token = "";
            }
        }
        else {
            current_token += ch;
        }
    }

    // Since the above code only adds tokens with a space after them, we need to add the last token as well, 
    // since it almost definitely won't have a space after it
    if (!current_token.empty()) {
        tokens.push_back(current_token);
    }

    return tokens;
}

// Token parser. Analyzes tokens. Runs valid commands, sends error messages if invalid commands are given
void CLI::parse_tokens(const vector<string>& tokens) {
    // Catch commands with invalid numbers of tokens
    if (tokens.size() > 2 || tokens.size() == 0) {
        print_generic_error();
        return;
    }

    // Deal with first token in command
    if (CLI::STRING_TO_COMMAND.find(tokens[0]) != CLI::STRING_TO_COMMAND.end()) {
        // using at() instead of [] b/c constant maps require it, as [] can insert new elements
        switch (CLI::STRING_TO_COMMAND.at(tokens[0])) {
            case(COMMAND::HELP):
                if (tokens.size() == 1) {
                    command_help();
                }
                else {
                    print_generic_error();
                }
            break;
            case(COMMAND::EXPLAIN):
                if (tokens.size() == 2) {
                    command_explain(tokens[1]);
                }
                else {
                    print_generic_error();
                }
            break;
            case(COMMAND::SHOW_SETTINGS):
                if (tokens.size() == 1) {
                    command_show_settings();
                }
                else {
                    print_generic_error();
                }
            break;
            case(COMMAND::SET_ALG):
                if (tokens.size() == 2) {
                    command_set_alg(tokens[1]);
                }
                else {
                    print_generic_error();
                }
            break;
            case(COMMAND::SET_THREADS):
                if (tokens.size() == 2) {
                    command_set_threads(tokens[1]);
                }
                else {
                    print_generic_error();
                }
            break;
            case(COMMAND::RUN):
                if (tokens.size() == 1) {
                    command_run();
                }
                else {
                    print_generic_error();
                }
            break;
            case(COMMAND::QUIT):
                if (tokens.size() == 1) {
                    command_quit();
                }
                else {
                    print_generic_error();
                }
            break;
            default:
                print_generic_error();
        }
    }
    else {
        // unrecognized token
        print_generic_error();
    }
}

// Generic help function
void CLI::command_help() {
    cout << 
    "\nThis is an application for calculating various statistics on Project Gutenberg's 10 most downloaded books.\n" <<
    
    "\nUSAGE:\n" <<
    "   -> [COMMAND]\n" <<

    "\nCOMMANDS:\n" <<
    "   help                                Displays help for the application.\n" <<
    "   explain [COMMAND]                   Explains how to use the specified command.\n" <<
    "   set_alg [ALGORITHM]                 Sets the algorithm to be used when the 'run' command is called.\n" <<
    "   set_threads [NUMBER_OF_THREADS]     Sets the number of threads for multithreaded algorithms to use.\n" <<
    "   run                                 Runs the algorithm and displays statistics.\n" <<
    "   quit                                Quits the application.\n\n";
}

// Explain the point of a command and how to use it
void CLI::command_explain(const string& input) {
    if (STRING_TO_COMMAND.find(input) != STRING_TO_COMMAND.end()) {
        switch (STRING_TO_COMMAND.at(input)) {
            case(COMMAND::HELP):
                cout <<
                "\nSYNTAX:\n" <<
                "   -> help\n" <<

                "\nDESCRIPTION:\n" <<
                "   The -> help command describes the application's purpose, how to use commands, and lists the application's commands.\n\n";
            break;
            case(COMMAND::EXPLAIN):
                cout <<
                "\nSYNTAX:\n" <<
                "   -> explain [COMMAND]\n"

                "\nVALID COMMANDS:\n" <<
                "   help\n" <<
                "   explain\n" <<
                "   set_alg\n" <<
                "   set_threads\n" <<
                "   run\n" <<
                "   quit\n" <<

                "\nDESCRIPTION:\n" <<
                "   The -> explain command describes the syntax and purpose of various commands.\n\n";
            break;
            case(COMMAND::SHOW_SETTINGS):
                cout <<
                "\nSYNTAX:\n" <<
                "   -> show_settings\n"

                "\nDESCRIPTION:\n" <<
                "   The -> show_settings command shows the settings that the application will use to analyze text.\n\n";
            break;
            case(COMMAND::SET_ALG):
                cout <<
                "\nSYNTAX:\n" <<
                "   -> set_alg [ALGORITHM]\n"

                "\nVALID ALGORITHMS:\n" <<
                "   single_thread\n" <<
                "   multi_thread_1\n" <<
                "   multi_thread_2\n" <<
                "   multi_thread_3\n" <<

                "\nDESCRIPTION:\n" <<
                "   The -> set_alg command sets the algorithm to be used when -> run is called.\n" <<
                "   Below is a brief explanation of how each algorithm works.\n" <<
                "       single_thread\n" <<
                "           This algorithm runs its tests using a single thread.\n" <<
                "           It is implemented as a baseline for testing the efficiency multithreaded algorithms.\n" <<
                "       multi_thread_1\n" <<
                "           This algorithm runs different tests on different threads.\n" <<
                "       multi_thread_2\n" <<
                "           This algorithm runs different files on different threads.\n" <<
                "       multi_thread_3\n" <<
                "           This algorithm runs different chunks of text, potentially from the same file, on different threads.\n\n";
            break;
            case(COMMAND::SET_THREADS):
                cout <<
                "\nSYNTAX:\n" <<
                "   -> set_threads [NUMBER_OF_THREADS]\n"

                "\nVALID NUMBERS OF THREADS:\n" <<
                "   1 and above \n" <<

                "\nDESCRIPTION:\n" <<
                "   The -> set_threads command sets how manys threads the multithreaded algorithms will use.\n" <<
                "   Multithreaded algorithms must use at least 2 threads.\n\n";
            break;
            case(COMMAND::RUN):
                cout <<
                "\nSYNTAX:\n" <<
                "   -> run\n" <<

                "\nDESCRIPTION:\n" <<
                "   Runs the application with the current algorithm and thread count.\n\n";
            break;
            case(COMMAND::QUIT):
                cout <<
                "\nSYNTAX:\n" <<
                "   -> quit\n" <<

                "\nDESCRIPTION:\n" <<
                "   Exits the application.\n\n";
            break;
            default:
                print_generic_error();
        }
    }
    else {
        // unrecognized token
        print_generic_error();
    }
}

// Show user the settings that analyzer will use
void CLI::command_show_settings() {
    cout << 
    "\nCurrent algorithm: " << analyzer.get_alg() << "\n"
    "Number of threads that will be used: " << analyzer.get_threads() << "\n\n";
}

// Set algorithm to use when application is ran
void CLI::command_set_alg(const string& input) {
    if (analyzer.set_alg(input)) {
        cout << "\n Algorithm set sucessfully.\n\n";
    }
    else {
        cout << 
        "\nThe algorithm could not be set with that parameter.\n" <<
        "Type -> explain set_alg   for a list of valid parameters. \n\n";
    }
}

// Set number of threads to use for multithreaded algorithms
void CLI::command_set_threads(const string& input) {
    int input_int = 0;
    try {
        input_int = stoi(input);
    }
    catch (...){
        print_set_threads_error();
        return;
    }

    if (analyzer.set_threads(input_int)) {
        cout << "\nThreads set sucessfully.\n\n";
    }
    else {
        print_set_threads_error();
    }
}

// Run the selected algorithm and display stats
void CLI::command_run() {
    const AnalyzerData* data = analyzer.run_analysis();

    cout << 
    "\nNumber of unique words: " << data->num_unique_words <<
    "\nMost common word: " << data->most_common_word <<
    "\nOccurences of most common word: " << data->most_common_word_occurences <<
    "\nLongest word: " << data->longest_word <<
    "\nAverage word length: " << data->average_word_length <<
    "\nTotal words: " << data->total_words;
    cout << "\n\nProcessing time: " << data->processing_time << " seconds \n\n";

    delete data;
    data = nullptr;
}

// Exit application
void CLI::command_quit() {
    loop_is_running = false;
}

void CLI::print_generic_error() {
    cout << 
    "\nERROR: invalid command supplied.\n" <<
    "To view a list of valid commands, type -> help\n" <<
    "To view the correct syntax for a specific command, type -> explain [COMMAND]\n\n";
}

// Tell user to give a value between 1 and whatever hardware_concurrency() thinks is the max
// If hardware concurrency has a problem, we tell the user to supply a value between 1 and 1
void CLI::print_set_threads_error() {
    cout << 
    "\nInvalid value supplied.\n" <<
    "Please supply a value in the range [2, " << (thread::hardware_concurrency() == 0u ? 2 : thread::hardware_concurrency()) << "].\n\n";
}