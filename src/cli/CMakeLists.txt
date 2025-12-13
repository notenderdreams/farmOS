add_executable(farmOS_cli
    main.cpp cli.cpp
)

# Link the CLI executable against the core static library.
target_link_libraries(farmOS_cli
    PRIVATE
        farmOS_core
)
