# Real Time Vehicle Tracker

Repository containing the c++ code of the real time vehicle tracker. It is based on the SORT algorithm and uses YOLOv7 as its neural network. Testing has been done on an ec2 p2.xlarge instance, an ec2 p3.xlarge instance, and a personal desktop PC with a NVIDIA 1060. All had Ubuntu 22.04LTS as their OS.

For installation, first compile the last version of OpenCV available with CUDA and CUdnn dependencies and then run the script/install.sh bash file. An installation tutorial for openCV can be found below in the ReadME. The tutorial is based on personal experience.



# Installing OpenCV on ec2 instance

Amazon Web Services EC2 has several instance types that could be used for GPU usage, but the cost per hour of the p3 and p4 server families might be too high. In that case, for a third of the cost one could use a p2 instance, which depending on the necessities might be enough. In my case, I was only using it to accelerate the processing speed of my application, which is heavily dependent on matrix multiplication. Even though it should've been a smooth transition I ended up having some issues with the installation, so here is a small tutorial. 

I installed **OpenCV 4.5 with CUDA integration in a p2.xlarge with Ubuntu 22.04**. 

## Installation

1. Installing CUDA
2. Installing cuDNN
3. Compile OpenCV
4. Link compiled files


For creating the instance I will reference [Amazon tutorials](https://docs.aws.amazon.com/AWSEC2/latest/UserGuide/EC2_GetStarted.html#ec2-launch-instance) which gives a great baseline to learn how to do it. If you have never used EC2 before, I recommend going through the tutorials and understanding the options before trying to install OpenCV, as it might not be the best option for your use case.

### Installing base packages

OpenCV has a lot of dependencies, so before starting it is necessary to install all the packages:

```bash
#Initial configurations
sudo apt update
sudo apt upgrade

# basic dependencies
sudo apt install -y build-essential cmake unzip pkg-config gcc g++ python3 python3-dev python3-numpy

# Dependencies for video
sudo apt install -y libavcodec-dev libavformat-dev libswscale-dev libavcodec-dev libavformat-dev \ 
libswscale-dev libavcodec-dev libavformat-dev libswscale-dev

# photo dependencies
sudo apt install -y libpng-dev libjpeg-dev libopenexr-dev libtiff-dev libwebp-dev

#other dependencies
sudo apt install -y libxvidcore-dev libx264-dev libopenblas-dev libatlas-base-dev liblapack-dev \ 
gfortran libhdf5-serial-dev python3-tk libgtk-3-dev 


sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev



#oneliner
sudo apt install -y build-essential cmake unzip pkg-config gcc g++ python3 python3-dev python3-numpy \ 
libavcodec-dev libavformat-dev libswscale-dev libavcodec-dev libavformat-dev libswscale-dev \
libavcodec-dev libavformat-dev libswscale-dev libpng-dev libjpeg-dev libopenexr-dev libtiff-dev \
libwebp-dev libxvidcore-dev libx264-dev libopenblas-dev libatlas-base-dev liblapack-dev gfortran \
libhdf5-serial-dev python3-tk libgtk-3-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev
```

### Installing CUDA

For CUDA first update the linux kernel headers:

```bash
sudo apt install linux-headers-$(uname -r)
```
Then it is necessary to install the drivers. The most reliable option I have found is using the `ubuntu-drivers` command, as it gives directly what drivers are usable. For the p2.xlarge the driver is the 470. I have used both the server and the non-server version and works fine. After installing the driver it is necessary to reboot. 

```bash 
# install latest driver tester
sudo apt install ubuntu-drivers-common

#execute
sudo ubuntu-drivers devices
##OUTPUT
== /sys/devices/pci0000:00/0000:00:1e.0 ==
modalias : pci:v000010DEd0000102Dsv000010DEsd0000106Cbc03sc02i00
vendor   : NVIDIA Corporation
model    : GK210GL [Tesla K80]
manual_install: True
driver   : nvidia-driver-470-server - distro non-free
driver   : nvidia-driver-390 - distro non-free
driver   : nvidia-driver-418-server - distro non-free
driver   : nvidia-driver-450-server - distro non-free
driver   : nvidia-driver-470 - distro non-free recommended
driver   : xserver-xorg-video-nouveau - distro free builtin


# install driver
sudo apt install nvidia-driver-470 # Choose the newest

sudo reboot
```

After rebooting, the nvidia suite should be installed, and with the command `nvidia-smi` the following screen should appear:

{{< figure src="/images/nvidia_smi.png" title="Nvidia-smi" >}}

Our CUDA toolkit is **11.4**

Then it is important to obtain the ARCH of the head to the [CUDA gpu list](https://developer.nvidia.com/cuda-gpus). It is different for each graphical card. For the Tesla K80 is 3.7, for the V100 7.0. Our graphics card ARCH is **3.7** then.

To find the Cuda version the best is to go to the [Cuda toolkit archive](https://developer.nvidia.com/cuda-toolkit-archive) and download the necessary installer. Installing is done by running the file. It will ask to accept an EULA and make sure it does not install an incompatible driver. For this case what I installed was the 11.4 CUDA Toolkit for Ubuntu 20.04, and it worked perfectly.

`wget https://developer.download.nvidia.com/compute/cuda/11.4.4/local_installers/cuda_11.4.4_470.82.01_linux.run`
`sudo sh cuda_11.4.4_470.82.01_linux.run`

After installing it the following lines have to be added to the `.bashrc` file. Make sure the CUDA version coincides with yours. 

```bash

# NVIDIA CUDA Toolkit
export CUDA_HOME=/usr/local/cuda-11.4
export LD_LIBRARY_PATH=${CUDA_HOME}/lib64:$LD_LIBRARY_PATH
export PATH=${CUDA_HOME}/bin:${PATH}
```

### Installing cuDNN

```bash
sudo apt install zlib1g

```
If you do not have an NVIDIA Developer Program account already, go ahead and create one at https://developer.nvidia.com/developer-program. It will be required to download cuDNN.

After joining the Developer Program, go to cuDNN's webpage at https://developer.nvidia.com/cudnn, click on Download cuDNN and login using your credentials.

Now, follow these steps:

Agree to the terms of the cuDNN Software License Agreement

Select the most suitable local installer, probably “Local Installer for Ubuntu20.04 x86_64 (Deb)” (compatible with Ubuntu 22.04)

Save it to your home directory and send it to the server via scp.

then execute:

```bash

sudo dpkg -i libcudnn8-dev_8.2.4.15-1+cuda11.4_amd64.deb 
sudo dpkg -i libcudnn8_8.2.4.15-1+cuda11.4_amd64.deb 

sudo apt update

```
Now the repositories should be added and installing the drivers is direct. It is important to set the versions correctly in the installation lines, so make sure before executing them it is all according to your installation:

```bash
sudo apt-get install libcudnn8=8.2.4.15-1+cuda11.4
sudo apt-get install libcudnn8-dev=8.2.4.15-1+cuda11.4
sudo apt-get install libcudnn8-samples=8.2.4.15-1+cuda11.4
```

A reboot might be useful to install everything. 

After this is all installed, it is finally the moment to start with OpenCV

### Installing virtualenv

Or so you thought! For the installation, it is necessary to create a virtual environment. In case of having it installed this step might not be necessary.

```bash
sudo apt install python3-pip
sudo pip install virtualenv virtualenvwrapper

```

Then we add the following lines to the bashrc:

```bash
## add to .bashrc
# virtualenv and virtualenv wrapper
export WORKON_HOME=$HOME/.virtualenvs
export VIRTUALENVWRAPPER_PYTHON=/usr/bin/python3
source /usr/local/bin/virtualenvwrapper.sh
```

And load them with `source .bashrc`.

Once loaded, we can create a virtualenv and install numpy:

```bash
mkvirtualenv opencv_cuda -p python3
pip install numpy

```


### Installing OpenCV

First we clone the repositories and change their folder names to make it easier afterwards. 

```bash
wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.5.5.zip
wget -O opencv.zip https://github.com/opencv/opencv/archive/4.5.5.zip
unzip opencv.zip
unzip opencv_contrib.zip
mv opencv-4.x/ opencv
mv opencv_contrib-4.x/ opencv_contrib
```

Once cloned, we can use cmake to create the compilation tree, the line that **WORKED FOR ME** is the following:
```bash

cd ~/opencv
mkdir build
cd build

cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local\
-D INSTALL_PYTHON_EXAMPLES=ON -D INSTALL_C_EXAMPLES=OFF \
-D OPENCV_ENABLE_NONFREE=ON -D WITH_CUDA=ON -D WITH_CUDNN=ON \
-D OPENCV_DNN_CUDA=ON -D ENABLE_FAST_MATH=1 -D CUDA_FAST_MATH=1 \
-D CUDA_ARCH_BIN=3.7 -D WITH_CUBLAS=1 -D OPENCV_EXTRA_MODULES_PATH=~/opencv_contrib/modules \
-D HAVE_opencv_python3=ON -D PYTHON_EXECUTABLE=~/.virtualenvs/opencv_cuda/bin/python \
-D BUILD_EXAMPLES=ON -D CUDA_TOOLKIT_ROOT_DIR=/usr/local/cuda -DWITH_LAPACK=OFF ..
```
Once this line is run, a very useful summary of the packages is printed on screen. The compilation is very long, so make sure all your necessary packages are activated, this includes obvious stuff like CUDA and cuDNN but also if you are using video ffmpeg is necessary. I also had some problems with python.  Make sure that the paths are correct, as it might create errors during compilation. If everything looks alright then it is time to compile

#### Compilation

To compile OpenCV I needed to downgrade the gcc version to 9. In case of failing with the gcc installed by default it is possible to downgrade it with:
```bash
# might need older gcc, 9 works fine.
sudo apt -y install gcc-9 g++-9
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-9 9
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-9 9

```

To compile and install the lines are:
```bash

make -j$(nproc)
sudo make install

```

Finally, to make sure the libraries are correctly linked, it is important to execute the following lines:

```bash
sudo /bin/bash -c 'echo "/usr/local/lib" >> /etc/ld.so.conf.d/opencv.conf'
sudo ldconfig
#Make sure the routes exist and the python version is correct for your system
ls -l /usr/local/lib/python3.6/site-packages/cv2/python-3.6
cd ~/.virtualenvs/opencv_cuda/lib/python3.6/site-packages/

##Next, create a sym-link to your virtual environment: ##
ln -s /usr/local/lib/python3.6/site-packages/cv2/python-3.6/cv2.cpython-36m-x86_64-linux-gnu.so cv2.so 
```

Once this is done it should be possible to use OpenCV CUDA and cuDNN functions, an easy test is to open a python terminal and executing:

```python
import cv2
print(dir(cv2.cuda))
```




## References:

I am obviously not the first to try to do this, and there are some amazing tutorials out there that might be useful for debugging it.

- https://medium.com/@pydoni/how-to-install-cuda-11-4-cudnn-8-2-opencv-4-5-on-ubuntu-20-04-65c4aa415a7b
- https://towardsdatascience.com/opencv-cuda-aws-ec2-no-more-tears-60af2b751c46
- https://towardsdev.com/installing-opencv-4-with-cuda-in-ubuntu-20-04-fde6d6a0a367
