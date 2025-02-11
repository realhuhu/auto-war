cmake_minimum_required(VERSION 3.27)
project(qt)
add_definitions(-DUNICODE -D_UNICODE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "D:/Qt/5.15.2/msvc2019_64")

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network REQUIRED)

set(OpenCV_DIR "D:/opencv/dist_vc/install")
include(D:/opencv/dist_vc/install/OpenCVConfig.cmake)
find_package(OpenCV REQUIRED)

qt5_add_resources(qrc_FILES resources.qrc)

add_executable(qt
        WIN32
        ${qrc_FILES}
        ico.o
        util/state.h
        util/state.cpp
        util/sleep.h
        util/sleep.cpp
        util/action.h
        util/action.cpp
        util/emitter.h
        util/enum.h
        util/enum.cpp
        flow/cv.h
        flow/cv.cpp
        flow/until.h
        flow/until.cpp
        flow/runner.h
        flow/runner.cpp
        flow/segment.h
        flow/segment.cpp
        widget/buttonWithSetting.h
        widget/buttonWithSetting.cpp
        widget/labeledSpinBox.h
        widget/labeledSpinBox.cpp
        widget/labeledComboBox.h
        widget/labeledComboBox.cpp
        task/common.h
        task/common.cpp
        task/battle.h
        task/battle.cpp
        task/daily.h
        task/daily.cpp
        function/clickerDialog.h
        function/clickerDialog.cpp
        function/imageDialog.cpp
        function/imageDialog.h
        function/activityDialog.h
        function/activityDialog.cpp
        main.h
        main.cpp
)

target_link_libraries(qt ${OpenCV_LIBS})
target_link_libraries(qt Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
set_target_properties(qt PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")


if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt5${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
