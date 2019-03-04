# Building-openv-4.0.1-using-Ubuntu-16.04-and-vscode
Ubuntu 16.04와 vscode를 사용하여 opencv build 하기  
[여기](https://webnautes.tistory.com/1030?category=704653)를 참고

## 1. 설치 환경
* Ubuntu 16.04
* Visual Studio Code 1.31 (Extensions: C/C++, C++ Intellisense, CMake, CMake Tools, Korean Language Pack for Visual Studio Code, and Python)
* Cmake 3.5.1
* Anaconda with Python 3.7
* no CUDA

## 2. 기존 OpenCV 제거
기존 opencv가 설치되어 있다면 제거. 설치 여부는 다음 코드로 확인
```shell
~$ pkg-config --modversion opencv
```

설치되어 있는 opencv는 아래의 코드로 삭제
```shell
~$ sudo apt-get remove --purge libopencv*
~$ sudo apt-get autoremove
```

## 3. 주요 패키지 설치
Anaconda가 미리 설치되어 있는 경우에 qt가 포함되어 있을 가능성이 높기 때문에 qt가 아닌 gtk로 build
```shell
~$ sudo apt-get install libjpeg-dev libtiff5-dev libjasper-dev libpng12-dev \
libavcodec-dev libavformat-dev libswscale-dev libxvidcore-dev libx264-dev libxine2-dev \
libv4l-dev v4l-utils \
libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev \
mesa-utils libgl1-mesa-dri libqt4-opengl-dev \
libatlas-base-dev gfortran libeigen3-dev \
libgtk2.0-dev mesa-utils libgl1-mesa-dri libgtkgl2.0-dev libgtkglext1-dev
```

## 4. OpenCV 설치
OpenCV 4.0.1에 대한 소스코드를 다운받고 압축을 푼다. Extra module을 위해 opencv_contrib도 다운로드
```shell
~$ mkdir opencv && cd opencv
~/opencv$ wget -O opencv.zip https://github.com/opencv/opencv/archive/4.0.1.zip
~/opencv$ unzip opencv.zip
~/opencv$ wget -O opencv_contrib.zip https://github.com/opencv/opencv_contrib/archive/4.0.1.zip
~/opencv$ unzip opencv_contrib.zip
```

cmake를 이용하여 opencv 컴파일 설정. Python package들은 포함하지 않음.
```shell
~/opencv$ cd opencv-4.0.1 && mkdir build && cd build
~/opencv/opencv-4.0.1/build$ cmake \
-D CMAKE_BUILD_TYPE=RELEASE \
-D CMAKE_INSTALL_PREFIX=/usr/local \
-D WITH_TBB=OFF \
-D WITH_IPP=OFF \
-D WITH_1394=OFF \
-D BUILD_WITH_DEBUG_INFO=OFF \
-D BUILD_DOCS=OFF \
-D INSTALL_C_EXAMPLES=ON \
-D INSTALL_PYTHON_EXAMPLES=OFF \
-D BUILD_EXAMPLES=OFF \
-D BUILD_TESTS=OFF \
-D BUILD_PERF_TESTS=OFF \
-D WITH_QT=OFF \
-D WITH_GTK=ON \
-D WITH_OPENGL=ON \
-D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-4.0.1/modules \
-D WITH_V4L=ON  \
-D WITH_FFMPEG=ON \
-D WITH_XINE=ON \
-D BUILD_opencv_python2=OFF \
-D BUILD_opencv_python3=OFF \
-D BUILD_opencv_python_bindings_generator=OFF \
-D BUILD_NEW_PYTHON_SUPPORT=OFF \
-D OPENCV_GENERATE_PKGCONFIG=ON \
../
```

에러가 발생하지 않았다면 사용가능한 코어 수를 확인하고 컴파일 시작
```shell
~/opencv/opencv-4.0.1/build$ cat /proc/cpuinfo | grep processor | wc -l
~/opencv/opencv-4.0.1/build$ make -j4
~/opencv/opencv-4.0.1/build$ sudo make install
```

다음 명령을 입력하여 opencv path 설정
```shell
~$ sudo sh -c 'echo '/usr/local/lib' > /etc/ld.so.conf.d/opencv.conf'
~$ sudo ldconfig
~$ source ~/.bashrc
```

마지막으로 설치가 잘 되었는 지 확인하기 위해 다음 명령을 입력!
```shell
~$ pkg-config --modversion opencv4
~$ pkg-config --libs --cflags opencv4
```

## 5. VS Code에서 프로젝트 build
opencv의 동작확인을 위해 아래와 같은 코드 작성
```cpp
#include <iostream>
#include <opencv2/opencv.hpp>
 
int main()
{
    std::cout << "OpenCV Version : " << CV_VERSION << std::endl;
    cv::Mat img;
 
    img = cv::imread("cat.jpg");
    if (img.empty())
    {
        std::cout << "[!] You can NOT see the cat!" << std::endl;
        return -1;
    }
    cv::imshow("EXAMPLE01", img);
    cv::waitKey(0);
    cv::destroyWindow("EXAMPLE01");
    return 0;
}

```
 
VS Code에서 opencv code를 build하기 위해서는 CMakeLists.txt를 아래와 같이 추가한다.
```
project(opencv_test)

set (CMAKE_CXX_STANDARD 11)
cmake_minimum_required(VERSION 3.0)
find_package(OpenCV REQUIRED)
 
file(GLOB SOURCES  *.cpp)

SET(OpenCV_DIR /usr/local/lib/cmake/opencv4) 

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries( opencv_test ${OpenCV_LIBS} )
```

SET(OpenCV_DIR /usr/local/lib/cmake/opencv4)으로 opencv4 PATH 설정

추가로 tasks.json을 만들어 cmake build를 위한 코드 작성
```json
{
    "version": "2.0.0",
    "runner": "terminal",
    "type": "shell",
    "echoCommand": true,
    "presentation" : { "reveal": "always" },
    "tasks": [
          //C++ 컴파일
          {
            "label": "compile for C++",
            "command": "cd ${fileDirname} && cmake . && make",
            "group": "build",

            //컴파일시 에러를 편집기에 반영
            //참고:   https://code.visualstudio.com/docs/editor/tasks#_defining-a-problem-matcher

            "problemMatcher": {
                "fileLocation": [
                    "relative",
                    "${workspaceRoot}"
                ],
                "pattern": {
                    // The regular expression. 
                   //Example to match: helloWorld.c:5:3: warning: implicit declaration of function 'prinft'
                    "regexp": "^(.*):(\\d+):(\\d+):\\s+(warning error):\\s+(.*)$",
                    "file": 1,
                    "line": 2,
                    "column": 3,
                    "severity": 4,
                    "message": 5
                }
            }
        },
        // 바이너리 실행(Ubuntu)

        {

            "label": "execute",

            "command": "cd ${fileDirname} && ./${workspaceFolderBasename} ",

            "group": "test"

        }

    ]
}
```
다시 소스코드(main.cpp)로 돌아온 후, task build를 하면 아래와 같이 출력되며 Makefile과 opencv_test, CMakeCache.txt, cmake_install.cmake, CMakeFiles가 생성
```
-- The C compiler identification is GNU 5.4.0
-- The CXX compiler identification is GNU 5.4.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found OpenCV: /usr/local (found version "4.0.1") 
-- Configuring done
-- Generating done
-- Build files have been written to: /home/rubai/Desktop/Linux/opencv_test
Scanning dependencies of target opencv_test
[ 50%] Building CXX object CMakeFiles/opencv_test.dir/main.cpp.o
[100%] Linking CXX executable opencv_test
[100%] Built target opencv_test
```
![screenshot 2019-03-05 at 00 07 33](https://user-images.githubusercontent.com/42333563/53742071-24f05180-3edb-11e9-8d90-26ae7dbeeb39.jpg)

+ opencv_test 파일을 실행시켜 동작확인
![screenshot 2019-03-05 at 00 12 06](https://user-images.githubusercontent.com/42333563/53742156-549f5980-3edb-11e9-9ed1-49a2bc09a2c0.jpg)
