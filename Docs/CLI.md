# Better Call CLI

This document explains how to use the custom CLI framework.


## Overview

This is a **module-based CLI system**.

Command format:

```
<app> <module> <command> [args...]
```

Examples:

```
farmos employee add batman
farmos employee remove batman
```

---

## Creating a Command Function

To make a function usable by the CLI, it must follow **one simple rule**:

 The function **must** take this parameter:

```cpp
(const Args& args)
```

That’s it.

Example:

```cpp
int functionName(const Args& args)
{
    // logic here
}
```

If you don’t follow this pattern, the CLI **will not work**.

---

## How Arguments Work

The `args` parameter contains **all arguments passed after the command**.

Internally it looks like this:

```cpp
struct Args 
{
    vector<string> positional;
};
```

So arguments are stored like this:

```
args.positional[0]  // first argument
args.positional[1]  // second argument
```

Example (raw access):

```cpp
std::string id = args.positional[0];
```

⚠️ This is unsafe if the argument is missing.

So we use **macros** to make life easier.

---

## Argument Macros (IMPORTANT)

### 1. `getArg(position)`

Use this **only if you know what the hell you are doing**.

```cpp
std::string nickname = getArg(1);
```

If the argument does not exist, it returns an **empty string**.

---

### 2. `loadArg(variable, position, name)`

Use this when the argument is **required**.

```cpp
std::string id;
loadArg(id, 0, "id");
```

If the argument is missing:

* It prints an error
* It exits the function immediately

Error shown:

```
Error: missing argument 'id'
```

---

## Simple Example

Command:

```
farmos person update <id> [nickname]
```

Code:

```cpp
int updatePerson(const Args& args)
{
    // REQUIRED argument
    std::string id;
    loadArg(id, 0, "id");

    // OPTIONAL argument
    std::string nickname = getArg(1);

    std::cout << "Updating person ID = " << id << "\n";

    if (!nickname.empty()) {
        std::cout << "New nickname = " << nickname << "\n";
    } else {
        std::cout << "No nickname provided\n";
    }

    return 0;
}
```

---

## Creating CLI and Modules

The `CLI` class is the **entry point**.

Create it using the app name:

```cpp
CLI cli("farmOS");
```

---

## Module Definition

A module is a group of commands.

```cpp
struct ModuleDef
{
    string name;
    string description;
    unordered_map<string, CommandDef> commands;
};
```

---

## Command Definition

Each command needs:

* A description
* A function

```cpp
struct CommandDef
{
    std::string description;
    std::function<int(const Args&)> run;
};
```

---

## Registering a Module (Simple Way)

```cpp
cli.registerModule({
    "person",           // module name
    "Manage persons",   // module description
    {
        { "add",    { "Add a person",    addPerson    } },
        { "delete", { "Delete a person", deletePerson } }
        //  ^ keyword   ^ description        ^ function
    }
});
```

---

### How This Part Works

```cpp
{ "delete", { "Delete a person", deletePerson } }
```

Explanation:

```
"delete"        -> command keyword (string key)
"Delete a person" -> command description
 deletePerson    -> function that runs
```

The description and function are part of `CommandDef`.