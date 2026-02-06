add_library(farmOS_core STATIC
    farm_os.cpp
    transaction_service.cpp
    database.cpp
)

target_include_directories(farmOS_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(farmOS_core
    PUBLIC sqlite3
)
