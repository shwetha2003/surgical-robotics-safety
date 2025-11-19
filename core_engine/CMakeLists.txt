# Core Engine CMake Configuration
add_library(core_engine
    safety_monitor.cpp
    kinematics_solver.cpp
    collision_detector.cpp
    real_time_controller.cpp
)

target_include_directories(core_engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(core_engine Eigen3::Eigen)

# Create executable for testing
add_executable(safety_demo safety_demo.cpp)
target_link_libraries(safety_demo core_engine)
