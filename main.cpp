/*
    A standard main.
*/
#include "CLI.h"

using namespace std;

int main(){

    CLI::get_instance().core_loop();
    
    return 0;
}