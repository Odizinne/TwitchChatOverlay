cmake_minimum_required(VERSION 3.16)

project(TwitchChatOverlay VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick Network)

qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(${CMAKE_PROJECT_NAME}
    main.cpp
)

set(QML_SINGLETONS
    UserSettings.qml
)

set_source_files_properties(${QML_SINGLETONS}
    PROPERTIES QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(${CMAKE_PROJECT_NAME}
    URI Odizinne.${CMAKE_PROJECT_NAME}
    VERSION 1.0
    QML_FILES
        Main.qml
        SystemTray.qml
        SettingsDialog.qml
        CustomWindow.qml
        SOURCES twitchchatclient.h twitchchatclient.cpp
        SOURCES shortcutmanager.h shortcutmanager.cpp
        QML_FILES ${QML_SINGLETONS}
)

set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(${CMAKE_PROJECT_NAME}
    PRIVATE Qt6::Quick Qt6::Network
)

include(GNUInstallDirs)
install(TARGETS ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
