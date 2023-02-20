
add_executable (detection_yolov5_image "detection_yolov5_image.cpp")
add_executable (detection_yolov5_video "detection_yolov5_video.cpp")
add_executable (detection_yolov5_stream "detection_yolov5_stream.cpp")
add_executable (KalmanFilter "kalmanFilter.cpp")


target_link_libraries(detection_yolov5_image ${OpenCV_LIBS})
target_link_libraries(detection_yolov5_image ${OPENCV_LDFLAGS})

target_link_libraries(detection_yolov5_video ${OpenCV_LIBS})
target_link_libraries(detection_yolov5_video ${OPENCV_LDFLAGS})

target_link_libraries(detection_yolov5_stream ${OpenCV_LIBS})
target_link_libraries(detection_yolov5_stream ${OPENCV_LDFLAGS})

target_link_libraries(KalmanFilter ${OpenCV_LIBS})
target_link_libraries(KalmanFilter ${OPENCV_LDFLAGS})