#!/bin/bash
#SBATCH --nodes=1
#SBATCH --ntasks=20
#SBATCH --time=04:00:00
#SBATCH --mail-type=none
#SBATCH -p sapphire
#SBATCH -A UKAEA-AP001-CPU
#SBATCH --cpus-per-task=1
#SBATCH -o out_%j_%A_%a
#SBATCH --exclusive
. /etc/profile.d/modules.sh  

function load_modules() {
    module purge
    module load rhel8/default-icl cmake
    module load intel-oneapi-mkl/2022.1.0/intel/mngj3ad6
    module load gcc/11
    module load curl
    module load python/3.8
    module load ninja

    export STACK_SRC=`mktemp -d /tmp/moose_stack_src.XXXXXX`
    export WORKDIR=`pwd`
    export compile_cores=8
    export OMPI_MCA_mca_base_component_show_load_errors=0

    USER=`whoami`
    BUILD_DIR_NAME=${WORKDIR}

    ROOT_PATH=/home/${USER}/rds/rds-ukaea-ap001/${USER}
    BUILD_PATH=${ROOT_PATH}/${BUILD_DIR_NAME}
    USR_PATH=${BUILD_PATH}/usr

    HDF5_MAJ_VER=1.10
    HDF5_MIN_VER=10
    HDF5_DIR_NAME=hdf5
    HDF5_INSTALL_PATH=${WORKDIR}/${HDF5_DIR_NAME}

    PETSC_COMMIT=38aca504f6ea08cc814f159b2c9bcf837a5876f3
    PETSC_DIR_NAME=petsc
    PETSC_INSTALL_DIR=${BUILD_PATH}/petsc

    MOOSE_COMMIT=4e99faf9804480e7be302895ff9b8ded5b9944ea

    APOLLO_COMMIT=6d3f2edd1792e0538490231de12935b4b5d4ff24

    export PATH=${BUILD_PATH}:${PATH}

    cd ${WORKDIR}
    #Need to set some compiler flags via config file"
    echo "-std=c++17" >> icpx.cfg
    echo "-Wno-tautological-constant-compare" >> icpx.cfg
    export ICPXCFG=${WORKDIR}/icpx.cfg

    export CC=mpiicc 
    export CXX=mpiicpc
    export FC=mpiifort
    export F77=mpiifort

    export I_MPI_CC=icx
    export I_MPI_CXX=icpx
    export I_MPI_F90=ifx
    export I_MPI_FC=ifx
    export I_MPI_F77=ifx  
}

function _build_hdf5() {
    cd ${WORKDIR}
    mkdir -p ${HDF5_DIR_NAME} || { echo "Failed to create ${HDF5_DIR_NAME}" ; exit 1 ; }

    HDF5_MAJ_VER=1.10
    HDF5_MIN_VER=10
    HDF5_VER=${HDF5_MAJ_VER}.${HDF5_MIN_VER}
    echo "Downloading HDF5"
    curl -kLJO \
        https://support.hdfgroup.org/ftp/HDF5/releases/hdf5-${HDF5_MAJ_VER}/hdf5-${HDF5_VER}/src/hdf5-${HDF5_VER}.tar.gz \
        || { echo "Failed to download hdf5" ; exit 1 ; }
    tar -xf hdf5-${HDF5_VER}.tar.gz

    cd hdf5-${HDF5_VER}
    make clean
    ./configure --prefix=${HDF5_INSTALL_PATH} --enable-cxx --enable-fortran --enable-build-mode=production
    make install -j ${compile_cores}
    if [ $? -eq 2 ]; then
        echo "HDF5 Build failed"
        exit 1
    fi
    echo "HDF5 built" 
}

function _build_petsc() {
    cd $WORKDIR
    # echo "Downloading SuperLU_dist"
    # curl -kLJO https://github.com/xiaoyeli/superlu_dist/archive/refs/tags/v8.1.0.tar.gz
    # curl -kLJO https://github.com/xiaoyeli/superlu_dist/archive/refs/tags/v8.1.0.tar.gz
    # if [ -d "$WORKDIR/petsc" ] ; then
    #    return
    # fi
    mkdir -p petsc
    cd petsc
    curl -kL -O http://ftp.mcs.anl.gov/pub/petsc/release-snapshots/petsc-3.19.3.tar.gz
    tar -xf petsc-3.19.3.tar.gz -C .
    cd petsc-3.19.3
    ./configure \
    --with-cc=$CC --with-cxx=$CXX --with-fc=$FC -CXXPP=cpp \
		       --prefix=${WORKDIR}/${PETSC_DIR_NAME} \
		       --download-hypre=1 \
		       --with-shared-libraries \
		       --with-debugging=no \
		       --with-hdf5-dir=${WORKDIR}/${HDF5_DIR_NAME} \
		       --with-blaslapack-dir=${MKLROOT} \
		       --download-metis=1 \
		       --download-parmetis=1 \
		       --download-ptscotch=1 \
		       --download-mumps=1 \
		       --download-superlu_dist=${WORKDIR}/superlu_dist-8.1.0.tar.gz \
		       --download-scalapack=1 \
		       --download-slepc=1 \
		       --with-mpi=1 \
		       --with-cxx-dialect=C++17 \
		       --with-fortran-bindings=0 \
		       --with-sowing=0 \
		       --with-64-bit-indices \
		       --with-make-np=${SLURM_NTASKS} \
		       COPTFLAGS='-O3 -fno-slp-vectorize' \
		       CXXOPTFLAGS='-O3 -fno-slp-vectorize' \
               FOPTFLAGS='-O3 -fno-slp-vectorize' \
	           PETSC_DIR=`pwd` PETSC_ARCH=arch-linux-c-opt
    make
    make PETSC_DIR=${WORKDIR}/${PETSC_DIR_NAME}/petsc-3.19.3 PETSC_ARCH=arch-linux-c-opt install \
	|| { echo "Failed to build petsc" ; exit 1 ; }
    cd ..
    cd ..
    export PETSC_DIR=$WORKDIR/petsc
}

function build_moose() {
    export MOOSE_JOBS=32
    cd $WORKDIR 
    # if [ -d "$WORKDIR/moose" ] ; then
    #    return
    # fi
    #remove xdr requirement from libmesh build
#    _build_mpich33
    # load_modules
    #build_vtk_git
    cd $WORKDIR
    git clone https://github.com/idaholab/moose
    cd moose
    git checkout ${MOOSE_COMMIT} || { echo "Checkout failed" ; exit 1 ; }
    if [ ! -f "$WORKDIR/petsc/lib/libpetsc.so" ] ; then
      echo "PETSc Install Unsuccessful"
      return
    fi
    export PETSC_DIR=$WORKDIR/petsc
    export PETSC_ARCH=arch-linux-c-opt

    #libmesh configure fails with the llvm compilers so we set these back here
    export I_MPI_CXX=icpc
    export I_MPI_FC=ifort
    export I_MPI_F90=ifort
    export I_MPI_F77=ifort
    export I_MPI_C=icc

    # echo "diff --git a/scripts/update_and_rebuild_libmesh.sh b/scripts/update_and_rebuild_libmesh.sh
    # index e49933bb68..b4d4159843 100755
    # --- a/scripts/update_and_rebuild_libmesh.sh
    # +++ b/scripts/update_and_rebuild_libmesh.sh
    # @@ -157,6 +157,11 @@ if [ -z \"\$go_fast\" ]; then
    # SRC_DIR=\${SCRIPT_DIR}/../libmesh configure_libmesh \$DISABLE_TIMESTAMPS \\
    #                                                     \$VTK_OPTIONS \\
    #                                                     \$* | tee -a \"\$SCRIPT_DIR/\$DIAGNOSTIC_LOG\" || exit 1
    # +  export I_MPI_CXX=icpx
    # +  export I_MPI_FC=ifx
    # +  export I_MPI_F90=ifx
    # +  export I_MPI_F77=ifx
    # +  export I_MPI_C=icx
    # else
    # # The build directory must already exist: you can't do --fast for
    # # an initial build." > moose.patch

    # git apply moose.patch || { echo "Patch failed" ; exit 1 ; }

    ./scripts/update_and_rebuild_libmesh.sh \
        --with-cxx-std=2017 \
        --with-cc=$CC \
        --with-cxx=$CXX \
        --with-fc=$FC

    export I_MPI_CXX=icpx
    export I_MPI_FC=ifx
    export I_MPI_F90=ifx
    export I_MPI_F77=ifx
    export I_MPI_C=icx
    # TODO: remove xdr requirement from libmesh build

    ./configure --with-derivative-size=200 --with-ad-indexing-type=global
    METHODS='opt' ./scripts/update_and_rebuild_wasp.sh
    cd framework
    METHOD=opt make -j"$compile_cores"
    cd ..
    cd modules
    METHOD=opt make -j"$compile_cores"
    cd ..
    cd test
    METHOD=opt make -j"$compile_cores"
    ./run_tests -j"$compile_cores"
    cd ..
    cd ..
}

function build_gslib() {
    cd $WORKDIR
    if [ -d "$WORKDIR/gslb" ] ; then
       return
    fi
    git clone https://github.com/Nek5000/gslib.git
    cd gslib
    make CFLAGS='-O2 -fPIC'
}

function build_mfem() {
    cd $WORKDIR
    # if [ -d "$WORKDIR/mfem" ] ; then
    #    return
    # fi
    git clone https://github.com/Heinrich-BR/mfem.git
    cd mfem
    git checkout SubmeshBoundary
    sed -i "s|list|# list|g" $WORKDIR/mfem/config/cmake/modules/FindNetCDF.cmake
    mkdir build
    cd build
    echo "Building MFEM"
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
	    -DCMAKE_POSITION_INDEPENDENT_CODE=YES \
	    -DMFEM_USE_OPENMP=NO \
	    -DMFEM_THREAD_SAFE=NO \
        -DHYPRE_DIR=/$WORKDIR/petsc/ \
        -DMFEM_USE_LAPACK=YES \
        -DMFEM_USE_MPI=YES \
        -DMFEM_USE_METIS_5=YES \
        -DMETIS_DIR=/$WORKDIR/petsc/ \
        -DParMETIS_DIR=/$WORKDIR/petsc/ \
        -DMFEM_USE_SUPERLU=YES \
        -DSuperLUDist_DIR=/$WORKDIR/petsc/ \
        -DSuperLUDist_VERSION_OK=YES \
        -DMFEM_USE_NETCDF=YES \
        -DNETCDF_LIBRARIES=$WORKDIR/moose/libmesh/installed/lib/libnetcdf.so \
        -DNETCDF_INCLUDE_DIRS=$WORKDIR/moose/libmesh/contrib/netcdf/netcdf-c-4.6.2/include \
        -DHDF5_DIR=/$WORKDIR/${HDF5_DIR_NAME}/ \
        -DMFEM_USE_GSLIB=YES \
      	-DGSLIB_DIR=/$WORKDIR/gslib/build
    make -j"$compile_cores"
    cd miniapps/common
    make -j"$compile_cores"
}

function build_apollo() {
    cd $WORKDIR
    # if [ -d "$WORKDIR/apollo" ] ; then
    #    return
    # fi

    git clone https://github.com/aurora-multiphysics/apollo.git
    cd apollo
    git checkout ${APOLLO_COMMIT}
    git submodule update --init --recursive
    cd contrib/hephaestus/
    # git checkout master
    mkdir build
    cd build
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DMFEM_DIR=/$WORKDIR/mfem/build ..
    ninja
    cd /$WORKDIR/apollo
    make -j"$compile_cores"
}

load_modules
_build_hdf5
_build_petsc
build_moose
build_gslib
build_mfem
build_apollo



#libmesh -remove patch and enable xdr from moose/scripts/configure_libmesh
#hephaestus tests fail to build