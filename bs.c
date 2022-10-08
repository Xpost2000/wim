#include <stdio.h>
#include <stdlib.h>
asidoasioodiasoiasd
char* line = "repeat this line a lot!";

int main(void) {
    int line_size = sizeof(line);

    int desired_size             = 1024*1024*512; // 512 mb
    int amount_of_times_to_write = desired_size / line_size;

    FILE* out = fopen("bs.txt", "w+");

    for (int i = 0; i < amount_of_times_to_write; ++i) {
        fprintf(out, "%s\n", line);
    }

    fclose(out);
}

#include <stdio.h>
#include <stdlib.h>

char* line = "repeat this line a lot!";

int main(void) {
    int line_size = sizeof(line);

    int desired_size             = 1024*1024*512; // 512 mb
    int amount_of_times_to_write = desired_size / line_size;

    FILE* out = fopen("bs.txt", "w+");

    for (int i = 0; i < amount_of_times_to_write; ++i) {
        fprintf(out, "%s\n", line);
    }

    fclose(out);
}
#include <stdio.h>
#include <stdlib.h>

char* line = "repeat this line a lot!";

int main(void) {
    int line_size = sizeof(line);

    int desired_size             = 1024*1024*512; // 512 mb
    int amount_of_times_to_write = desired_size / line_size;

    FILE* out = fopen("bs.txt", "w+");

    for (int i = 0; i < amount_of_times_to_write; ++i) {
        fprintf(out, "%s\n", line);
    }

    fclose(out);
}
#include <stdio.h>
#include <stdlib.h>

char* line = "repeat this line a lot!";

int main(void) {
    int line_size = sizeof(line);

    int desired_size             = 1024*1024*512; // 512 mb
    int amount_of_times_to_write = desired_size / line_size;

    FILE* out = fopen("bs.txt", "w+");

    for (int i = 0; i < amount_of_times_to_write; ++i) {
        fprintf(out, "%s\n", line);
    }

    fclose(out);
}

