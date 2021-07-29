#include <unistd.h> // For sysconf, _SC_PAGE_SIZE
#include <stdio.h>

long page_size_in_bytes() {
    return sysconf(_SC_PAGE_SIZE);
}

int main()
{
    printf("Page size = %ld bytes", page_size_in_bytes());
    return 0;
}
