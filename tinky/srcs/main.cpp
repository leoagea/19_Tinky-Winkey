#include "tinky.h"

int main(int argc, char* argv[])
{
	// Convert arguments to strings for easier handling
	std::vector<std::string> args;
	for (int i = 1; i < argc; ++i) {
		args.push_back(argv[i]);
	}

	if (args.empty()) {
		SERVICE_TABLE_ENTRY serviceTable[] = {
			{SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
			{NULL, NULL}
		};

		if (!StartServiceCtrlDispatcher(serviceTable)) {
			DWORD error = GetLastError();
			if (error == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
				// We're not being started by the SCM, show usage instead
				std::cout << "This executable must be started by the Service Control Manager or with command arguments." << std::endl;
				std::cout << "Usage: svc.exe <command>" << std::endl;
				std::cout << "Commands: install, start, stop, delete" << std::endl;
				return 1;
			} else {
				std::cout << "StartServiceCtrlDispatcher failed: " << GetErrorMessage() << std::endl;
				return 1;
			}
		}
		return 0;
	}

	// Use safer string handling to avoid Spectre warnings
	std::unordered_map<std::string, std::function<void()>> commands = {
		{"install", Install},
		{"start", Start},
		{"stop", Stop},
		{"delete", Delete}
	};

	// Use the first argument as the command
	std::string command_str = args[0];
	
	if (auto it = commands.find(command_str); it != commands.end()) {
		it->second();
	} else {
		std::cout << "Unknown command: " << command_str << std::endl;
		std::cout << "Valid commands: install, start, stop, delete" << std::endl;
	}

	return 0;
}