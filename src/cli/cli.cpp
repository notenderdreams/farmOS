#include "cli.h"
#include <iostream>

// For Colored printing
#define ANSI_RESET   "\033[0m"
#define ANSI_BLUE    "\033[94m"
#define ANSI_GREEN   "\033[92m"
#define ANSI_GREY     "\033[2m"
#define ANSI_BG_GRAY "\033[100m"


CLI::CLI(std::string app_name) 
	: app_name_(std::move(app_name)) {}

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
	const std::string& name)
const
{
	auto it = module.commands.find(name);

	if (it == module.commands.end())
		return nullptr;

	return &it->second;
}

int CLI::run(int argc, char** argv)
{	
	// argc		=	number of args 
	// argv[0]	=	Program Mame [Not useful]
	// argv[1]	=	Module Name
	// argv[2]	=	Command Name
	// after these all the other args are passed to the command 


	//! TODO: FIX THE HELP .
	//! TODO: ADD MODULE HELP SUPPORT
	if ( argc >1 && std::string(argv[1]) == "help") {
		help();
		return 0;
	}
	
	if (argc < 3) {
		std::cerr << "Usage: " << app_name_ << " <module> <command> [args...]\n";
		return 1;
	}

	std::string mod_name = argv[1];
	std::string cmd_name = argv[2];

	const ModuleDef* module = findModule(mod_name);
	if (!module) {
		std::cerr << "Error: module '" << mod_name << "' not found\n";
		return 1;
	}

	const CommandDef* command = findCommand(*module, cmd_name);
	if (!command) {
		std::cerr << "Error: command '" << cmd_name
			<< "' not found in module '" << mod_name << "'\n";
		return 1;
	}

	Args args;
	for (int i = 3; i < argc; ++i) {
		args.positional.push_back(argv[i]);
	}

	if (!command->run) {
		std::cerr << "Error: command '" << cmd_name << "' has no implementation\n";
		return 1;
	}

    return command->run(args);
}

void CLI::help() const
{
	std::cout << app_name_ << "\n";
	std::cout<< "Modules :  \n";

	for (const auto& mod : modules_) {
		std::cout<<ANSI_GREY<< "---------------------------------\n"<<ANSI_RESET;
		std::cout <<ANSI_BLUE<< "# " << mod.first << ANSI_RESET << "\n"
			<< "  " << ANSI_GREY << mod.second.description <<"\n";

		std::cout << "  Commands:\n"<<ANSI_RESET;

		for (const auto& cmd : mod.second.commands) {
			std::cout << "    "
				<< ANSI_GREEN << " " << cmd.first<< ANSI_RESET
				<< "\t:  " << cmd.second.description
				<< "\n";
		}

		std::cout<<ANSI_GREY<< "---------------------------------\n"<<ANSI_RESET;
	}
}

