# [Choice] Ubuntu version (use jammy on local arm64/Apple Silicon): jammy, focal
ARG VARIANT="jammy"
FROM buildpack-deps:${VARIANT}-curl

LABEL dev.containers.features="common"


######################## Install Libraries ########################
SHELL ["/bin/bash", "-c"] 
RUN \
    apt-get update && \
    export DEBIAN_FRONTEND=noninteractive && \
    apt-get -y install \
        sudo cmake bison build-essential clang flex gperf git gdb \
        libatspi2.0-dev libbluetooth-dev libclang-dev libcups2-dev libdrm-dev \
        libegl1-mesa-dev libfontconfig1-dev libfreetype6-dev \
        libgstreamer1.0-dev libhunspell-dev libnss3-dev libopengl-dev \
        libpulse-dev libssl-dev libts-dev libx11-dev libx11-xcb-dev \
        libxcb-glx0-dev libxcb-icccm4-dev libxcb-image0-dev \
        libxcb-keysyms1-dev libxcb-randr0-dev libxcb-render-util0-dev \
        libxcb-shape0-dev libxcb-shm0-dev libxcb-sync-dev libxcb-util-dev \
        libxcb-xfixes0-dev libxcb-xinerama0-dev libxcb-xkb-dev libxcb1-dev \
        libxcomposite-dev libxcursor-dev libxdamage-dev libxext-dev \
        libxfixes-dev libxi-dev libxkbcommon-dev libxkbcommon-x11-dev \
        libxkbfile-dev libxrandr-dev libxrender-dev libxshmfence-dev \
        libxshmfence1 llvm ninja-build nodejs python-is-python3 python3 \
        libzip-dev libxcb-cursor-dev libsm-dev libopenblas-dev

# download qt
RUN \
    cd /opt && \
    wget -q https://download.qt.io/official_releases/qt/6.6/6.6.1/single/qt-everywhere-src-6.6.1.tar.xz

# compile/install Qt
RUN \
    cd /opt && \
    tar xf qt-everywhere-src-6.6.1.tar.xz && \
    cd /opt/qt-everywhere-src-6.6.1 && \
    ./configure -prefix /usr/local/Qt6 -opensource -confirm-license -release -nomake tests -nomake examples && \
    cmake --build . --parallel 4 && \
    cmake --install . && \
    cd /opt && \
    rm -rf qt-everywhere-src-6.6.1 && \
    rm -rf qt-everywhere-src-6.6.1.tar.xz

# compile/install libcint
RUN \
    cd /opt && \
    git clone http://github.com/sunqm/libcint.git && \
    cd /opt/libcint && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/libcint -DBUILD_SHARED_LIBS=0 -DCMAKE_POSITION_INDEPENDENT_CODE=ON .. && \
    cmake --build . --target install && \
    rm -r /opt/libcint

# compile/install muparser
RUN \
    cd /opt && \
    git clone https://github.com/beltoforion/muparser.git && \
    cd /opt/muparser && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr/local/muparser -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON .. && \
    cmake --build . --target install && \
    rm -r /opt/muparser

# download eigen
RUN \
    cd /usr/local && \
    git clone https://gitlab.com/libeigen/eigen.git

# download vtk
RUN \
    cd /opt && \
    wget -q https://www.vtk.org/files/release/9.3/VTK-9.3.0.tar.gz && \
    tar -xvzf VTK-9.3.0.tar.gz

# compile/install VTK
RUN \
    cd /opt/VTK-9.3.0 && \
    mkdir build && \
    cd build && \
    cmake .. \
    -DBUILD_SHARED_LIBS:BOOL=OFF \
    -DCMAKE_BUILD_TYPE:STRING=Release \
    -DVTK_ENABLE_LOGGING:BOOL=OFF \
    -DVTK_ENABLE_WRAPPING:BOOL=OFF \
    -DVTK_QT_VERSION:STRING=6 \
    -DQT_QMAKE_EXECUTABLE:PATH=/usr/local/Qt6/bin/qmake \
    -DVTK_GROUP_ENABLE_Qt:STRING=YES \
    -DCMAKE_PREFIX_PATH:PATH=/usr/local/Qt6/lib/cmake \
    -DVTK_MODULE_ENABLE_VTK_GUISupportQtSQL:STRING=NO \
    -DVTK_MODULE_ENABLE_VTK_hdf5:STRING=NO \
    -DVTK_MODULE_ENABLE_VTK_GUISupportQtQuick:STRING=DONT_WANT \
    -DVTK_MODULE_ENABLE_VTK_RenderingContextOpenGL2:STRING=YES \
    -DVTK_MODULE_ENABLE_VTK_RenderingLICOpenGL2:STRING=DONT_WANT \
    -DVTK_MODULE_ENABLE_VTK_RenderingCellGrid:STRING=NO \
    -DVTK_MODULE_ENABLE_VTK_sqlite:STRING=NO \
    -DCMAKE_INSTALL_PREFIX=/usr/local/VTK && \
    cmake --build . -j4 && \
    cmake --install . && \
    rm -rf /opt/VTK-9.3.0 && \
    rm -rf /opt/VTK-9.3.0.tar.gz

# compile/install QuEST
RUN \
    cd /opt && \
    git clone https://github.com/quest-kit/QuEST.git && \
    cd /opt/QuEST && \
    mkdir build && \
    cd build && \
    cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON .. && \
    cmake --build . && \
    mkdir /usr/local/QuEST && \
    mkdir /usr/local/QuEST/lib && \
    cp QuEST/libQuEST.a /usr/local/QuEST/lib && \
    cp -r ../QuEST/include /usr/local/QuEST/include && \
    rm -rf /opt/QuEST