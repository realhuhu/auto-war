cmake_minimum_required(VERSION 3.30)
project(Red)
add_definitions(-DUNICODE -D_UNICODE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "D:/Qt/5.15.2/msvc2019_64")

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network WebEngineWidgets REQUIRED)

set(OpenCV_DIR "D:/opencv/dist_vc/install")
include(D:/opencv/dist_vc/install/OpenCVConfig.cmake)
find_package(OpenCV REQUIRED)

qt5_add_resources(qrc_FILES resource/resource.qrc)

add_executable(Red
#        WIN32
        ${qrc_FILES}

        util/tool.h
        util/tool.cpp
        util/enum.h
        util/enum.cpp
        util/state.h
        util/state.cpp
        util/action.h
        util/action.cpp
        util/emitter.h
        util/emitter.cpp

        interceptor/link.h
        interceptor/link.cpp
        interceptor/storage.h
        interceptor/storage.cpp

        component/unit/labelSpinBox.h
        component/unit/labelSpinBox.cpp
        component/unit/labelComboBox.h
        component/unit/labelComboBox.cpp
        component/unit/labelTabWidget.h
        component/unit/labelTabWidget.cpp
        component/unit/buttonWithSetting.h
        component/unit/buttonWithSetting.cpp
        component/unit/placeholderDelegate.h
        component/unit/placeholderDelegate.cpp

        component/panel/qqManger.h
        component/panel/qqManger.cpp
        component/panel/qqBrowser.h
        component/panel/qqBrowser.cpp
        component/panel/redManager.h
        component/panel/redManager.cpp
        component/panel/redBrowser.h
        component/panel/redBrowser.cpp
        component/panel/redController.h
        component/panel/redController.cpp
        component/panel/redConfigurer.h
        component/panel/redConfigurer.cpp

        processor/cv.h
        processor/cv.cpp
        processor/env.h
        processor/env.cpp
        processor/until.h
        processor/until.cpp
        processor/runner.h
        processor/runner.cpp
        processor/segment.h
        processor/segment.cpp

        task/daily.cpp
        task/daily.h

        main.h
        main.cpp
)
target_link_libraries(Red ${OpenCV_LIBS} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::WebEngineWidgets)
set_target_properties(Red PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set_target_properties(Red PROPERTIES OUTPUT_NAME 启动多开器)