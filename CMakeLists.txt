cmake_minimum_required(VERSION 3.16)
project(HospitalQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets REQUIRED)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(QT_LIBS Qt5::Widgets)
else()
    set(QT_LIBS Qt6::Widgets)
endif()

add_executable(hospital_qt
    main.cpp
    mainwindow.h
    mainwindow.cpp
    patient.h
    patient.cpp
    doctor.h
    doctor.cpp
    appointment.h
    appointment.cpp
    billing.h
    billing.cpp
)

target_link_libraries(hospital_qt PRIVATE ${QT_LIBS})
