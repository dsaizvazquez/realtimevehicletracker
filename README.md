# realTimeVehicleTracker

Repository containing the c++ code of the real time vehicle tracker. It is based on the SORT algorithm and uses YOLOv5 as it's neural network. It is meant to be used on a ec2 p2.xlarge instance with Ubuntu 22.04LTS.

For more information head to the [documentation](https://gitlab.dlr.de/saiz_da/documentation) (Still a WIP), based in obsidian.

## Bibliography

- https://github.com/mcximing/hungarian-algorithm-cpp/
- https://github.com/mcximing/sort-cpp
- https://www.kalmanfilter.net/default.aspx
- https://docs.opencv.org/3.4/dd/d6a/classcv_1_1KalmanFilter.html
- https://github.com/eminfedar/async-sockets-cpp
- https://github.com/JustasBart/yolov8_CPP_Inference_OpenCV_ONNX
- https://github.com/WongKinYiu/yolov7
- https://github.com/UNeedCryDear/yolov7-opencv-dnn-cpp
- https://img.ly/blog/ultimate-guide-to-ffmpeg/

## MPEGTS TO RTMP WORKS WITH SERVER
ffmpeg -i udp://127.0.0.1:8554 -vcodec libx264 -acodec aac -f flv rtmp://127.0.0.1:1935/in
ffmpeg -i udp://127.0.0.1:8555 -vcodec libx264 -acodec aac -f flv rtmp://127.0.0.1:1935/in1

ffmpeg -re -stream_loop -1 -i video4.mp4 -f mpegts udp://127.0.0.1:13001