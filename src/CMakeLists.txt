file(GLOB SOURCES_FILE 
    "detection/*.cpp" 
    "detection/*.h"
    "tracking/*.cpp" 
    "tracking/*.h"
    "tracking/hungarian/*.cpp" 
    "tracking/hungarian/*.h"
) 

include_directories("utils")
include_directories("detection")
include_directories("tracking")
include_directories("tracking/hungarian")




add_executable (main ${SOURCES_FILE} "main.cpp")


target_link_libraries(main ${OpenCV_LIBS})
target_link_libraries(main ${OPENCV_LDFLAGS})
