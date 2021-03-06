cmake_minimum_required (VERSION 2.6)

include_directories("${PROJECT_SOURCE_DIR}/src/"
                    "${PROJECT_SOURCE_DIR}/benchmarks/"
                    "${PROJECT_SOURCE_DIR}/ext")

set (CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

# Cuda Dependent Code
if    (CUDA_FOUND)
  include(FindCUDA)
  add_definitions(-DCUDA)
  set(BACKEND_LIBRARIES ${BACKEND_LIBRARIES} ${CUDA_LIBRARIES}
    ${CUDA_CUBLAS_LIBRARIES} ${CUDA_cusparse_LIBRARY} cuda_kernels)
endif (CUDA_FOUND)

# MKL Dependent Code
if    (MKL_FOUND)
  include(FindMKL)
  add_definitions(-DMKL)
  include_directories(${MKL_INCLUDE_DIR})
  set(BACKEND_LIBRARIES ${BACKEND_LIBRARIES} ${MKL_LIBRARIES}
    pthread dl ${MKL_LIBRARIES})
endif (MKL_FOUND)

if    (MPI_FOUND)
  set(BACKEND_LIBRARIES ${BACKEND_LIBRARIES} ${MPI_LIBRARIES})
endif (MPI_FOUND)

# Executables
if (EIGEN3_FOUND)
  include_directories(SYSTEM ${EIGEN3_INCLUDE_DIR})

  if(MATS_DATA)
    add_definitions(-DMATS_DATA=${MATS_DATA})
    add_executable(mats mats.cpp)
    target_link_libraries(mats ${BACKEND_LIBRARIES})
  endif(MATS_DATA)

  add_executable(matrix_vector_multiplication matrix_vector_multiplication.cpp)
  target_link_libraries(matrix_vector_multiplication ${BACKEND_LIBRARIES})

endif (EIGEN3_FOUND)
