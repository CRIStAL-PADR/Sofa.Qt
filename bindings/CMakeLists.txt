project(Bindings_Sofa.Qt)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Module_Sofa.Qt.cpp
)

if (NOT TARGET SofaPython3::Plugin)
    find_package(SofaPython3 REQUIRED COMPONENTS Sofa.Qt Bindings.Sofa)
endif()

SP3_add_python_module(
    TARGET       ${PROJECT_NAME}
    PACKAGE      SofaQt
    MODULE       SofaQt
    DESTINATION  .
    SOURCES      ${SOURCE_FILES}
    DEPENDS      SofaPython3::Plugin SofaPython3::Bindings.Sofa Sofa.Qt

)
message("-- SofaPython3 bindings for Sofa.Qt will be created.")
