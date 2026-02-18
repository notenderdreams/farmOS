#include "cli.h"
#include "color.h"
#include <iostream>
#include <cctype>

namespace {
	std::vector<std::string> tokenizeInput(const std::string& line)
	{
		std::vector<std::string> tokens;
		std::string current;
		bool in_quotes = false;
		bool escaping = false;
		for (char ch : line) {
			if (escaping) {
				current.push_back(ch);
				escaping = false;
				continue;
			}

			if (ch == '\\' && in_quotes) {
				escaping = true;
				continue;
			}

			if (ch == '"') {
				in_quotes = !in_quotes;
				continue;
			}

			if (!in_quotes && std::isspace(static_cast<unsigned char>(ch))) {
				if (!current.empty()) {
					tokens.push_back(current);
					current.clear();
				}
				continue;
			}

			current.push_back(ch);
		}

		if (!current.empty()) {
			tokens.push_back(current);
		}

		return tokens;
	}
}

using namespace color;

CLI::CLI(const std::string& app_name)
	: _app_name(app_name) {
}

void CLI::registerModule(const ModuleDef& module)
{
	_modules[module.name] = module;
}

const ModuleDef* CLI::findModule(const std::string& name) const
{
	auto it = _modules.find(name);
	if (it == _modules.end())
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
		startRepl();
		return 0;
	}

	std::vector<std::string> tokens;
	for (int i = 1; i < argc; ++i) {
		tokens.push_back(argv[i]);
	}

	return runCommand(tokens);
}

int CLI::runCommand(const std::vector<std::string>& tokens)
{
	if (tokens.empty()) {
		return 0;
	}

	if (tokens[0] == "help") {
		if (tokens.size() == 1) {
			showHelp();
			return 0;
		}
		if (tokens.size() == 2) {
			auto mod = findModule(tokens[1]);
			if (!mod) {
				printError("Module '" + tokens[1] + "' not found.");
				return 1;
			}
			showModuleHelp(*mod);
			return 0;
		}
	}

	if (tokens.size() < 2) {
		printError("Both module and command must be specified.");
		std::cout << "Usage: " << getAppName() << " <module> <command> [args...]\n";
		std::cout << "Use '" << getAppName() << " help' to see available modules.\n";
		std::cout << "Use \\q to leave the REPL.\n";
		return 1;
	}

	std::string mod_name = tokens[0];
	std::string cmd_name = tokens[1];

	const ModuleDef* module = findModule(mod_name);
	if (!module) {
		printError("Module '" + mod_name + "' not found.");
		std::cout << "Use '" << getAppName() << " help' to see available modules.\n";
		return 1;
	}

	const CommandDef* command = findCommand(*module, cmd_name);
	if (!command) {
		printError("Command '" + cmd_name + "' not found in module '" + mod_name + "'.");
		std::cout << "Use '" << getAppName() << " help " << mod_name << "' to see available commands.\n";
		return 1;
	}

	if (!command->run) {
		printError("Command '" + cmd_name + "' has no implementation.");
		return 1;
	}

	Args args;
	args.cli = this;
	for (size_t i = 2; i < tokens.size(); ++i) {
		args.positional.push_back(tokens[i]);
	}

	return command->run(args);
}

void CLI::startRepl()
{
	std::string line;
	while (true) {
		std::cout << color::TEAL <<color::BOLD
			<< getAppName() 
			<< color::RESET << "> ";
		if (!std::getline(std::cin, line)) {
			break;
		}
		auto tokens = tokenizeInput(line);
		if (tokens.empty()) {
			continue;
		}
		if (
			tokens[0] == "exit" || 
			tokens[0] == "quit" ||
			tokens[0] == "\\q"
		) {
			break;
		}
		runCommand(tokens);
	}
}

void CLI::showHelp() const
{
	std::cout << getAppName() << "\n";
	std::cout << "Usage: " << getAppName() << " <module> <command> [args...]\n";
	std::cout << "Modules:\n";
	for (const auto& mod : _modules) {
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