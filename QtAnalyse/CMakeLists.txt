cmake_minimum_required(VERSION 2.9)
project(qtanalyse)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

string(ASCII 27 Esc)
set(ColourReset "${Esc}[m")
set(ColourBold  "${Esc}[1m")
set(Red         "${Esc}[31m")
set(Green       "${Esc}[32m")
set(Yellow      "${Esc}[33m")
set(Blue        "${Esc}[34m")
set(Magenta     "${Esc}[35m")
set(Cyan        "${Esc}[36m")
set(White       "${Esc}[37m")
set(BoldRed     "${Esc}[1;31m")
set(BoldGreen   "${Esc}[1;32m")
set(BoldYellow  "${Esc}[1;33m")
set(BoldBlue    "${Esc}[1;34m")
set(BoldMagenta "${Esc}[1;35m")
set(BoldCyan    "${Esc}[1;36m")
set(BoldWhite   "${Esc}[1;37m")



if(DAQ_PROJECT_PATH)
	
else()
	if(DEFINED ENV{DAQ_PROJECT_PATH})
		
		set (LIB_QCUSTOMPLOT_PATH $ENV{DAQ_PROJECT_PATH}/Lib/libqcustomplot)
		set (LIB_SHM_PATH $ENV{DAQ_PROJECT_PATH}/libshm)
		set (LIB_DECODEFRAME_PATH $ENV{DAQ_PROJECT_PATH}/libframe)
		set (DAQ_PROJECT_PATH $ENV{DAQ_PROJECT_PATH})
	else()
		message(FATAL_ERROR  ${Red} ERROR "  DAQ PATH Not Defined " ${White})
	endif()
endif()

message( ${Green} STATUS "  DAQ PATH = " ${DAQ_PROJECT_PATH} ${White})
link_directories(/usr/local/lib  /usr/lib/qwt ${DAQ_PROJECT_PATH}/build/lib)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_library (QWT_LIBRARIES NAMES qwt6 qwt6-qt5 qwt qwt-qt5)

include_directories(	${Qt5Widgets_INCLUDES} ./include/ $ENV{DAQ_PROJECT_PATH}/include  ${LIB_QCUSTOMPLOT_PATH}/src
							${LIB_DECODEFRAME_PATH}/include /usr/local/include /usr/local/qwt)

add_definitions(${Qt5Widgets_DEFINITIONS})

qt5_wrap_ui(UIS_HDRS
    src/mainwindow.ui
    src/dialogchcell.ui
)

set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

file (
	GLOB_RECURSE

	source_files
	src/*.c
	src/*.cpp
        
	include/*.h     
	../include/*.h
)


add_executable(
	qtanalyse 
	${source_files}
	${UIS_HDRS}
)

target_link_libraries(qtanalyse ${Qt5Widgets_LIBRARIES} Qt5::PrintSupport frame ${QWT_LIBRARIES})




