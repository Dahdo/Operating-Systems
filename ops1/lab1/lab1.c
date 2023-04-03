#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#define MAX_STR_LEN 150

#define ERR(source) (perror(source),fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

void dir_scan(char* path, char* text_arg)
{
    DIR *dirp;
	struct dirent *dir_ent;
	struct stat filestat;

	if (NULL == (dirp = opendir(path)))
		ERR("opendir");
	do {
		errno = 0;
		if ((dir_ent = readdir(dirp)) != NULL) {
			lstat(dir_ent->d_name, &filestat);
            
            if(text_arg[0] != '\0' && text_arg != NULL)
            {
                char tmp [MAX_STR_LEN];
                memcpy(tmp, dir_ent->d_name, strlen(text_arg));

                if(!strcmp(text_arg, tmp))
                    continue;
                else
                     printf("%s\n", dir_ent->d_name);
            }
            else
                printf("%s\n", dir_ent->d_name);
		}
	} while (dir_ent != NULL);

    if (errno != 0)
		ERR("readdir");

	if (closedir(dirp))
		ERR("closedir");
}


int main(int argc, char **argv)
{
int c;
char dir_arg[MAX_STR_LEN];
char text_arg[MAX_STR_LEN];

while ((c = getopt(argc, argv, "p:n:o:rs")) != -1)
{
    if(c == 'p')
        strcpy(dir_arg, optarg);
    else if(c == 'n')
        strcpy(text_arg, optarg);
}

    printf("path: %s\n", dir_arg);
    dir_scan(dir_arg, text_arg);

return EXIT_SUCCESS;
}