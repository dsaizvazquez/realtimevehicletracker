

add_library (projectionLib "projection.h" "projection.cpp")

target_link_libraries(projectionLib ${OpenCV_LIBS})
target_link_libraries(projectionLib ${OPENCV_LDFLAGS})
