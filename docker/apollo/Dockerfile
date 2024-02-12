# Build and test Apollo image
# Get base image
FROM alexanderianblair/apollo-deps:master

# By default four cores are used to compile
ARG compile_cores=4

# By default checkout master branch
ARG build_git_sha=master

# By default we install everything under /opt
ARG WORKDIR=opt

# Get Apollo with Hephaestus
RUN cd /$WORKDIR && \
    git clone https://github.com/aurora-multiphysics/apollo && \
    cd apollo && \
    git checkout $build_git_sha && \
    git submodule update --init --recursive

# Build Hephaestus
RUN cd /$WORKDIR/apollo/contrib/hephaestus/ && \
    mkdir build && \
    cd build && \
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DMFEM_DIR=/$WORKDIR/mfem/build -DMFEM_COMMON_INCLUDES=/$WORKDIR/mfem/miniapps/common  .. && \
    make -j1

# Test Hephaestus
RUN cd /$WORKDIR/apollo/contrib/hephaestus/build && \
    make test

# Build Apollo
RUN cd /$WORKDIR/apollo && \
    make -j$compile_cores

# Test Apollo
RUN cd /$WORKDIR/apollo && \
    make test
