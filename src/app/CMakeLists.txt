add_executable(farmos
    main.cpp
    app_state.cpp
    handle_tx.cpp
)

target_include_directories(farmos
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(farmos
    PRIVATE
        farmOS_cli
        farmOS_core
)