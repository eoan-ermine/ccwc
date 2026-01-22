# CMake generated Testfile for 
# Source directory: /home/runner/work/ccwc/ccwc/test
# Build directory: /home/runner/work/ccwc/ccwc/_codeql_build_dir/test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("/home/runner/work/ccwc/ccwc/_codeql_build_dir/test/sample_include-b12d07c.cmake")
add_test([=[sample_test]=] "sample")
set_tests_properties([=[sample_test]=] PROPERTIES  _BACKTRACE_TRIPLES "/home/runner/work/ccwc/ccwc/test/CMakeLists.txt;19;add_test;/home/runner/work/ccwc/ccwc/test/CMakeLists.txt;0;")
subdirs("../_deps/catch2-build")
