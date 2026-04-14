#include <stdio.h>

int main() {

#ifdef __clang__
    printf("Compiled with Clang %d.%d.%d\n",
           __clang_major__,
           __clang_minor__,
           __clang_patchlevel__);
#elif defined(__GNUC__)
    printf("Compiled with GCC %d.%d.%d\n",
           __GNUC__,
           __GNUC_MINOR__,
           __GNUC_PATCHLEVEL__);
#elif defined(__INTEL_COMPILER)
    printf("Compiled with Intel Compiler %d\n",
           __INTEL_COMPILER);
#else
    printf("Unknown compiler\n");
#endif

    return 0;
}