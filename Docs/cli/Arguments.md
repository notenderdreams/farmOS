# Arguments

Everything the user types after the command goes into `args.positional`:

```
farmos person update 42 batman
                     ^    ^
              positional[0]  positional[1]
```

## `loadArg(variable, position, name)` — Required

```cpp
std::string id;
loadArg(id, 0, "id");
```

If missing, prints `Error: missing argument 'id'` and exits the function.

## `getArg(position)` — Optional

```cpp
std::string nickname = getArg(1);
```

Returns an empty string if missing.

## Example

```
farmos person update <id> [nickname]
```

```cpp
int updatePerson(const Args& args)
{
    std::string id;
    loadArg(id, 0, "id");             // required

    std::string nickname = getArg(1);  // optional

    std::cout << "Updating ID: " << id << "\n";
    if (!nickname.empty())
        std::cout << "Nickname: " << nickname << "\n";

    return 0;
}
```