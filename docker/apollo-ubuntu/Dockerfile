# To make use of multiple cores during the compile stages of the docker build
# docker build -t apollo-ubuntu --build-arg compile_cores=8 .

# To compile a particular git sha use
# docker build -t apollo-ubuntu --build-arg build_git_sha=${GITHUB_SHA} .

# Get MOOSE image
# FROM helenbrooks/moose-ubuntu
FROM herter4171/ubuntu-moose:latest

# By default one core is used to compile
ARG compile_cores=1

# By default checkout main branch
ARG WORKDIR="opt"
ARG build_git_sha="alexanderianblair/mfem-moose"
ARG hypre_git_sha="562c29a"

RUN apt-get update
RUN apt-get install libboost-all-dev -y
RUN apt-get install libnetcdf-dev -y

RUN cd /$WORKDIR && \
cd moose/framework/src/base && \
sed -i 's/(type.family == NEDELEC_ONE)/(type.family == NEDELEC_ONE||type.family == LAGRANGE_VEC)/g' Assembly.C

RUN cd /$WORKDIR && \
    git clone https://github.com/hypre-space/hypre.git && \
    cd hypre && \
    git checkout "$hypre_git_sha" && \
    git reset --hard && \
    cd src/ && \
    ./configure --disable-fortran --enable-shared --with-openmp && \
    make && \
    make install

RUN cd /$WORKDIR && \
    curl -O http://glaros.dtc.umn.edu/gkhome/fetch/sw/metis/metis-5.1.0.tar.gz && \
    tar -zxvf metis-* && \
    cd metis-5.1.0 && \
    make config shared=1 && \
    make && \
    make install && \
    ln -s ../build/Linux-x86_64/libmetis/libmetis.so lib

RUN cd /$WORKDIR && \
    git clone https://github.com/mfem/mfem.git && \
    cd mfem && \
    mkdir build && \
    cd build && \
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DMFEM_USE_MPI=YES -DMFEM_USE_METIS_5=YES -DMETIS_DIR=../../metis-5.1.0 -DMFEM_USE_OPENMP=YES -DMFEM_THREAD_SAFE=OFF -DMFEM_SHARED=YES -DCMAKE_CXX_FLAGS_RELEASE=-fPIC -DMFEM_USE_NETCDF=YES && \
    make && \
    cd miniapps/common && \
    make

RUN cd /$WORKDIR && \
    git clone https://github.com/aurora-multiphysics/apollo.git && \
    cd apollo && \
    git checkout "$build_git_sha" && \
    git submodule update --init --recursive && \
    cd contrib/hephaestus/ && \
    mkdir build && \
    cd build && \
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DMFEM_DIR=/$WORKDIR/mfem/build -DMFEM_COMMON_INCLUDES=/$WORKDIR/mfem/miniapps/common  .. && \
    make && \
    cd /$WORKDIR/apollo && \
    make -j"$compile_cores"