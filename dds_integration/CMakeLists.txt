# DDS Integration CMake Configuration
add_library(dds_integration
    data_publisher.cpp
    command_subscriber.cpp
)

target_include_directories(dds_integration PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Note: RTI Connext DDS would be linked here
# target_link_libraries(dds_integration nddsc nddscpp nddscore)
