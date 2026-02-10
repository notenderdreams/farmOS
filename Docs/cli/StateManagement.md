# State Management
Share data (like a database connection) across commands using `StateComponent`:

```cpp
class AppState : public StateComponent {
public:
    Database* db;
};
```

Register in main:

```cpp
cli.registerState(new AppState());
```

Access inside any command:

```cpp
auto* state = args.cli->getState<AppState>();