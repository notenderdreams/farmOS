add_executable(cli
    main.cpp
    cli.cpp
    color.cpp
)

target_link_libraries(cli
    PRIVATE
        farmOS_core
)
