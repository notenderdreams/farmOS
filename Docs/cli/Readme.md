# Better Call CLI

**File:** `src/cli/cli.h`

A module-based CLI framework. You group commands into modules and run them as:

```
<app> <module> <command> [args...]
```

```
farmos employee add batman
farmos employee remove batman
farmos help
farmos help employee
```

## Quick Start

```cpp
#include "cli.h"

int addPerson(const Args& args) 
{
    std::string name;
    loadArg(name, 0, "name");
    std::cout << "Added: " << name << "\n";
    return 0;
}

int main(int argc, char** argv) 
{
    CLI cli("farmos");

    REG_MOD(cli, person, "Manage persons") {
        ADD_CMD(add, "Add a person", addPerson);
    }

    return cli.run(argc, argv);
}
```

```
farmos person add batman
```

## Documentation

- [Commands](Commands.md) — How to write command functions
- [Arguments](Arguments.md) — How to access user input
- [Modules](Modules.md) — How to register modules
- [State Management](StateManagement.md) — How to share data across commands
- [Widgets](Widgets.md) — Terminal input helpers