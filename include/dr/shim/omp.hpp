#pragma once

#ifdef _OPENMP
#include <omp.h>
#else

inline int omp_get_max_threads() { return 1; }

inline int omp_in_parallel() { return 0; }

inline int omp_get_num_threads() { return 1; }

inline int omp_get_thread_num() { return 0; }

#endif
