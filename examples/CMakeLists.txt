
add_executable (detection_yolov5_image "yolo/detection_yolov5_image.cpp")
add_executable (detection_yolov5_video "yolo/detection_yolov5_video.cpp")
add_executable (detection_yolov5_stream "yolo/detection_yolov5_stream.cpp")
add_executable (KalmanFilter "kalman/kalmanFilter.cpp")

add_executable (udpClientTest "udpBasic/udpClientTest.cpp")
add_executable (udpServerTest "udpBasic/udpServerTest.cpp")
add_executable (tcpClientTest "udpBasic/tcpClientTest.cpp")

add_executable (udpConnTestS "udpConn/udpConnTestS.cpp")
add_executable (udpConnTestC "udpConn/udpConnTestC.cpp")

target_link_libraries(detection_yolov5_image ${OpenCV_LIBS})
target_link_libraries(detection_yolov5_image ${OPENCV_LDFLAGS})

target_link_libraries(detection_yolov5_video ${OpenCV_LIBS})
target_link_libraries(detection_yolov5_video ${OPENCV_LDFLAGS})

target_link_libraries(detection_yolov5_stream ${OpenCV_LIBS})
target_link_libraries(detection_yolov5_stream ${OPENCV_LDFLAGS})

target_link_libraries(KalmanFilter ${OpenCV_LIBS})
target_link_libraries(KalmanFilter ${OPENCV_LDFLAGS})

target_link_libraries(udpClientTest udpLib)
target_link_libraries(udpServerTest udpLib)
target_link_libraries(tcpClientTest udpLib)


target_link_libraries(udpConnTestS trackingLib)
target_link_libraries(udpConnTestC trackingLib)

