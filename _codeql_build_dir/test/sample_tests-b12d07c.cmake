add_test( test.txt /home/runner/work/ccwc/ccwc/_codeql_build_dir/test/sample test.txt  )
set_tests_properties( test.txt PROPERTIES WORKING_DIRECTORY /home/runner/work/ccwc/ccwc/_codeql_build_dir/test)
set( sample_TESTS test.txt)
