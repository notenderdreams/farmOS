# Commands

Every command function **must** have this signature:

```cpp
int functionName(const Args& args)
```

No exceptions. The CLI won't work otherwise.

```cpp
int addPerson(const Args& args)
{
    // your logic here
    return 0;
}
```