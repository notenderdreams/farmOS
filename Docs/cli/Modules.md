# Modules

## Using Macros (Recommended)

`REG_MOD` takes the CLI instance, module name (no quotes), and a description.
`ADD_CMD` takes the command name (no quotes), a description, and the function.

```cpp
REG_MOD(cli, person, "Manage persons") {
    ADD_CMD(add,    "Add a person",    addPerson);
    ADD_CMD(delete, "Delete a person", deletePerson);
}

REG_MOD(cli, employee, "Manage employees") {
    ADD_CMD(add,    "Add an employee",    addEmployee);
    ADD_CMD(remove, "Remove an employee", removeEmployee);
}
```

## Manual Way

```cpp
cli.registerModule({
    "person",
    "Manage persons",
    {
        { "add",    { "Add a person",    addPerson    } },
        { "delete", { "Delete a person", deletePerson } }
    }
});
```

