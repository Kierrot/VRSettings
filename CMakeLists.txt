cmake_minimum_required(VERSION 3.19)
project(VR_Settings LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    OpenGLWidgets
)

qt_standard_project_setup()

qt_add_executable(VR_Settings
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    registrymanager.h registrymanager.cpp
    filemanager.h filemanager.cpp
)

target_link_libraries(VR_Settings
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt6::OpenGLWidgets
)

include(GNUInstallDirs)

install(TARGETS VR_Settings
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET VR_Settings
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
