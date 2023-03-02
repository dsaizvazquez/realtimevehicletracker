

add_library (trackingLib "KalmanTracker.h" "KalmanTracker.cpp" "TrackerHandler.h" "TrackerHandler.cpp" "hungarian/Hungarian.h" "hungarian/Hungarian.cpp" "udpThread/udpConn.cpp" "udpThread/udpConn.hpp")

target_link_libraries(trackingLib ${OpenCV_LIBS})
target_link_libraries(trackingLib ${OPENCV_LDFLAGS})
target_link_libraries(trackingLib detectionLib udpLib projectionLib)