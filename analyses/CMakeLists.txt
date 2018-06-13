cmake_minimum_required(VERSION 2.8)
#Déclaration du projet

project(Analyse)


set (CMAKE_CXX_FLAGS "-std=c++11 -O3 -Wall -Wextra -Wno-unused-parameter -ggdb -g -pthread")
set (EXECUTABLE_OUTPUT_PATH bin)




#set (USERLIB tinyxml)

include_directories(./inc/ ../include)

link_directories()

#Configuration de l'édition de liens



file (
		GLOB_RECURSE

        source_files

        src/*.cpp
        
        include/*.h
        
        ../include/*.h
)

#Déclaration de l'exécutable

add_executable(

        analyse

        ${source_files}
)

target_link_libraries(

        analyse
        
        ${Libraries}
        ${USERLIB}


)
