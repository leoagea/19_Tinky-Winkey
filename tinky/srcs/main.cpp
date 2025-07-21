#include "tinky.h"

int main(int ac, char *av[])
{
    if (ac < 2) {
        std::cout << "Usage: " << av[0] << " <command>" << std::endl;
        std::cout << "Commands: install, start, stop, delete" << std::endl;
        return 1;
    }

    Tinky tinky;

    std::unordered_map<std::string_view, std::function<void()>> commands = {
        {"install", [&tinky]() { tinky.Install(); }},
        {"start",   [&tinky]() { tinky.Start(); }},
        {"stop",    [&tinky]() { tinky.Stop(); }},
        {"delete",  [&tinky]() { tinky.Delete(); }},
    };

    auto command = commands.find(av[1]);
    if (command != commands.end()) {
        command->second();
    } else {
        std::cout << "Unknown command: " << av[1] << std::endl;
        return 1;
    }

    return 0;
}