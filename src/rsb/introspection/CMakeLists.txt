# Create and install plugin library.
set(PLUGIN_NAME rsbintrospection)
add_library(${PLUGIN_NAME} SHARED "Plugin.cpp"
                                  "IntrospectionSender.cpp")

target_link_libraries(${PLUGIN_NAME} ${RSC_LIBRARIES}
                                     ${RSB_LIBRARIES})
set_target_properties(${PLUGIN_NAME}
                      PROPERTIES
                      VERSION ${SO_VERSION})

install(TARGETS ${PLUGIN_NAME}
        LIBRARY DESTINATION "${RSB_RELATIVE_PLUGIN_DIRECTORY}" NAMELINK_SKIP
        RUNTIME DESTINATION "bin/${RSB_PLUGIN_PATH_SUFFIX}")
