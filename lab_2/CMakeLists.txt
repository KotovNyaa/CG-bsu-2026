cmake_minimum_required(VERSION 3.16)
project(ImageEditor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(ImageEditor
    main.cpp
    resources.qrc
    lib/ColorTypes.h
    lib/ColorMath.h
    lib/ColorMath.cpp
    core/Types.h
    core/Ops.h
    core/Ops.cpp
    controller/Controller.h
    controller/Controller.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
    ui/widgets/common/Combo.h
    ui/widgets/common/Combo.cpp
    ui/widgets/common/Slider.h
    ui/widgets/common/Slider.cpp
    ui/widgets/top/Top.h
    ui/widgets/top/Top.cpp
    ui/widgets/viewer/Viewer.h
    ui/widgets/viewer/Viewer.cpp
    ui/widgets/status/Status.h
    ui/widgets/status/Status.cpp
    ui/widgets/tools/Hist.h
    ui/widgets/tools/Hist.cpp
    ui/widgets/tools/Contrast.h
    ui/widgets/tools/Contrast.cpp
    ui/widgets/tools/Filter.h
    ui/widgets/tools/Filter.cpp
    ui/widgets/tools/Tools.h
    ui/widgets/tools/Tools.cpp
)

target_link_libraries(ImageEditor PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)