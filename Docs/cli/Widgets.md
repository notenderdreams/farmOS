# Widgets Documentation

**File:** `src/cli/widgets.h`  
**Namespace:** `wx`


## `wx::lineInput<T>`

Prompts the user and reads a value.

```cpp
double price = wx::lineInput<double>("Price: ");
std::string name = wx::lineInput<std::string>("Name: ");
```

- `std::string` reads the full line (spaces included)
- Everything else reads a single word/number



## `wx::selectInput<E>`

Shows a numbered list, user picks one, returns an enum.

```cpp
E selectInput(prompt, options[], length, converterFunction)
```

**Requirements:**
1. A `const char*` array of option strings
2. A function that converts a string to your enum

```cpp
    enum class TType { BUY, SELL };
    char* array_of_strings[] = {"BUY", "SELL"};
    int len = 2;

    TType converter (string& s) {
        if (s == "BUY")    return TType::BUY;
        if (s == "SELL")   return TType::SELL;
        throw std::runtime_error("Invalid type: " + s);
    }

    string prompt = "Transaction Type:";

    E t = wx::selectInput<TType>(
        prompt,
        array_of_strings,
        len,
        converter
    );
```

User sees:
```
Transaction type:
1. BUY
2. SELL
Enter your choice (1-2): 
```
