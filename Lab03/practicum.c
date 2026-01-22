#include <stdio.h> //for printf(), sscanf()
#include <math.h> //for fabs()
#include <string.h> //for strcmp()

void round_one(void) {
	char* str = "User: roberto ID: 101";
	char user[16];
	int id;

	sscanf(str, "User: %s ID: %d", user, &id);
	if (!strcmp(user, "roberto") && id == 101) {
		printf("Round one passed!\n");
	} else {
		printf("Round one failed!\n");
	}
}

void round_two(void) {
	char* str = "v15.5.0"; //format is vMAJOR.MINOR.INC
	int major, minor, inc;
	sscanf(str, "v%d.%d.%d", &major, &minor, &inc);
	if (major == 15 && minor == 5 && inc == 0) {
		printf("Round two passed!\n");
	} else {
		printf("Round two failed!\n");
	}
}

void round_three(void) {
	char* str = " Power: 89.5%";
	float val;
	sscanf(str, " Power: %f%%", &val);
	if (fabs(val) - fabs(89.5) <= 1e-9) {
		printf("Round three passed!\n");
	} else {
		printf("Round three failed!\n");
	}
}

int main(int argc, char* argv[]) {
	round_one();
	round_two();
	round_three();

	return 0;
}
