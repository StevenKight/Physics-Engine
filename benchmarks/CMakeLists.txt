# One benchmark binary per gravity backend. bench_fortran and bench_cuda
# compile their own copy of gravity.c with a forced threshold; because that
# copy defines newtonian_gravity(), the linker never pulls the router version
# out of forces_lib. bench_router uses the library as normal.
set(GRAVITY_SOURCE ${CMAKE_SOURCE_DIR}/src/logic/forces/gravity.c)

add_executable(bench_router bench.cpp)
target_compile_definitions(bench_router PRIVATE BENCH_BACKEND="router")

add_executable(bench_fortran bench.cpp ${GRAVITY_SOURCE})
target_compile_definitions(bench_fortran PRIVATE BENCH_BACKEND="fortran"
    USE_CUDA GRAVITY_GPU_THRESHOLD=2147483647)

add_executable(bench_cuda bench.cpp ${GRAVITY_SOURCE})
target_compile_definitions(bench_cuda PRIVATE BENCH_BACKEND="cuda"
    USE_CUDA GRAVITY_GPU_THRESHOLD=0)

foreach(target bench_router bench_fortran bench_cuda)
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/src/logic)
    target_link_libraries(${target} PRIVATE logic_lib OpenMP::OpenMP_CXX)
endforeach()
