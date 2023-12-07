#!/bin/bash
#set -ue

export STACK_SRC=`mktemp -d /tmp/moose_stack_src.XXXXXX`
export WORKDIR=`pwd`
export compile_cores=32
export OMPI_MCA_mca_base_component_show_load_errors=0

function load_modules() {
    module purge
    module load rhel7/global dot slurm cmake-3.19.7-gcc-5.4-5gbsejo
    module load hdf5/openmpi/gcc/9.3/openmpi-4.0.4/1.12.0
    module load python/3.8
}

function build_petsc() {
    cd $WORKDIR
    curl -LJO https://github.com/xiaoyeli/superlu_dist/archive/refs/tags/v8.1.0.tar.gz
    if [ -d "$WORKDIR/petsc" ] ; then
       return
    fi
    mkdir petsc
    cd petsc
    curl -L -O http://ftp.mcs.anl.gov/pub/petsc/release-snapshots/petsc-3.19.3.tar.gz
    tar -xf petsc-3.19.3.tar.gz -C .
    cd petsc-3.19.3
    ./configure \
	--prefix=$WORKDIR/petsc \
        CXXOPTFLAGS='-O3 -march=cascadelake -funroll-loops' COPTFLAGS='-O3 -march=cascadelake -funroll-loops' FOPTFLAGS='-O3 -march=cascadelake' \
	--with-debugging=0 \
	--with-ssl=0 \
	--with-pic=1 \
	--with-openmp=1 \
	--with-mpi=1 \
	--with-shared-libraries=1 \
    --with-fortran-bindings=0 \
    --with-sowing=0 \
    --download-hypre=1 \
    --download-fblaslapack=1 \
    --download-metis=1 \
    --download-ptscotch=1 \
    --download-parmetis=1 \
    --download-superlu_dist=$WORKDIR/superlu_dist-8.1.0.tar.gz \
    --download-scalapack=1 \
    --download-mumps=1 \
    --download-slepc=1 \
    --with-64-bit-indices=1 \
    --with-mpi-dir=/usr/local/Cluster-Apps/openmpi/gcc/9.3/4.0.4 \
    PETSC_DIR=`pwd` PETSC_ARCH=arch-linux-c-opt
    make
    make PETSC_DIR=$WORKDIR/petsc/petsc-3.19.3 PETSC_ARCH=arch-linux-c-opt install
    make PETSC_DIR=$WORKDIR/petsc PETSC_ARCH="" check
    cd ..
    cd ..
    export PETSC_DIR=$WORKDIR/petsc
}

function build_moose() {
    export MOOSE_JOBS=32
    cd $WORKDIR
    if [ -d "$WORKDIR/moose" ] ; then
       return
    fi
#    _build_mpich33
    load_modules
    #build_vtk_git
    cd $WORKDIR
    git clone https://github.com/idaholab/moose
    cd moose
    git checkout master
    if [ ! -f "$WORKDIR/petsc/lib/libpetsc.so" ] ; then
      echo "PETSc Install Unsuccessful"
      return
    fi

    export PETSC_DIR=$WORKDIR/petsc
    export PETSC_ARCH=arch-linux-c-opt
    export CC=mpicc
    export CXX=mpicxx
    export F90=mpif90
    export F77=mpif77
    export FC=mpif90
    if [ -d "$WORKDIR/vtk" ] ; then
      echo "building libmesh with VTK"
      METHODS='opt' ./scripts/update_and_rebuild_libmesh.sh --with-mpi --with-cxx-std=2017 --with-vtk-include=$WORKDIR/vtk/include/vtk-9.1 --with-vtk-lib=$WORKDIR/vtk/lib64
    else
      echo "Building libmesh withOUT VTK"
      METHODS='opt' ./scripts/update_and_rebuild_libmesh.sh --with-mpi
    fi
    ./configure --with-derivative-size=200 --with-ad-indexing-type=global
    METHODS='opt' ./scripts/update_and_rebuild_wasp.sh
    cd framework
    METHOD=opt make -j32
    cd ..
    cd modules
    METHOD=opt make -j32
    cd ..
    cd test
    METHOD=opt make -j32
    ./run_tests -j32
    cd ..
    cd ..
}

function build_mfem() {
    cd $WORKDIR
    if [ -d "$WORKDIR/mfem" ] ; then
       return
    fi
    git clone https://github.com/mfem/mfem.git
    cd mfem
    git checkout master
    sed -i "s|list|# list|g" $WORKDIR/mfem/config/cmake/modules/FindNetCDF.cmake
    mkdir build
    cd build
    echo "Building MFEM"
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_CXX_FLAGS_RELEASE=-fPIC \
        -DMFEM_SHARED=YES \
        -DMFEM_USE_OPENMP=YES \
        -DMFEM_THREAD_SAFE=NO \
        -DHYPRE_DIR=/$WORKDIR/petsc/ \
        -DBLAS_LIBRARIES=/$WORKDIR/petsc/lib/libfblas.a \
        -DLAPACK_LIBRARIES=/$WORKDIR/petsc/lib/libflapack.a \
        -DMFEM_USE_MPI=YES \
        -DMFEM_USE_METIS_5=YES \
        -DMETIS_DIR=/$WORKDIR/petsc/ \
        -DParMETIS_DIR=/$WORKDIR/petsc/ \
        -DMFEM_USE_SUPERLU=YES \
        -DSuperLUDist_DIR=/$WORKDIR/petsc/ \
        -DMFEM_USE_NETCDF=YES \
        -DNETCDF_LIBRARIES=$WORKDIR/moose/libmesh/installed/lib/libnetcdf.so \
        -DNETCDF_INCLUDE_DIRS=$WORKDIR/moose/libmesh/contrib/netcdf/netcdf-c-4.6.2/include \
        -DHDF5_DIR=/usr/local/Cluster-Apps/hdf5/openmpi/gcc/9.3/1.12.0 \
        -DCMAKE_POLICY_DEFAULT_CMP0057=NEW
    make -j"$compile_cores"
    cd miniapps/common
    make -j"$compile_cores"
}

function build_apollo() {
    cd $WORKDIR
    if [ -d "$WORKDIR/apollo" ] ; then
       return
    fi

    git clone https://github.com/aurora-multiphysics/apollo.git
    cd apollo
    git checkout master
    git submodule update --init --recursive
    cd contrib/hephaestus/
    mkdir build
    cd build
    cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release -DMFEM_DIR=/$WORKDIR/mfem/build -DMFEM_COMMON_INCLUDES=/$WORKDIR/mfem/miniapps/common  ..
    make -j1
    cd /$WORKDIR/apollo
    make -j"$compile_cores"
}

load_modules
build_petsc
build_moose
build_mfem
build_apollo
