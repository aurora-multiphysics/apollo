# Build and test Apollo image
# Get base image
FROM alexanderianblair/apollo-debug-deps:master

# By default one core is used to compile
ARG compile_cores=4

# By default checkout mfem-moose branch
ARG WORKDIR="opt"
ARG build_git_sha="master"

# Get Apollo with Hephaestus
RUN cd /$WORKDIR && \
    git clone https://github.com/aurora-multiphysics/apollo.git && \
    cd apollo && \
    git checkout "$build_git_sha" && \
    git submodule update --init --recursive

# Build Hephaestus
RUN cd /$WORKDIR/apollo/contrib/hephaestus/ && \
    mkdir build && \
    cd build && \
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug -DMFEM_DIR=/$WORKDIR/mfem/build -DMFEM_COMMON_INCLUDES=/$WORKDIR/mfem/miniapps/common  .. && \
    make -j1

# Test Hephaestus
RUN cd /$WORKDIR/apollo/contrib/hephaestus/build && \
    make test

# Build Apollo
RUN cd /$WORKDIR/apollo && \
    METHOD=dbg make -j"$compile_cores"

# Test Apollo
RUN cd /$WORKDIR/apollo && \
    METHOD=dbg make test
