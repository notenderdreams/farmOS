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
// Already defined in transaction.h
enum class TransactionType { BUY, SELL, SALARY, BILLS };
const char* TransactionTypeStrs[] = {"BUY", "SELL", "SALARY", "BILLS"};

TransactionType stt(const std::string& s) {
    if (s == "BUY")    return TransactionType::BUY;
    if (s == "SELL")   return TransactionType::SELL;
    if (s == "SALARY") return TransactionType::SALARY;
    if (s == "BILLS")  return TransactionType::BILLS;
    throw std::runtime_error("Invalid type: " + s);
}

// Usage
TransactionType t = wx::selectInput<TransactionType>(
    "Transaction type:", tx::TransactionTypeStrs, 4, tx::stt
);
```

User sees:
```
Transaction type:
1. BUY
2. SELL
3. SALARY
4. BILLS
Enter your choice (1-4): 
```
