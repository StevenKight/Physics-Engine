# Analytic validation cases, run through sim_run(). Also registered as a test
# so the suite fails if the physics drifts from the expected results.
add_executable(validate validate.c)

target_link_libraries(validate PRIVATE logic_lib m)

add_test(NAME validation COMMAND validate)

# Each tick is a few microseconds of work for these small scenes, so a full
# OpenMP team per parallel region costs far more than it saves. Results are
# identical with any thread count.
set_tests_properties(validation PROPERTIES ENVIRONMENT OMP_NUM_THREADS=1)
