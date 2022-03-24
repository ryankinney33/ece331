#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>

int main(int argc, char *argv[])
{
	struct passwd *usr;
	if (argc < 2) {
		// No username passed
		fprintf(stderr, "Usage: %s username\n", argv[0]);
		return 1;
	}

	usr = getpwnam(argv[1]);
	if (usr == NULL) { // error checking
		// either user not found or another error
		fprintf(stderr, "Error from getpwnam\n");
		return 1;
	}

	printf("GECOS: %s\nhome: %s\n", usr->pw_gecos, usr->pw_dir);
	return 0;
}
