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
        ${qrc_FILES}

        util/tool.h
        util/tool.cpp
        util/state.h
        util/state.cpp

        component/unit/widTabWidget.h
        component/unit/widTabWidget.cpp
        component/unit/placeholderDelegate.h
        component/unit/placeholderDelegate.cpp

        component/panel/controller.h
        component/panel/controller.cpp
        component/panel/browser.h
        component/panel/browser.cpp
        component/panel/qqManger.h
        component/panel/qqManger.cpp
        component/panel/qqLoginBrowser.h
        component/panel/qqLoginBrowser.cpp

        main.h
        main.cpp
)
target_link_libraries(Red ${OpenCV_LIBS} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::WebEngineWidgets)
set_target_properties(Red PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set_target_properties(Red PROPERTIES OUTPUT_NAME 启动浏览器)