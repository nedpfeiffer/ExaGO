export MY_CLUSTER=ascent
export PROJ_DIR=/gpfs/wolf/proj-shared/csc359
source $PROJ_DIR/src/spack/share/spack/setup-env.sh
module purge
module load cuda/11.0.2
module use $PROJ_DIR/$MY_CLUSTER/Modulefiles/Core
module load exasgd-base
module load gcc-ext/7.4.0
module load spectrum-mpi-ext
module load openblas
module load cmake/3.18.2

# Dirty workaround to fix permissions errors
# see https://github.com/spack/spack/issues/17407
ls $PROJ_DIR/src/spack/var/spack/environments/*

spack env activate exago-v1-0-0-deps

export MY_PETSC_DIR=`spack location -i petsc`
export EXTRA_CMAKE_ARGS="$EXTRA_CMAKE_ARGS -DCMAKE_CUDA_ARCHITECTURES=70"
export EXTRA_CMAKE_ARGS="$EXTRA_CMAKE_ARGS -DEXAGO_TEST_WITH_BSUB=ON"

if [[ ! -f $builddir/nvblas.conf ]]; then
  cat > $builddir/nvblas.conf <<-EOD
NVBLAS_LOGFILE  nvblas.log
NVBLAS_CPU_BLAS_LIB $(spack location -i openblas)/lib/libopenblas.so
NVBLAS_GPU_LIST ALL
NVBLAS_TILE_DIM 2048
NVBLAS_AUTOPIN_MEM_ENABLED
EOD
  export NVBLAS_CONFIG_FILE=$builddir/nvblas.conf
fi