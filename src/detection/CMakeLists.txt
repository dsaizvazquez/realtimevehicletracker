

add_library (detectionLib "constants.h" "Detector.cpp" "Detector.h")

target_link_libraries(detectionLib ${OpenCV_LIBS})
target_link_libraries(detectionLib ${OPENCV_LDFLAGS})
