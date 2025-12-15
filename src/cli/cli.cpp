#include "cli.h"
#include <iostream>

namespace
{
	// For Colored printing
	constexpr const char* RESET = "\033[0m";
	constexpr const char* BLUE = "\033[94m";
	constexpr const char* GREEN = "\033[92m";
	constexpr const char* GREY = "\033[2m";
	constexpr const char* RED = "\033[91m";
	constexpr const char* YELLOW = "\033[93m";

	void printError(const std::string& message)
	{
		std::cerr << RED << "Error: " << RESET << message << "\n";
	}

	void printSeperator()
	{
		std::cout << GREY << "---------------------------------\n" << RESET;
	}
}

CLI::CLI(const std::string& app_name)
	: app_name_(app_name) {
}

void CLI::registerModule(const ModuleDef& module)
{
	modules_[module.name] = module;
}

const ModuleDef* CLI::findModule(const std::string& name) const
{
	auto it = modules_.find(name);
	if (it == modules_.end())
		return nullptr;
	return &it->second;
}

const CommandDef* CLI::findCommand(
	const ModuleDef& module,
	const std::string& name) const
{
	auto it = module.commands.find(name);
	if (it == module.commands.end())
		return nullptr;
	return &it->second;
}

int CLI::run(int argc, char** argv)
{
	if (argc < 2) {
		printError("No arguments provided.");
		std::cout << "Use '" << app_name_ << " help' for usage information.\n";
		return 1;
	}

	if (std::string(argv[1]) == "help") {
		if (argc == 2) {
			showHelp();
			return 0;
		}
		else if (argc == 3) {
			auto mod = findModule(argv[2]);
			if (!mod) {
				printError("Module '" + std::string(argv[2]) + "' not found.");
				return 1;
			}
			showModuleHelp(*mod);
			return 0;
		}
	}

	if (argc < 3) {
		printError("Both module and command must be specified.");
		std::cout << "Usage: " << app_name_ << " <module> <command> [args...]\n";
		return 1;
	}

	std::string mod_name = argv[1];
	std::string cmd_name = argv[2];

	const ModuleDef* module = findModule(mod_name);
	if (!module) {
		printError("Module '" + mod_name + "' not found.");
		std::cout << "Use '" << app_name_ << " help' to see available modules.\n";
		return 1;
	}

	const CommandDef* command = findCommand(*module, cmd_name);
	if (!command) {
		printError("Command '" + cmd_name + "' not found in module '" + mod_name + "'.");
		std::cout << "Use '" << app_name_ << " help " << mod_name << "' to see available commands.\n";
		return 1;
	}

	if (!command->run) {
		printError("Command '" + cmd_name + "' has no implementation.");
		return 1;
	}

	Args args;
	for (int i = 3; i < argc; ++i) {
		args.positional.push_back(argv[i]);
	}

	return command->run(args);
}

void CLI::showHelp() const
{
	std::cout << app_name_ << "\n";
	std::cout << "Usage: " << app_name_ << " <module> <command> [args...]\n";
	std::cout << "Modules:\n";
	for (const auto& mod : modules_) {
		showModuleHelp(mod.second);
	}
}

void CLI::showModuleHelp(const ModuleDef& module) const
{
	std::cout << BLUE << "# " << module.name << RESET << "\n"
		<< "  " << GREY << module.description << "\n";
	std::cout << "  Commands:\n" << RESET;
	for (const auto& cmd : module.commands) {
		std::cout << "    "
			<< GREEN << " " << cmd.first << RESET
			<< "\t:  " << cmd.second.description
			<< "\n";
	}
	printSeperator();
}