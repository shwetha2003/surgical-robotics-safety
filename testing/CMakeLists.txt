# Testing CMake Configuration
# Check if GTest is available, but don't fail if it's not
find_package(GTest QUIET)

if(GTest_FOUND)
    message(STATUS "GTest found - building tests")
    
    add_executable(test_safety_monitor test_safety_monitor.cpp)
    target_link_libraries(test_safety_monitor core_engine GTest::gtest GTest::gtest_main)

    add_executable(test_kinematics test_kinematics.cpp)
    target_link_libraries(test_kinematics core_engine GTest::gtest GTest::gtest_main)

    # Add tests
    include(GoogleTest)
    gtest_discover_tests(test_safety_monitor)
    gtest_discover_tests(test_kinematics)
else()
    message(WARNING "GTest not found - skipping C++ test builds")
    message(STATUS "Python tests will still run")
endif()
