file(GLOB SOURCES_FILE 
    "detection/*.cpp" 
    "detection/*.h"
) 

include_directories("utils")
include_directories("detection")



add_executable (main ${SOURCES_FILE} "main.cpp")


target_link_libraries(main ${OpenCV_LIBS})
target_link_libraries(main ${OPENCV_LDFLAGS})
