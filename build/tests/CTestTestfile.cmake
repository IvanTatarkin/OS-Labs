# CMake generated Testfile for 
# Source directory: /home/ivan/OS-Labs/tests
# Build directory: /home/ivan/OS-Labs/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(LW1_test "/home/ivan/OS-Labs/build/tests/LW1_test")
set_tests_properties(LW1_test PROPERTIES  _BACKTRACE_TRIPLES "/home/ivan/OS-Labs/tests/CMakeLists.txt;22;add_test;/home/ivan/OS-Labs/tests/CMakeLists.txt;0;")
subdirs("../_deps/googletest-build")
