#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>


#define getArg(pos) \
    ((pos) < args.positional.size() ? args.positional[pos] : "")

#define loadArg(var, pos, name) \
    if ((pos) >= args.positional.size()) { \
        std::cerr << "Error: missing argument '" << name << "'\n"; \
        return 1; \
    } else { \
        var = getArg(pos); \
    }


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
    explicit CLI(std::string app_name);

    void registerModule(const ModuleDef& module);
    int run(int argc, char** argv);

private:
    const ModuleDef* findModule(const std::string& name) const ;
    const CommandDef* findCommand(const ModuleDef& module,
                                  const std::string& name) const ;
    void help() const;
    std::string app_name_;
    std::unordered_map<std::string, ModuleDef> modules_;
};
