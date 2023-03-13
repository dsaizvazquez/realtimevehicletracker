git clone https://github.com/gabime/spdlog.git
cd spdlog && mkdir build && cd build
cmake .. && make -j
sudo make install
cd ../..

sudo apt install -y gstreamer1.0-rtsp gstreamer1.0-plugins-ugly gdb



rm -rf spdlog

mkdir build
cd build
cmake ..
cmake --build .