cmake_minimum_required(VERSION 3.22)
project(Sofa.Qt LANGUAGES CXX)

# Detect if component is compiled outside SOFA
if ("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
    find_package(Sofa.Config REQUIRED)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
    sofa_find_package(Sofa.GUI.Common REQUIRED)
endif()

# Qt dependencies
set(SOFA_QT_TARGETS "")
set(QT_USE_IMPORTED_TARGETS 1)
set(QT5_NO_LINK_QTMAIN 1)




# Will only use Qt5 if Qt5 is found and Qt6 is not found
# if Qt5 and Qt6 are both found, Qt6 will take priority
find_package(Qt6 COMPONENTS Core CoreTools QUIET)
if (NOT Qt6Core_FOUND)
    find_package(Qt5 COMPONENTS Core QUIET)
endif()

if (Qt6Core_FOUND)
    message("${PROJECT_NAME}: will use Qt6")
    find_package(Qt6 COMPONENTS Gui GuiTools Widgets WidgetsTools OpenGLWidgets REQUIRED)
    # GuiTools & WidgetsTools does not define a Qt6::component CMake target
    # So we can't look for those target to know how we should define SOFA_QT_HAVE_QT6
    sofa_find_package(Qt6 COMPONENTS Gui Widgets OpenGLWidgets REQUIRED)
    set(SOFA_QT_TARGETS ${SOFA_QT_TARGETS} Qt::Core Qt::Gui Qt::Widgets Qt::OpenGLWidgets )
elseif (Qt5Core_FOUND)
    message("${PROJECT_NAME}: will use Qt5 (deprecated)")
    sofa_find_package(Qt5 COMPONENTS Core Gui OpenGL REQUIRED)
    set(SOFA_QT_TARGETS ${SOFA_QT_TARGETS} Qt5::Core Qt5::Gui Qt5::OpenGL)
else()
    message(SEND_ERROR "${PROJECT_NAME}: Could not find either Qt5 or Qt6.")
endif()

if (Qt5Core_FOUND)
    # Profiling
    sofa_find_package(Qt5 COMPONENTS Charts QUIET BOTH_SCOPES)
    if(Qt5Charts_FOUND)
        set(SOFA_QT_TARGETS ${SOFA_QT_TARGETS} Qt5::Charts)
    else()
        message(STATUS "${PROJECT_NAME}: Qt5Charts not found. No chart nor Profile will be compiled")
    endif()

    # QDocBrowser
    find_package(Qt5 COMPONENTS WebEngine QUIET) # if found, then QDocBrowser will be ON by default
    option(SOFA_QT_ENABLE_QDOCBROWSER "Build the QDocBrowser. QtWebEngine is needed." ${Qt5WebEngine_FOUND})

    if(SOFA_QT_ENABLE_QDOCBROWSER)
        sofa_find_package(Qt5 COMPONENTS WebEngine WebEngineWidgets REQUIRED BOTH_SCOPES)
        set(SOFA_QT_TARGETS ${SOFA_QT_TARGETS} Qt5::WebEngine Qt5::WebEngineWidgets)
    endif()
elseif (Qt6Core_FOUND)

    # Profiling
    sofa_find_package(Qt6 COMPONENTS Charts QUIET BOTH_SCOPES)
    if(Qt6Charts_FOUND)
        set(SOFA_QT_TARGETS ${SOFA_QT_TARGETS} Qt::Charts)
    else()
        message(STATUS "${PROJECT_NAME}: Qt6 Charts not found. No chart nor Profile will be compiled")
    endif()

    # # QDocBrowser
    find_package(Qt6 COMPONENTS WebEngineCore QUIET) # if found, then QDocBrowser will be ON by default
    option(SOFA_QT_ENABLE_QDOCBROWSER "Build the QDocBrowser. QtWebEngine is needed." ${Qt6WebEngineCore_FOUND})
    if(SOFA_QT_ENABLE_QDOCBROWSER)
         # WebEngineCore needs Positioning WebChannel at cmake configure step (?)
        sofa_find_package(Qt6 COMPONENTS Positioning WebChannel WebEngineCore WebEngineWidgets REQUIRED BOTH_SCOPES)
        set(SOFA_QT_TARGETS ${SOFA_QT_TARGETS} Qt::WebEngineCore Qt::WebEngineWidgets)
    endif()
endif()


sofa_find_package(Sofa.GL QUIET)

# QtViewer and QGLViewer
if(Sofa.GL_FOUND)
    # QtViewer
    option(SOFA_QT_ENABLE_QTVIEWER "Compile the QtViewer for the Qt GUI" ON)

    # QGLViewer
    option(SOFA_QT_ENABLE_QGLVIEWER "Compile the QGLViewer for the Qt GUI" ON)

    if(SOFA_QT_ENABLE_QGLVIEWER)

        find_package(QGLViewer QUIET)
        if(NOT QGLViewer_FOUND AND SOFA_ALLOW_FETCH_DEPENDENCIES)
            message("Sofa.Qt: DEPENDENCY QGLViewer NOT FOUND. SOFA_ALLOW_FETCH_DEPENDENCIES is ON, fetching QGLViewer...")
            include(FetchContent)
            FetchContent_Declare(QGLViewer
                    GIT_REPOSITORY https://github.com/sofa-framework/libQGLViewer/
                    GIT_TAG v2.8.1_SOFA
            )
            FetchContent_MakeAvailable(QGLViewer)
        elseif (NOT QGLViewer_FOUND)
            message(FATAL_ERROR "Sofa.Qt: DEPENDENCY QGLViewer NOT FOUND. SOFA_ALLOW_FETCH_DEPENDENCIES is OFF and thus cannot be fetched. Install QGLViewer (version=2.8.0), or enable SOFA_ALLOW_FETCH_DEPENDENCIES to fix this issue.")
        endif()
    endif()
else()
    message(STATUS "Sofa.Qt: Sofa.GL has not been activated, QtViewer/QGLViewer will be disabled.")
    set(SOFA_QT_ENABLE_QTVIEWER OFF)
    set(SOFA_QT_ENABLE_QGLVIEWER OFF)
endif()

# VSync
option(SOFA_QT_ENABLE_VSYNC "Enable vertical refresh sync" OFF)

# Qt native menu
set(DEFAULT_SOFA_QT_ENABLE_NATIVE_MENU ON)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(DEFAULT_SOFA_QT_ENABLE_NATIVE_MENU OFF)
endif()
option(SOFA_QT_ENABLE_NATIVE_MENU "Enable Qt to use the operating system's native menu for the Qt GUI" ${DEFAULT_SOFA_QT_ENABLE_NATIVE_MENU})

# NodeEditor
find_package(NodeEditor QUIET) # if found, then NodeGraph will be ON by default
option(SOFA_QT_ENABLE_NODEGRAPH "Build the Node graph representation. NodeEditor library is needed." ${NodeEditor_FOUND})

# FFMPEG_exec
sofa_find_package(FFMPEG_exec BOTH_SCOPES)

# TinyXML
if(SOFA_DUMP_VISITOR_INFO)
    sofa_find_package(TinyXML2 REQUIRED BOTH_SCOPES)
endif()



set(SRC_ROOT src/sofa/qt)

set(MOC_HEADER_FILES
    ${SRC_ROOT}/AboutSOFADialog.h
    ${SRC_ROOT}/DataFilenameWidget.h
    ${SRC_ROOT}/DataWidget.h
    ${SRC_ROOT}/DisplayFlagsDataWidget.h
    ${SRC_ROOT}/GenGraphForm.h
    ${SRC_ROOT}/LinkWidget.h
    ${SRC_ROOT}/MaterialDataWidget.h
    ${SRC_ROOT}/RGBAColorDataWidget.h
    ${SRC_ROOT}/ModifyObject.h
    ${SRC_ROOT}/QDataDescriptionWidget.h
    ${SRC_ROOT}/QDisplayDataWidget.h
    ${SRC_ROOT}/QDisplayLinkWidget.h
    ${SRC_ROOT}/QDisplayPropertyWidget.h
    ${SRC_ROOT}/QRGBAColorPicker.h
    ${SRC_ROOT}/QModelViewTableUpdater.h
    ${SRC_ROOT}/QMouseOperations.h
    ${SRC_ROOT}/QMouseWheelAdjustementGuard.h
    ${SRC_ROOT}/QSofaListView.h
    ${SRC_ROOT}/QSofaStatWidget.h
    ${SRC_ROOT}/QTabulationModifyObject.h
    ${SRC_ROOT}/QtMessageRedirection.h
    ${SRC_ROOT}/QTransformationWidget.h
    ${SRC_ROOT}/RealGUI.h
    ${SRC_ROOT}/SimpleDataWidget.h
    ${SRC_ROOT}/SofaMouseManager.h    
    ${SRC_ROOT}/SofaVideoRecorderManager.h
    ${SRC_ROOT}/SofaPluginManager.h
    ${SRC_ROOT}/SofaSceneGraphWidget.h
    ${SRC_ROOT}/WDoubleLineEdit.h    
    )
set(HEADER_FILES
    ${SRC_ROOT}/config.h.in
    ${SRC_ROOT}/fwd.h
    ${SRC_ROOT}/init.h
    ${SRC_ROOT}/qt.conf.h
    ${SRC_ROOT}/FileManagement.h
    ${SRC_ROOT}/GraphListenerQListView.h
    ${SRC_ROOT}/PickHandlerCallBacks.h
    ${SRC_ROOT}/QMenuFilesRecentlyOpened.h
    ${SRC_ROOT}/QModelViewTableDataContainer.h
    ${SRC_ROOT}/StructDataWidget.h
    ${SRC_ROOT}/TableDataWidget.h
    ${SRC_ROOT}/viewer/SofaViewer.h
    ${SRC_ROOT}/viewer/EngineBackend.h
    ${SRC_ROOT}/viewer/VisualModelPolicy.h
    ${SRC_ROOT}/GenericWidget.h
    ${SRC_ROOT}/QtDataRepository.h
    )
set(SOURCE_FILES
    ${SRC_ROOT}/init.cpp
    ${SRC_ROOT}/AboutSOFADialog.cpp
    ${SRC_ROOT}/DataFilenameWidget.cpp
    ${SRC_ROOT}/DataWidget.cpp
    ${SRC_ROOT}/DisplayFlagsDataWidget.cpp
    ${SRC_ROOT}/FileManagement.cpp
    ${SRC_ROOT}/GenGraphForm.cpp
    ${SRC_ROOT}/GraphListenerQListView.cpp
    ${SRC_ROOT}/LinkWidget.cpp
    ${SRC_ROOT}/MaterialDataWidget.cpp
    ${SRC_ROOT}/RGBAColorDataWidget.cpp
    ${SRC_ROOT}/ModifyObject.cpp
    ${SRC_ROOT}/PickHandlerCallBacks.cpp
    ${SRC_ROOT}/QDataDescriptionWidget.cpp
    ${SRC_ROOT}/QDisplayDataWidget.cpp
    ${SRC_ROOT}/QDisplayLinkWidget.cpp
    ${SRC_ROOT}/QDisplayPropertyWidget.cpp
    ${SRC_ROOT}/QRGBAColorPicker.cpp
    ${SRC_ROOT}/QMenuFilesRecentlyOpened.cpp
    ${SRC_ROOT}/QModelViewTableUpdater.cpp
    ${SRC_ROOT}/QMouseOperations.cpp
    ${SRC_ROOT}/QMouseWheelAdjustementGuard.cpp
    ${SRC_ROOT}/QSofaListView.cpp
    ${SRC_ROOT}/QSofaStatWidget.cpp
    ${SRC_ROOT}/QTabulationModifyObject.cpp
    ${SRC_ROOT}/QtMessageRedirection.cpp
    ${SRC_ROOT}/QTransformationWidget.cpp
    ${SRC_ROOT}/RealGUI.cpp
    ${SRC_ROOT}/SimpleDataWidget.cpp
    ${SRC_ROOT}/SofaMouseManager.cpp
    ${SRC_ROOT}/SofaPluginManager.cpp
    ${SRC_ROOT}/SofaVideoRecorderManager.cpp    
    ${SRC_ROOT}/StructDataWidget.cpp
    ${SRC_ROOT}/TableDataWidget.cpp
    ${SRC_ROOT}/WDoubleLineEdit.cpp
    ${SRC_ROOT}/viewer/SofaViewer.cpp
    ${SRC_ROOT}/SofaSceneGraphWidget.cpp
    ${SRC_ROOT}/viewer/VisualModelPolicy.cpp
    ${SRC_ROOT}/QtDataRepository.cpp
)
set(UI_FILES
    ${SRC_ROOT}/AboutDialog.ui
    ${SRC_ROOT}/BaseGenGraphForm.ui
    ${SRC_ROOT}/DialogAddObject.ui
    ${SRC_ROOT}/GUI.ui
    ${SRC_ROOT}/MouseManager.ui
    ${SRC_ROOT}/PluginManager.ui
    ${SRC_ROOT}/VideoRecorderManager.ui
)
set(QRC_FILES
    ${SRC_ROOT}/resources/RealGUI.qrc
)


if(SOFA_DUMP_VISITOR_INFO)
    list(APPEND MOC_HEADER_FILES
        ${SRC_ROOT}/WindowVisitor.h
        ${SRC_ROOT}/QVisitorControlPanel.h
        ${SRC_ROOT}/PieWidget.h
        )
    list(APPEND HEADER_FILES
        ${SRC_ROOT}/GraphVisitor.h
        )
    list(APPEND SOURCE_FILES
        ${SRC_ROOT}/GraphVisitor.cpp
        ${SRC_ROOT}/WindowVisitor.cpp
        ${SRC_ROOT}/QVisitorControlPanel.cpp
        ${SRC_ROOT}/PieWidget.cpp
        )
    list(APPEND UI_FILES
        ${SRC_ROOT}/VisitorGUI.ui
        )
endif()

if(Sofa.GL_FOUND)
    list(APPEND HEADER_FILES
        ${SRC_ROOT}/GLPickHandler.h
        ${SRC_ROOT}/viewer/GLBackend.h
        ${SRC_ROOT}/viewer/OglModelPolicy.h
        )
    list(APPEND SOURCE_FILES
        ${SRC_ROOT}/GLPickHandler.cpp
        ${SRC_ROOT}/viewer/GLBackend.cpp
        ${SRC_ROOT}/viewer/OglModelPolicy.cpp
        )

    # QtViewer
    if(SOFA_QT_ENABLE_QTVIEWER)
        list(APPEND MOC_HEADER_FILES ${SRC_ROOT}/viewer/qt/QtViewer.h)
        list(APPEND SOURCE_FILES ${SRC_ROOT}/viewer/qt/QtViewer.cpp)
    endif()

    if(SOFA_QT_ENABLE_QGLVIEWER)
        list(APPEND MOC_HEADER_FILES ${SRC_ROOT}/viewer/qgl/QtGLViewer.h)
        list(APPEND SOURCE_FILES ${SRC_ROOT}/viewer/qgl/QtGLViewer.cpp)
    endif()

endif()

# QtCharts
if(Qt5Charts_FOUND OR Qt6Charts_FOUND)
    list(APPEND MOC_HEADER_FILES 
        ${SRC_ROOT}/SofaWindowProfiler.h
        ${SRC_ROOT}/QGraphStatWidget.h
        ${SRC_ROOT}/QEnergyStatWidget.h
        ${SRC_ROOT}/QMomentumStatWidget.h
        ${SRC_ROOT}/GraphDataWidget.h
        )
    list(APPEND SOURCE_FILES 
        ${SRC_ROOT}/SofaWindowProfiler.cpp
        ${SRC_ROOT}/QGraphStatWidget.cpp
        ${SRC_ROOT}/QEnergyStatWidget.cpp
        ${SRC_ROOT}/QMomentumStatWidget.cpp
        ${SRC_ROOT}/GraphDataWidget.cpp
        )
    list(APPEND UI_FILES ${SRC_ROOT}/WindowProfiler.ui)
else()
    message(STATUS "${PROJECT_NAME}: QtCharts not found, SofaWindowProfiler will not be built.")
endif()

if(SOFA_QT_ENABLE_QDOCBROWSER)
    list(APPEND MOC_HEADER_FILES ${SRC_ROOT}/panels/QDocBrowser.h)
    list(APPEND SOURCE_FILES ${SRC_ROOT}/panels/QDocBrowser.cpp)
else()
    message(STATUS "${PROJECT_NAME}: QtWebEngine not found, QDocBrowser will not be built.")
endif()




#NodeEditor
if (SOFA_QT_ENABLE_NODEGRAPH)
    list(APPEND MOC_HEADER_FILES 
        ${SRC_ROOT}/SofaWindowDataGraph.h
        ${SRC_ROOT}/dataGraph/SofaComponentNodeModel.h
        )

    list(APPEND SOURCE_FILES 
        ${SRC_ROOT}/SofaWindowDataGraph.cpp
        ${SRC_ROOT}/dataGraph/SofaComponentNodeModel.cpp
        )
endif()

if (Qt5Core_FOUND)
    qt5_wrap_cpp(MOC_FILES ${MOC_HEADER_FILES})
    qt5_wrap_ui(FORM_FILES ${UI_FILES})
elseif (Qt6Core_FOUND)
    qt6_wrap_cpp(MOC_FILES ${MOC_HEADER_FILES})
    qt6_wrap_ui(FORM_FILES ${UI_FILES})
endif()
set(MOC_FILES_GROUP "Generated")
set(FORM_FILES_GROUP "Generated")

sofa_find_package(Sofa.Component.Visual REQUIRED)
sofa_find_package(Sofa.Component.SceneUtility REQUIRED)

set(CMAKE_AUTORCC ON)
add_library(${PROJECT_NAME} SHARED ${MOC_HEADER_FILES} ${HEADER_FILES} ${MOC_FILES} ${FORM_FILES} ${SOURCE_FILES} ${QRC_FILES})


# For files generated by the moc
target_include_directories(${PROJECT_NAME} PUBLIC "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>")

target_link_libraries(${PROJECT_NAME} PUBLIC Sofa.GUI.Common)
target_link_libraries(${PROJECT_NAME} PUBLIC Sofa.Component.Visual)
target_link_libraries(${PROJECT_NAME} PUBLIC Sofa.Component.SceneUtility)
target_link_libraries(${PROJECT_NAME} PUBLIC ${SOFA_QT_TARGETS})



if(SOFA_DUMP_VISITOR_INFO)
    target_link_libraries(${PROJECT_NAME} PRIVATE tinyxml2::tinyxml2)
endif()

if(Sofa.GL_FOUND)
    target_link_libraries(${PROJECT_NAME} PUBLIC Sofa.GL)
    if(SOFA_QT_ENABLE_QGLVIEWER)
        target_link_libraries(${PROJECT_NAME} PUBLIC QGLViewer)
        include_directories(${PROJECT_NAME} PUBLIC ${qglviewer_SOURCE_DIR})
    endif()
endif()

if(SOFA_QT_ENABLE_NODEGRAPH)
    target_link_libraries(${PROJECT_NAME} PUBLIC NodeEditor::nodes)
    if (SOFA_BUILD_RELEASE_PACKAGE OR CMAKE_SYSTEM_NAME STREQUAL Windows)
        sofa_install_libraries(TARGETS NodeEditor::nodes)
    endif()
endif()

# FFMPEG
if(FFMPEG_EXEC_FOUND)
    install(PROGRAMS "${FFMPEG_EXEC_FILE}" DESTINATION bin COMPONENT applications)
endif()

find_package(SofaPython3 QUIET)
if(SofaPython3_FOUND)
    add_subdirectory(bindings)
endif()

# Create build and install versions of .ini file for resources finding
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/${SRC_ROOT}/resources")
set(FFMPEG_EXEC_PATH "${FFMPEG_EXEC_FILE}") # absolute path for build dir, see .ini file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/etc/${PROJECT_NAME}.ini.in "${CMAKE_BINARY_DIR}/etc/${PROJECT_NAME}.ini")
set(RESOURCES_DIR "../share/sofa/gui/qt")
get_filename_component(FFMPEG_EXEC_FILENAME "${FFMPEG_EXEC_FILE}" NAME)
set(FFMPEG_EXEC_PATH "../bin/${FFMPEG_EXEC_FILENAME}") # relative path for install dir, see .ini file
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/etc/${PROJECT_NAME}.ini.in "${CMAKE_BINARY_DIR}/etc/installed${PROJECT_NAME}.ini")
install(FILES "${CMAKE_BINARY_DIR}/etc/installed${PROJECT_NAME}.ini" DESTINATION etc RENAME ${PROJECT_NAME}.ini COMPONENT applications)

install(DIRECTORY "${SRC_ROOT}/resources/" DESTINATION "share/sofa/gui/qt" COMPONENT resources)

sofa_create_package_with_targets(
    PACKAGE_NAME ${PROJECT_NAME}
    PACKAGE_VERSION ${Sofa_VERSION}
    TARGETS ${PROJECT_NAME} AUTO_SET_TARGET_PROPERTIES
    INCLUDE_SOURCE_DIR "src"
    INCLUDE_INSTALL_DIR ${PROJECT_NAME}
    RELOCATABLE "plugins"
    )

# Qt deployment
if(CMAKE_SYSTEM_NAME STREQUAL Windows)
    include(windeployqt)
    windeployqt(${PROJECT_NAME} ${CMAKE_BINARY_DIR}/bin bin)
endif()
