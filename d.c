#include <stdio.h>
#include <stdlib.h>


int main(int argc, char** argv) {
    int num = 0;
    scanf("%d", &num);

    FILE* file = fopen(argv[1], "w");
    fprintf(file, "%d", num * atoi(argv[2]));
    fclose(file);
    FILE* file2 = fopen("d_res.txt", "w");
    fprintf(file2, "%d", num + atoi(argv[2]));
    fclose(file2);
    
    return 0;
}
