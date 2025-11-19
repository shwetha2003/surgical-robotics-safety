# DDS Integration CMake Configuration
# Note: This is a simulation - real DDS would require RTI Connext

# Only build if we have the source files
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/data_publisher.cpp" AND 
   EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/command_subscriber.cpp")
    
    add_library(dds_integration
        data_publisher.cpp
        command_subscriber.cpp
    )

    target_include_directories(dds_integration PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
    
    message(STATUS "DDS integration library built (simulation mode)")
else()
    message(WARNING "DDS integration source files not found - skipping")
endif()
