# Get base image
FROM idaholab/moose:latest

# By default one core is used to compile
ARG compile_cores=1

# By default checkout mfem-moose branch
ARG WORKDIR="opt"
ARG build_git_sha="master"
ARG hypre_git_sha="562c29a"

# Install apt dependencies
RUN apt-get update && DEBIAN_FRONTEND=noninteractive TZ=Etc/UTC apt-get install -y \
    clang-format-9 \
    curl \
    libboost-all-dev \
    libnetcdf-dev \
    libhdf5-mpich-dev \
    libssl-dev \
    xxd \
    libfontconfig1-dev \
    libfreetype6-dev \
    libsdl2-dev \
    libglew-dev \
    libglm-dev \
    libpng-dev \
    ninja-build

# Install CMake
RUN cd /$WORKDIR && \
    git clone https://github.com/Kitware/CMake.git && \
    cd CMake/ && \
    git checkout 78ae759 && \
    ./bootstrap --parallel="$compile_cores" && \
    make -j"$compile_cores" && \
    make install && \
    alias cmake='/$WORKDIR/CMake/bin/cmake'

# Install METIS 5.1.0
RUN cd /$WORKDIR && \
    git clone https://github.com/mfem/tpls.git && \
    mv tpls/metis-5.1.0.tar.gz ./ && \
    rm -rf tpls/ && \
    tar -zxvf metis-* && \
    cd metis-5.1.0 && \
    make config shared=1 && \
    make && \
    make install && \
    ln -s ../build/Linux-x86_64/libmetis/libmetis.so lib

# Install MUMPS
RUN cd /$WORKDIR && \
    git clone https://github.com/scivision/mumps && \
    cd mumps/ && \
    mkdir build && \
    cd build && \
    cmake -DBUILD_SHARED_LIBS=on -Dscotch=true -Dintsize64=false -Darith="s;d;c;z"  \
    -DCMAKE_INSTALL_PREFIX=/usr/local/ .. && \
    make -j"$compile_cores" && \
    make install

# Build MFEM and common miniapp
RUN cd /$WORKDIR && \
    git clone https://github.com/mfem/mfem.git && \
    cd mfem && \
    sed -i "s|list|# list|g" /opt/mfem/config/cmake/modules/FindNetCDF.cmake && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DMFEM_USE_MPI=YES -DMFEM_USE_METIS_5=YES -DMETIS_DIR=../../metis-5.1.0 \
    -DMFEM_USE_OPENMP=YES -DMFEM_THREAD_SAFE=NO -DMFEM_SHARED=YES -DCMAKE_CXX_FLAGS_DEBUG=-fPIC -DMFEM_USE_NETCDF=YES -DHDF5_DIR=/usr/lib/x86_64-linux-gnu/hdf5/mpich/ \
    -DCMAKE_POLICY_DEFAULT_CMP0057=NEW  &&\
    make -j"$compile_cores" && \
    cd miniapps/common && \
    make -j"$compile_cores"

# Build GLVis
RUN cd /$WORKDIR && \
    git clone https://github.com/GLVis/glvis.git && \
    cd glvis/ && \
    mkdir build && \
    cd build/ && \
    cmake -G Ninja -DMFEM_DIR=/$WORKDIR/mfem/build .. && \
    ninja

# Build Apollo with Hephaestus
RUN cd /$WORKDIR && \
    git clone https://github.com/aurora-multiphysics/apollo.git && \
    cd apollo && \
    git checkout "$build_git_sha" && \
    git submodule update --init --recursive && \
    cd contrib/hephaestus/ && \
    mkdir build && \
    cd build && \
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DMFEM_DIR=/$WORKDIR/mfem/build -DMFEM_COMMON_INCLUDES=/$WORKDIR/mfem/miniapps/common  .. && \
    make -j"$compile_cores" && \
    cd /$WORKDIR/apollo && \
    make -j"$compile_cores"
