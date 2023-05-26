#include <stdlib.h>
#include <stdio.h>

int main() {
    printf("%d", 5);
    FILE* file = fopen("b_res.txt", "w");
    fprintf(file, "%d", 5);
    fclose(file);
    return 0;
}
