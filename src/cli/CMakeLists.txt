add_executable(cli
    main.cpp
    cli.cpp
)

target_link_libraries(cli
    PRIVATE
        farmOS_core
)
