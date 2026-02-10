#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <typeindex>


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

class CLI;

struct Args
{
    std::vector<std::string> positional;
    CLI* cli = nullptr;  
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

class StateComponent{
public:
    virtual ~StateComponent() = default;
};

class CLI
{
public:
    explicit CLI(const std::string& app_name);

    void registerModule(const ModuleDef& module);
    
    template<typename T>
    void registerState(T* component) {
        _state_components[typeid(T)] = component;
    }

    template<typename T>
    T* getState() const {
        auto it = _state_components.find(typeid(T));
        if (it == _state_components.end())
            return nullptr;
        return static_cast<T*>(it->second);
    }

    template<typename T>
    bool hasState() const {
        return _state_components.find(typeid(T)) != _state_components.end();
    }

    int run(int argc, char** argv);

private:
    const ModuleDef* findModule(const std::string& name) const ;
    const CommandDef* findCommand(const ModuleDef& module,
                                  const std::string& name) const ;
    void showHelp() const;
    void showModuleHelp(const ModuleDef& module) const;    

    std::string _app_name;
    std::unordered_map<std::string, ModuleDef> _modules;
    std::unordered_map<std::type_index, StateComponent*> _state_components;
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
