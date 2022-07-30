#include "todoFn.h"

using namespace todo;

int main(int argc, char const *argv[]) {

    // Store tasks from file in array
    std::fstream fr = open_file();
    vstr tasks = copy_tasks(fr);

    // Start interactive loop
    DisplayMode d = { .disp_sec = false, .section = "general" }; // Display-status variable (show default / section tasks)
    vstr history;
    std::string input;

    while (true) {
        if (d.disp_sec == true) display_section(d.section, tasks); // Display section tasks
        else display_main(tasks); // Display current tasks (default)
        prompt(); // Display prompt for user input
        input = get_input(); // Store user input

        // Input analysis
        if (input.compare("exit") == 0) break; // Exit loop
        handle_input(input, tasks, history, d); // Parse command from user
    }

    // Save edited tasks to file
    save_file(tasks);

    return 0;
}
