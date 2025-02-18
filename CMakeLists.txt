cmake_minimum_required(VERSION 3.27)
project(QT)
add_definitions(-DUNICODE -D_UNICODE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_PREFIX_PATH "D:/Qt/5.15.2/msvc2019_64")

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Network WebEngineWidgets REQUIRED)

set(OpenCV_DIR "D:/opencv/dist_vc/install")
include(D:/opencv/dist_vc/install/OpenCVConfig.cmake)
find_package(OpenCV REQUIRED)

qt5_add_resources(qrc_FILES resource/resource.qrc)

add_executable(AutoWarSimple
        WIN32
        ${qrc_FILES}
        resource/ico/simple/ico.o
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
        widget/panelWidget.h
        widget/panelWidget.cpp
        task/common.h
        task/common.cpp
        task/battle.h
        task/battle.cpp
        task/daily.h
        task/daily.cpp
        task/guildWar.h
        task/guildWar.cpp
        function/clickerDialog.h
        function/clickerDialog.cpp
        function/imageDialog.cpp
        function/imageDialog.h
        function/activityDialog.h
        function/activityDialog.cpp
        function/replaceDialog.h
        function/replaceDialog.cpp
        simple.h
        simple.cpp
)

target_link_libraries(AutoWarSimple ${OpenCV_LIBS} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network)
set_target_properties(AutoWarSimple PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}")
set_target_properties(AutoWarSimple PROPERTIES OUTPUT_NAME 启动)

add_executable(AutoWarBrowser
        WIN32
        ${qrc_FILES}
        resource/ico/browser/ico.o
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
        widget/panelWidget.h
        widget/panelWidget.cpp
        task/common.h
        task/common.cpp
        task/battle.h
        task/battle.cpp
        task/daily.h
        task/daily.cpp
        task/guildWar.h
        task/guildWar.cpp
        function/clickerDialog.h
        function/clickerDialog.cpp
        function/imageDialog.cpp
        function/imageDialog.h
        function/activityDialog.h
        function/activityDialog.cpp
        function/replaceDialog.h
        function/replaceDialog.cpp
        browser.h
        browser.cpp
)

target_link_libraries(AutoWarBrowser ${OpenCV_LIBS} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::Network Qt5::WebEngineWidgets)
set_target_properties(AutoWarBrowser PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set_target_properties(AutoWarBrowser PROPERTIES OUTPUT_NAME 启动浏览器)
