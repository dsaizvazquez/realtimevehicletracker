

include_directories("utils")
add_subdirectory("detection")
add_subdirectory("tracking")
add_subdirectory("udp")
add_subdirectory("projection")



add_executable (main ${SOURCES_FILE} "main.cpp")


target_link_libraries(main ${OpenCV_LIBS})
target_link_libraries(main ${OPENCV_LDFLAGS})
target_link_libraries(main yaml-cpp detectionLib trackingLib udpLib)