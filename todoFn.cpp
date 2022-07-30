#include "todoFn.h"
#include <iostream>
#include <algorithm>

namespace todo {
    
    // File access and modification methods
    std::fstream open_file() {
        std::fstream fr;
        fr.open(FNAME, std::fstream::in);
        if (!fr) {
            std::cout << "\"" << FNAME << "\" not found, do you want to create a new blank file? Press ENTER to continue or [Ctrl + C] to cancel..";
            getchar();
            std::fstream fr;
            fr.open(FNAME, std::fstream::app);
        }
        fr.close();
        return fr;
    }

    vstr copy_tasks(std::fstream &fr) {
        vstr tasks;
        std::string temp;
        fr.open(FNAME, std::fstream::in);
        while (true) {
            std::getline(fr, temp);
            if (fr.eof()) break;
            tasks.push_back(temp);
        }
        fr.close();
        return tasks;
    }

    Ret_codes save_file (vstr tasks) {
        
        // Remove empty sections
        std::fstream fw;
        fw.open(FNAME, std::fstream::out);
        for (int i = 0; i < (int) tasks.size(); i++) fw << tasks[i] << std::endl;
        fw.close();
        return Ret_codes::Success;
    }

    // Miscellaneous methods
    Ret_codes clear() {
        std::cout << "\033[2J\033[1;1H";
        return Ret_codes::Success;
    }

    Ret_codes prompt() {
        std::cout << "\ncmd> ";
        return Ret_codes::Success;
    }

    std::string bold(std::string text) {
        text = "\e[1m" + text + "\e[0m";
        return text;
    }
    
    std::string italicize(std::string text) {
        text = "\e[3m" + text + "\e[0m";
        return text;
    }

    std::string title(std::string text) {
        text = "\u2591 " + bold(text) + " \u2591";
        return text;
    }
    
    Ret_codes strike_through(std::string &text) {
        text = "\e[9m" + text + "\e[0m";
        return Ret_codes::Success;
    }

    vstr split(std::string input) {
        vstr results;
        int startIdx = 0;
        for (int i = 0; i < (int) input.length(); i++) {
            if (input[i] == ' ') {
                results.push_back(input.substr(startIdx, i - startIdx));
                startIdx = i + 1;
            }
            else if (i == (int) input.length() - 1) results.push_back(input.substr(startIdx, i - startIdx + 1));
        }
        return results;
    }

    // Display methods
    Ret_codes print_array(vstr array, std::string style) {
        if (style == "bullets") for (std::string str : array) std::cout << "- " << str << std::endl;
        else if (style == "numbers") for (int i = 0; i < (int) array.size(); i++) std::cout << i + 1 << ". " << array[i] << std::endl;
        else return Ret_codes::Inv_arg; // only bullets and numbers allowed for now
        return Ret_codes::Success;
    }

    Ret_codes print_tasks(std::string section, vstr tasks) {
        int section_start = SectionStart(section, tasks);
        if (section_start == -1) std::cout << "Nothing much to do in " << section << "..\n";
        else {
            int section_size = SectionSize(section, tasks);
            for (int i = section_start; i < section_start + section_size; i++) std::cout << i - section_start + 1 << ". " << tasks[i] << std::endl;
        }
        return Ret_codes::Success;
    }

    Ret_codes print_history(vstr history) {
        clear();
        std::cout << title("History") << "\n\n";
        if (history.size() == 0) std::cout << "No history yet.\n";
        else print_array(history, "bullets");
        std::cout << "\nPress ENTER to go back..";
        getchar ();
        return Ret_codes::Success;
    }

    Ret_codes print_help() {
        clear();
        std::cout << title("Help") << "\n\n";
        std::cout << "- Type " << italicize("add <task>") << " to add a general task.\n";
        std::cout << "- Type " << italicize("<section name> add <task>") << " to add a task to a particular section.\n";
        std::cout << "- Type " << italicize("remove <task number>") << " to remove a task from the section currently displayed.\n";
        std::cout << "- Type " << italicize("remove <section name>") << " to remove an entire section of tasks from the list.\n";
        std::cout << "- Type " << italicize("<section name> remove <task number>") << " to remove a section task from the list.\n";
        std::cout << "- Type " << italicize("<section name>") << " to view the tasks of a particular section.\n";
        std::cout << "- Type " << italicize("cp <src section name> <dest section name>") << " to copy all tasks of the src section and append it to the dest section.\n";
        std::cout << "- Type " << italicize("history") << " to view commands used in current session.\n";
        std::cout << "- Type " << italicize("exit") << " to save and exit.\n";
        std::cout << "\nEnd of help.\nPress ENTER to go back..";
        getchar ();
        return Ret_codes::Success;
    }

    Ret_codes display_main(vstr tasks) {
        clear();

        // Find number of sections present
        int count = 0;
        for (int i = 0; i < (int) tasks.size(); i++) {
            if (tasks[i][0] == ';' && tasks[i] != ";general") count++;
        }
        
        // Print section names (if present)
        for (int i = 0; i < (int) tasks.size(); i++) {
            if ((tasks[i][0] == ';') && (tasks[i] != ";general")) {
                std::string section = tasks[i].substr(1);
                if (count == 1) std::cout << "\u2514\u2500 ";
                else std::cout << "\u251C\u2500 ";
                std::cout << bold(section) << std::endl;
                i += SectionSize(section, tasks);
                count--;
            }
        }
        std::cout << std::endl;

        // Print general tasks
        std::cout << title("To do") << "\n\n";
        print_tasks("general", tasks);
        return Ret_codes::Success;
    }

    Ret_codes display_section(std::string section, vstr tasks) {
        clear();
        std::cout << title(section) << "\n\n";
        print_tasks(section, tasks);
        std::cout << "\n(Press ENTER to return to general tasks view)\n";
        return Ret_codes::Success;
    }

    // Input handling methods
    bool OnlyAlpha(std::string str) {
        std::locale loc;
        for (std::string::iterator it = str.begin(); it != str.end(); it++) {
            if (!std::isalpha(*it, loc)) return false;
        }
        return true;
    }

    bool OnlyDigit(std::string str) {
        std::locale loc;
        for (std::string::iterator it = str.begin(); it != str.end(); it++) {
            if (!std::isdigit(*it, loc)) return false;
        }
        return true;
    }

    Ret_codes InvalidCmd() {
        std::cout << "\nInvalid Command! (Try \"help\" for help)\nPress ENTER to try again..";
        getchar();
        return Ret_codes::Success;
    }

    Ret_codes InvalidArgs() {
        std::cout << "\nPlease provide valid arguments! (Try \"help\" for help)\nPress ENTER to try again..";
        getchar();
        return Ret_codes::Success;
    }

    std::string get_input() {
        std::string input;
        std::getline(std::cin, input);
        return input;
    }
    
    int SectionStart(std::string section, vstr tasks) {
        section = ";" + section;
        vstr::iterator sp = std::find(tasks.begin(), tasks.end(), section);
        if (sp == tasks.end()) return -1;
        int section_start = sp - tasks.begin() + 1;
        return section_start;
    }

    int SectionSize(std::string section, vstr tasks) {
        int section_start = SectionStart(section, tasks);
        if (section_start == -1) return 0;
        int size = 0;
        for (int i = section_start; i < (int) tasks.size(); i++) {
            if (tasks[i][0] == ';') break;
            if ((int) tasks[i][0] != 27) size++;
        }
        return size;
    }

    Ret_codes add(std::string &input, vstr &tasks, DisplayMode &d) {
        std::string section, task;

        // Checking section to add to
        if (input.compare(0, 4, "add ") == 0) {
            if (d.disp_sec == true) section = d.section;
            else section = "general";
            task = input.substr(input.find("add ") + 4);
        }
        else if (input.find(" add ") != std::string::npos) {
            section = input.substr(0, input.find(" add "));
            task = input.substr(input.find(" add ") + 5);
            d.section = section;
        }

        // If task is blank
        if (task == "\0") {
            InvalidArgs();
            return Ret_codes::Inv_arg;
        }

        // If section name has space(s) in it
        if (section.find(' ') != std::string::npos) {
            InvalidArgs();
            return Ret_codes::Inv_arg;
        }

        // Finding position to insert at
        int section_start = SectionStart(section, tasks);
        if (section_start == -1) { // Section DNE
            if (section.compare("general") == 0) {
                tasks.push_back(";" + section);
                tasks.push_back(task);
            }
            else {
                std::cout << "Specified section does not exist. Would you like to create a new one? (y/N) ";
                char c = getchar();
                if (c == '\n' || c == 'n' || c == 'N') return Ret_codes::Success;
                else if (c == 'y' || c == 'Y') {
                    tasks.push_back(";" + section);
                    tasks.push_back(task);
                }
                else {
                    while (getchar() != '\n'); // Flushes stdin buffer
                    InvalidArgs();
                }
            }
            return Ret_codes::Success;
        }
        else {
            vstr::iterator sp = tasks.begin();
            std::advance(sp, section_start + SectionSize(section, tasks));
            tasks.insert(sp, task);
        }

        return Ret_codes::Success;
    }

    Ret_codes remove(std::string &input, vstr &tasks, vstr &history, DisplayMode &d) {
        std::string section, temp;
        int task_no = -1;

        // Checking which task(s) to remove
        if (input.compare(0, 7, "remove ") == 0) {
            temp = input.substr(input.find("remove ") + 7);
            
            // Checking for section or task_no
            if (temp == "\0") {
                InvalidArgs();
                return Ret_codes::Inv_arg;
            }
            else if (OnlyDigit(temp)) {
                task_no = stoi(temp);
                if (d.disp_sec == true) section = d.section;
                else section = "general";
            }
            else if (OnlyAlpha(temp)) section = temp; // To remove a section completely
        }

        else if (input.find(" remove ") != std::string::npos) {
            section = input.substr(0, input.find(" remove "));
            temp = input.substr(input.find(" remove ") + 8);
            d.section = section;

            if (temp == "\0") {
                InvalidArgs();
                return Ret_codes::Inv_arg;
            }
            else if (OnlyDigit(temp)) task_no = stoi(temp);
            else {
                InvalidArgs();
                return Ret_codes::Inv_arg;
            }
        }

        // If section name has space(s) in it
        if (section.find(' ') != std::string::npos) {
            InvalidArgs();
            return Ret_codes::Inv_arg;
        }

        // Check if section exists
        if (SectionStart(section, tasks) == -1) {
            InvalidArgs();
            return Ret_codes::Inv_arg;
        }

        // Find section_size and section_start
        int section_size = SectionSize(section, tasks), section_start = SectionStart(section, tasks);

        // Remove the task(s)
        if (task_no == -1) { // Remove entire section
            vstr::iterator start = tasks.begin();
            advance(start, section_start - 1); // Points to section title
            vstr::iterator end = start;
            advance(end, section_size + 1); // Points to last task in the section

            // Place removed tasks in history
            vstr::iterator it = start;
            for (advance(it, 1); it != end; advance(it, 1)) history.push_back("removed (" + *it + ")");

            // Confirm removal
            std::cout << "Do you really want to remove this section along with all its tasks? (y/N) ";
            char c = getchar();
            if (c == '\n' || c == 'n' || c == 'N') return Ret_codes::Success;
            else if (c == 'y' || c == 'Y') {
                tasks.erase(start, end);
                d.disp_sec = false;
            }
            else {
                while (getchar() != '\n');
                InvalidArgs();
            }
        }
        else { // Remove single task
            if ((task_no > section_size) || (task_no <= 0)) InvalidArgs();
            else {
                vstr::iterator tp = tasks.begin();
                std::advance(tp, section_start + task_no - 1);
                history.erase(history.end());
                input = input + " (" + tasks[section_start + task_no - 1] + ")";
                history.push_back(input);
                tasks.erase(tp);
            }
            if (SectionSize(section, tasks) == 0) { // Remove section tag if section is empty
                vstr::iterator sp = tasks.begin();
                std::advance(sp, section_start - 1);
                tasks.erase(sp);
                d.disp_sec = false;
            }
        }
        
        return Ret_codes::Success;
    }

    Ret_codes copy_section(std::string input, vstr &tasks) {
        vstr split_input = split(input); // Split input string to get arguments
        if (split_input.size() != 3 || SectionStart(split_input[1], tasks) == -1) {
            InvalidArgs();
            return Ret_codes::Inv_arg;
        }

        std::string src = split_input[1], dest = split_input[2];
        if (SectionStart(dest, tasks) == -1) { // If destination section doesn't exist, create
            std::cout << "Specified section to copy to does not exist. Would you like to create a new one? (y/N) ";
            char c = getchar();
            if (c == '\n' || c == 'n' || c == 'N') return Ret_codes::Success;
            else if (c == 'y' || c == 'Y') tasks.push_back(";" + dest);
            else {
                while (getchar() != '\n');
                InvalidArgs();
                return Ret_codes::Inv_arg;
            }
        }

        // Copy tasks from src
        vstr tasks_copy;
        int src_start = SectionStart(src, tasks), src_size = SectionSize(src, tasks);
        for (int i = src_start; i < src_start + src_size; i++) tasks_copy.push_back(tasks[i]);

        // Insert tasks into destination
        int dest_start = SectionStart(dest, tasks);
        vstr::iterator sp = tasks.begin();
        std::advance(sp, dest_start + SectionSize(dest, tasks));
        for (std::string task : tasks_copy) {
            tasks.insert(sp, task);
            sp = tasks.begin(); // Reset iterator and find next position to insert to
            std::advance(sp, dest_start + SectionSize(dest, tasks));
        }

        return Ret_codes::Success;
    }
    
    Ret_codes handle_input(std::string &input, vstr &tasks, vstr &history, DisplayMode &d) {
        // Add relevant user inputs to history
        if (input != "history" && input != "\0" && input != "help") history.push_back(input);
        
        // Add tasks
        if (input.compare(0, 4, "add ") == 0 || input.find(" add ") != std::string::npos) add(input, tasks, d);

        // Remove tasks
        else if (input.compare(0, 7, "remove ") == 0 || input.find(" remove ") != std::string::npos) remove(input, tasks, history, d);

        // Copy a particular tasks section to another section
        else if (input.compare(0, 3, "cp ") == 0) copy_section(input, tasks);

        // Miscellaneous
        else if (input == "help") print_help();
        else if (input == "history") print_history(history);
        else if (input == "\0") {
            d.disp_sec = false;
            display_main(tasks);
        }
        
        // Display section tasks
        else if (OnlyAlpha(input)) { // Section name is given as input
            if (SectionStart(input, tasks) == -1) InvalidCmd();
            else {
                d.disp_sec = true;
                d.section = input;
                display_section(d.section, tasks);
            }
        }

        else InvalidCmd();
        return Ret_codes::Success;
    }
}