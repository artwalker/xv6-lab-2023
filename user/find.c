#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

#define NULL ((void *)0)

int match(char *, char *);

char *strpbrk(const char *s1, const char *s2)
{
    for (const char *p1 = s1; *p1 != '\0'; ++p1)
    {
        for (const char *p2 = s2; *p2 != '\0'; ++p2)
        {
            if (*p1 == *p2)
            {
                return (char *)p1;
            }
        }
    }
    return NULL;
}

void 
find(char *path, char *filename)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, O_RDONLY)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    switch (st.type)
    {
    case T_DEVICE:
    case T_FILE:
        if (strpbrk(filename, ".^$*+?()[{\\|") != NULL)
        {
            if (match(filename, de.name))
                fprintf(1, "%s\n", path);
        }
        else
        {
            if (strcmp(de.name, filename) == 0)
                fprintf(1, "%s\n", path);
        }
        break;

    case T_DIR:
        strcpy(buf, path);
        p = buf + strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de))
        {
            if (de.inum == 0 || strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (stat(buf, &st) < 0)
            {
                fprintf(1, "find: cannot stat %s\n", buf);
                continue;
            }
            if (st.type == T_DIR)
            {
                find(buf, filename);
            }
            else if (st.type == T_FILE)
            {
                if (strpbrk(filename, ".^$*+?()[{\\|") != NULL)
                {
                    if (match(filename, de.name))
                        fprintf(1, "%s\n", buf);
                }
                else
                {
                    if (strcmp(de.name, filename) == 0)
                        fprintf(1, "%s\n", buf);
                }
            }
        }
        break;
    }
    close(fd);
}

int 
main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "Usage: find <path> <filename>\n");
        exit(1);
    }
    find(argv[1], argv[2]);
    exit(0);
}

// Regexp matcher from Kernighan & Pike,
// The Practice of Programming, Chapter 9, or
// https://www.cs.princeton.edu/courses/archive/spr09/cos333/beautiful.html

int matchhere(char *, char *);
int matchstar(int, char *, char *);

int match(char *re, char *text)
{
    if (re[0] == '^')
        return matchhere(re + 1, text);
    do
    { // must look at empty string
        if (matchhere(re, text))
            return 1;
    } while (*text++ != '\0');
    return 0;
}

// matchhere: search for re at beginning of text
int matchhere(char *re, char *text)
{
    if (re[0] == '\0')
        return 1;
    if (re[1] == '*')
        return matchstar(re[0], re + 2, text);
    if (re[0] == '$' && re[1] == '\0')
        return *text == '\0';
    if (*text != '\0' && (re[0] == '.' || re[0] == *text))
        return matchhere(re + 1, text + 1);
    return 0;
}

// matchstar: search for c*re at beginning of text
int matchstar(int c, char *re, char *text)
{
    do
    { // a * matches zero or more instances
        if (matchhere(re, text))
            return 1;
    } while (*text != '\0' && (*text++ == c || c == '.'));
    return 0;
}
