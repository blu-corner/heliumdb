add_test(NAME pyunittest
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/python/test_runner.py ${CMAKE_INSTALL_PREFIX}/lib/python
)