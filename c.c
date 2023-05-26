#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
    int num = 0;
    scanf("%d", &num);

    printf("%d", num + atoi(argv[1]));
    FILE* file = fopen("c_res.txt", "w");
    fprintf(file, "%d", num + atoi(argv[1]));
    fclose(file);
    return 0;
}
