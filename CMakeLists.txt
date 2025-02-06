cmake_minimum_required(VERSION 3.27)
project(qt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(OpenCV_DIR "D:/opencv/dist/install")
set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.14.2/5.14.2/mingw73_64")
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")


find_package(OpenCV REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets REQUIRED)

qt5_add_resources(qrc_FILES resources.qrc)

add_executable(qt
        WIN32
        flow/cv.h
        flow/until.h
        flow/runner.h
        flow/segment.h
        flow/cv.cpp
        flow/until.cpp
        flow/runner.cpp
        flow/segment.cpp
        task/battle.h
        task/battle.cpp
        util/state.h
        util/state.cpp
        main.h
        main.cpp
        util/emitter.h
        ico.o
        task/daily.h
        task/daily.cpp
        task/common.h
        task/common.cpp
        ${qrc_FILES}
        util/enum.h
        util/enum.cpp
        function/clickerDialog.h
        function/clickerDialog.cpp
        widget/buttonWithSetting.h
        widget/buttonWithSetting.cpp
        widget/labeledSpinBox.cpp
        widget/labeledSpinBox.h
        widget/labeledComboBox.h
        widget/labeledComboBox.cpp
        function/imageDialog.cpp
        function/imageDialog.h
        util/sleep.cpp
        util/sleep.h
        util/action.cpp
        util/action.h
)

target_link_libraries(qt ${OpenCV_LIBS})
target_link_libraries(qt Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(qt PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/dist")

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
