#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))
#define MAX_PATH_LEN 100
ssize_t scan_dir(char *path)
{
    DIR *dirp;
    struct dirent *dir_ent;
    struct stat filestat;

    ssize_t tot_size = 0;

    if (NULL == (dirp = opendir(path)))
    {
        if (errno == EACCES)
        {
            fprintf(stderr, "No access to folder %s", path);
            return -1;
        }

        else
            ERR("opendir");
    }

    do
    {
        errno = 0;
        if ((dir_ent = readdir(dirp)) != NULL)
        {
            if (lstat(dir_ent->d_name, &filestat))
                ERR("lstat");

            if (strcmp(dir_ent->d_name, ".") && strcmp(dir_ent->d_name, ".."))
            {
                tot_size += filestat.st_size;
            }
        }
        if (errno)
            ERR("readdir");

    } while (dir_ent != NULL);

    if (closedir(dirp))
        ERR("closedir");

    return tot_size;
}

int main(int argc, char **argv)
{
    ssize_t size = -1;

    for (int i = 1; i < argc; i += 2)
    {
        size = strtol(argv[i + 1], (char **)NULL, 10);
        printf("\nscanning dir: %s\n", argv[i]);
        if (size < scan_dir(argv[i]))
        {
          
            FILE *afile;
            if ((afile = fopen("out.txt", "w+")) == NULL)
                ERR("fopen");
            fputs(argv[i], afile);
            if (fclose(afile))
                ERR("fclose");
     	}
    }
    return EXIT_SUCCESS;
}
