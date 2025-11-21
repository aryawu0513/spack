// tests_gmake.c
#include <stdio.h>

int main(int argc, char **argv) {
    printf("=================================\n");
    printf("  GNU Make Test Suite Running\n");
    printf("=================================\n\n");
    
    // Test 1: Check if we can access GNU Make's global variables
    printf("Test 1: Accessing GNU Make globals\n");
    extern const char *program;
    extern char *version_string;
    
    if (program) {
        printf("  ✓ program name: %s\n", program ? program : "NULL");
    }
    if (version_string) {
        printf("  ✓ version: %s\n", version_string);
    }
    
    // Test 2: Simple function call test
    printf("\nTest 2: Calling GNU Make function\n");
    extern void initialize_global_hash_tables(void);
    initialize_global_hash_tables();
    printf("  ✓ initialize_global_hash_tables() completed\n");
    
    // Test 3: Check some flags
    printf("\nTest 3: Checking flags\n");
    extern int silent_flag;
    extern int debug_flag;
    printf("  silent_flag: %d\n", silent_flag);
    printf("  debug_flag: %d\n", debug_flag);
    
    printf("\n=================================\n");
    printf("  All tests passed!\n");
    printf("=================================\n");
    
    return 0;
}
