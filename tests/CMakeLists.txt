project(mai-os)
include(FetchContent)

FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)

FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)


set(CMAKE_CXX_STANDARD 17)

# add_executable(LW1_test lab1_test.cpp)
# target_include_directories(LW1_test PUBLIC ../LW1/include)
# target_link_libraries(LW1_test PRIVATE utils parent GTest::GTest)

add_executable(LW3_test lab3_test.cpp)
target_include_directories(LW3_test PUBLIC ../LW3/include)
target_link_libraries(LW3_test PRIVATE utils parent GTest::GTest)

add_executable(LW4_test test4.cpp)
target_include_directories(LW4_test PUBLIC ../LW4/include)
target_link_libraries(LW4_test PRIVATE GTest::gtest GTest::gtest_main dl)

add_executable(LW567_test lab567_test.cpp)
target_include_directories(LW567_test PUBLIC ../LW567/include)
target_link_libraries(LW567_test PRIVATE lab567_tools lab567_worker lab567_controller /usr/lib/x86_64-linux-gnu/libzmq.so GTest::GTest)

enable_testing()
# add_test(NAME LW1_test COMMAND LW1_test)
# add_test(NAME LW3_test COMMAND LW3_test)
# add_test(NAME LW4_test COMMAND LW4_test)
 #add_test(NAME LW567_test COMMAND LW567_test)