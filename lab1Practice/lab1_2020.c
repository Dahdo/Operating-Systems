#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{
    FILE *file;
    if (strcmp(argv[1], "create") == 0)
    {
        struct stat filestat;
        lstat(argv[2], &filestat);
        if (S_ISREG(filestat.st_mode))
        {
            perror("file exists already!");
            exit(EXIT_FAILURE);
        }
        file = fopen(argv[2], "w+");
    }
    else if (strcmp(argv[1], "show") == 0)
    {
        struct stat filestat;
        lstat(argv[2], &filestat);
        if (S_ISREG(filestat.st_mode))
        {
            file = fopen(argv[2], "r");
            printf("Database filename: %s\n", argv[2]);
            while (!feof(file))
            {
                char line[65];
                fgets(line, 64, file);
                puts(line);
            }
        }
    }

    for (int i = 3; i < argc; i++)
    {
        fprintf(file, "%-64s\n", argv[i]);
    }
    fclose(file);
    return EXIT_SUCCESS;
}