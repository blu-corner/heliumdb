set (SOURCES
     module.cpp
     utils.cpp
     iter.cpp
    )

add_library (heliumdb SHARED ${SOURCES})
target_link_libraries(heliumdb ${LIBHE})
set_target_properties (heliumdb PROPERTIES PREFIX "")
set_target_properties (heliumdb PROPERTIES SUFFIX ".so")

install (TARGETS heliumdb DESTINATION src/heliumdb)
