

add_library (detectionLib "Detector.cpp" "Detector.h")

target_link_libraries(detectionLib ${OpenCV_LIBS})
target_link_libraries(detectionLib ${OPENCV_LDFLAGS})
