cmake_minimum_required(VERSION 3.16)
project(ColorConverter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(ColorConverter
    main.cpp
    resources.qrc
    lib/ColorTypes.h
    lib/ColorMath.h
    lib/ColorMath.cpp
    controller/Controller.h
    controller/Controller.cpp
    ui/MainWindow.h
    ui/MainWindow.cpp
    ui/widgets/common/InputRow.h
    ui/widgets/common/InputRow.cpp
    ui/widgets/common/SliderRow.h
    ui/widgets/common/SliderRow.cpp
    ui/widgets/common/BaseCard.h
    ui/widgets/common/BaseCard.cpp
    ui/widgets/models/RgbCard.h
    ui/widgets/models/RgbCard.cpp
    ui/widgets/models/CmykCard.h
    ui/widgets/models/CmykCard.cpp
    ui/widgets/models/HlsCard.h
    ui/widgets/models/HlsCard.cpp
    ui/widgets/top/CieDialog.h
    ui/widgets/top/CieDialog.cpp
    ui/widgets/top/Picker2D.h
    ui/widgets/top/Picker2D.cpp
    ui/widgets/top/PreviewBox.h
    ui/widgets/top/PreviewBox.cpp
    ui/widgets/top/TopPanel.h
    ui/widgets/top/TopPanel.cpp
)

target_link_libraries(ColorConverter PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
