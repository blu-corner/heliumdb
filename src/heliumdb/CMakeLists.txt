set (SOURCES
     module.cpp
     utils.cpp
     iter.cpp
    )

add_library (heliumdb SHARED ${SOURCES})
target_link_libraries(heliumdb he)
set_target_properties (heliumdb PROPERTIES PREFIX "")

install (TARGETS heliumdb DESTINATION lib/python)
