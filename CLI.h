/*
    A singleton to implement the command line interface.
    I've made this as a singleton because I only want 1 command line interface object.
*/

#include <string>
#include <vector>
#include <unordered_map>
#include "analyzer.h"

// An enumerator to help handle commands in token_parser().
enum class COMMAND {
    HELP,
    EXPLAIN,
    SHOW_SETTINGS,
    SET_ALG,
    SET_THREADS,
    RUN,
    QUIT
};

class CLI {
public:
    void core_loop();

    // give access to the one instance of this class
    static CLI& get_instance();

    // This is a singleton, so we want to disable copying
    CLI(CLI const&)             = delete;
    void operator=(CLI const&)  = delete;
private:
    CLI(); // make constructor inaccessible to avoid multiple instances
    std::vector<std::string> create_tokens(const std::string& input) ;  // Lexical analyzer
    void parse_tokens(const std::vector<std::string>& tokens);         // Token parser
    void command_help();
    void command_explain(const std::string& input);
    void command_show_settings();
    void command_set_alg(const std::string& input);
    void command_set_threads(const std::string& input);
    void command_run();
    void command_quit();
    void print_generic_error();
    void print_set_threads_error();

    // keeps core loop running until quit is called
    bool loop_is_running;

    // A map to convert from strings to enumerated commands. 
    // Allows commands to be handled with a switch statement in token_parser().
    static const std::unordered_map<std::string, COMMAND> STRING_TO_COMMAND;

    static Analyzer analyzer;
};