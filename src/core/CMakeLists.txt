add_library(farmOS_core STATIC
    transaction_service.cpp
    inventory_service.cpp
    database.cpp
    models/transaction.cpp
    models/inventory.cpp
    models/animal.cpp
)

target_include_directories(farmOS_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(farmOS_core
    PUBLIC sqlite3
)
