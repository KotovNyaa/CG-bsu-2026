cmake_minimum_required(VERSION 3.16)
project(ClipLab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(ClipLab
    main.cpp
    resources.qrc
    core/Types.h
    core/Ops.h
    core/Ops.cpp
    core/Io.h
    core/Io.cpp
    controller/Controller.h
    controller/Controller.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
    ui/widgets/common/Combo.h
    ui/widgets/common/Combo.cpp
    ui/widgets/top/Top.h
    ui/widgets/top/Top.cpp
    ui/widgets/status/Status.h
    ui/widgets/status/Status.cpp
    ui/widgets/canvas/Canvas.h
    ui/widgets/canvas/Canvas.cpp
    ui/widgets/side/Side.h
    ui/widgets/side/Side.cpp
)

target_include_directories(ClipLab PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ClipLab PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
