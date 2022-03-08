# create a new CMake project
cmake_minimum_required(VERSION 3.14.0)
project(custom_folder_syntacts VERSION 1.0.0)# must be the name of the overarching folder

# clone libraries in-place and build
include(FetchContent) 
# cloning MAHI GUI
FetchContent_Declare(mahi-gui GIT_REPOSITORY https://github.com/mahilab/mahi-gui.git) 
FetchContent_MakeAvailable(mahi-gui)
# cloning MAHI Util
FetchContent_Declare(mahi-util GIT_REPOSITORY https://github.com/mahilab/mahi-util.git)
FetchContent_MakeAvailable(mahi-util)
# # cloning Syntacts 
# FetchContent_Declare(syntacts GIT_REPOSITORY https://github.com/mahilab/Syntacts.git)
# FetchContent_MakeAvailable(syntacts)

include_directories(include)

# Syntacts 
# find_package(Syntacts REQUIRED) # find Syntacts from C:\Program Files x86
set(SYNTACTS_BUILD_GUI OFF CACHE BOOL "" FORCE)
set(SYNTACTS_BUILD_C_DLL OFF CACHE BOOL "" FORCE)
set(SYNTACTS_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SYNTACTS_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(syntacts GIT_REPOSITORY https://github.com/mahilab/Syntacts.git) 
FetchContent_MakeAvailable(syntacts)

# create an executable from main.cpp and link it to wanted libraries
# add_executable(myApp src/main.cpp)
# target_link_libraries(myApp syntacts)
# target_link_libraries(myApp mahi::util)
# target_link_libraries(myApp mahi::gui)

# create an executable from majmin_ampdur.cpp and link it to wanted libraries
add_executable(exp1 src/majmin_ampdur.cpp)
target_link_libraries(exp1 syntacts)
target_link_libraries(exp1 mahi::util)
target_link_libraries(exp1 mahi::gui)

# Also makes sure to read from test/CMakeLists.txt
add_subdirectory(test)
