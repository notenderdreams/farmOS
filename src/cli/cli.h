#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

// ## ARG MACROS
#define getArg(pos) \
    ((pos) < args.positional.size() ? args.positional[pos] : "")

#define loadArg(var, pos, name) \
    if ((pos) >= args.positional.size()) { \
        std::cerr << "Error: missing argument '" << name << "'\n"; \
        return 1; \
    } else { \
        var = getArg(pos); \
    }

// ## BUILD / REG MACROS
#define REG_MOD(cli,name,desc) \
    if (ModuleBuilder __module(cli, #name, desc); true)

#define ADD_CMD(name, desc, fn) \
    __module.cmd(#name, desc, fn)


struct Args
{
    std::vector<std::string> positional;
};

struct CommandDef
{
    std::string description;
    std::function<int(const Args&)> run;
};

struct ModuleDef
{
    std::string name;
    std::string description;
    std::unordered_map<std::string, CommandDef> commands;
};

class CLI
{
public:
    explicit CLI(const std::string& app_name);

    void registerModule(const ModuleDef& module);
    int run(int argc, char** argv);

private:
    const ModuleDef* findModule(const std::string& name) const ;
    const CommandDef* findCommand(const ModuleDef& module,
                                  const std::string& name) const ;
    void showHelp() const;
    void showModuleHelp(const ModuleDef& module) const;    
    std::string _app_name;
    std::unordered_map<std::string, ModuleDef> modules_;
};

class ModuleBuilder
{
public:
    ModuleBuilder(CLI& cli,
        std::string name,
        std::string desc
    ) : _cli(cli),
        _name(std::move(name)),
        _desc(std::move(desc)) {
    }

    void cmd(const std::string& name,
        const std::string& desc,
        std::function<int(const Args&)>fn
    ) {
        _cmds[name] = { desc,fn };
    }

    ~ModuleBuilder()
    {
        _cli.registerModule({ _name,_desc,_cmds });
    }

private:
    CLI& _cli;
    std::string _name;
    std::string _desc;
    std::unordered_map<std::string, CommandDef>_cmds;
            
};
