#pragma once
#include <vector>
#include <fstream>

namespace todo {
    using vstr = std::vector<std::string>; // deque<string> alias
    
    enum class Ret_codes : int {
        Success = 0,
        Inv_arg, // invalid arguments given
        // Inv_sec, // invalid section name
    };

    typedef struct DisplayMode { // Struct to select either default view or section tasks view
        bool disp_sec;
        std::string section;
    } DisplayMode;

    // File access and modification methods

#define FNAME "tasks.txt" // name of the file that stores the tasks
    
    std::fstream open_file(); // Opens an existing file; if file DNE asks to create new file
    vstr copy_tasks(std::fstream &fr); // Copies tasks from file to a buffer array and returns it
    Ret_codes save_file (vstr tasks); // Overwrites contents of existing file with current contents of buffer

    // Miscellaneous methods

    Ret_codes clear(); // Clears console window
    Ret_codes prompt(); // Displays prompt "cmd>" during interactive loop
    std::string bold(std::string text); // Prefixes the given text with the ANSI code for bold text and resets it at the end, returns a copy
    std::string italicize(std::string text); // Prefixes the given text with the ANSI code for italicized text and resets it at the end, returns a copy
    std::string title(std::string text); // Returns a copy of the given text in bold enclosed in the ANSI block character \u2591
    Ret_codes strike_through(std::string &text); // Prefixes the given text with the ANSI code for striked-through text and resets it at the end
    
    // Display methods

    Ret_codes print_array(vstr array, std::string style); // Prints strings from a vector as a numbered / bulleted list
    Ret_codes print_tasks(std::string section, vstr tasks); // Prints the tasks in a given section as a numbered list
    Ret_codes print_history(vstr history); // Prints inputs stored in the history buffer as a bulleted list
    Ret_codes print_help(); // Prints help page
    Ret_codes display_main(vstr tasks); // Displays section names along with general tasks stored in the buffer as a numbered list
    Ret_codes display_section(std::string section, vstr tasks); // Displays tasks under given section

    // Input handling methods

    bool OnlyAlpha(std::string str); // Returns true if string consists only of alphabets
    bool OnlyDigit(std::string str); // Returns true if string consists only of digits    
    Ret_codes InvalidCmd(); // Interactive method that prompts user if command is invalid
    Ret_codes InvalidArgs(); // Interactive method that prompts user if argument(s) is(are) invalid
    std::string get_input(); // Gets string input from user and returns it
    int SectionStart(std::string section, vstr tasks); // Finds index of first task in a given section
    int SectionSize(std::string section, vstr tasks); // Finds number of tasks in a given section
    Ret_codes add(std::string &input, vstr &tasks, DisplayMode &d); // Adds task
    Ret_codes remove(std::string &input, vstr &tasks, vstr &history, DisplayMode &d); // Removes task(s)
    Ret_codes handle_input(std::string &input, vstr &tasks, vstr &history, DisplayMode &d); // Parses user input

}