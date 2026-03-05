#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "exam.h"

// Helper to create a dummy file for tests
void make_file(char *name, char *content) {
    int fd = open(name, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, content, strlen(content));
        close(fd);
    }
}

int main(int argc, char *argv[]) {
	setvbuf(stdout, NULL, _IONBF, 0);
    if (argc < 2) return 1;
    char *test = argv[1];

    if (strcmp(test, "e1") == 0) {
        make_file("test_e1.txt", "apple banana apricot");
        int count = count_char("test_e1.txt", 'a');
        printf("RESULT:%d\n", count);
        unlink("test_e1.txt");
    }
    else if (strcmp(test, "e2") == 0) {
        // 10 bytes of junk, 5 bytes of target, then more junk
        make_file("test_e2.txt", "0123456789HELLOworld");
        int ret = read_middle_bytes("test_e2.txt");
        printf("\nRETURN:%d\n", ret);
        unlink("test_e2.txt");
    }
    else if (strcmp(test, "e3") == 0) {
        spawn_and_check();
    }
    else if (strcmp(test, "e4") == 0) {
        // We use "echo" and "SUCCESS" as our dynamic inputs
        run_custom_exec("echo", "SUCCESS");
    }
    else if (strcmp(test, "m1") == 0) {
        spawn_fleet(3);
    }
    else if (strcmp(test, "m2") == 0) {
        run_number_doubler(21);
    }
    else if (strcmp(test, "m3") == 0) {
        // Create a fake command that just prints "RELAY_WORKS"
        make_file("dummy_cmd", "#!/bin/bash\necho 'RELAY_WORKS'");
        system("chmod +x dummy_cmd");
        run_file_relay("./dummy_cmd", "relay_out.txt");
        unlink("dummy_cmd");
        unlink("relay_out.txt");
    }

    return 0;
}
