# create test executable and link it to needed libraries
add_executable(test_trial1 test_trial1.cpp ../src/Chord.cpp ../src/Note.cpp)
target_link_libraries(test_trial1 syntacts)
target_link_libraries(test_trial1 mahi::util)
target_link_libraries(test_trial1 mahi::gui)
