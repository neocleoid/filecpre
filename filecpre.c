#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <inttypes.h>

#define BLOCK_SIZE 1024

typedef unsigned long long ui64;
typedef unsigned char bit;

static FILE *fp;

static unsigned char cache[BLOCK_SIZE];

char *read_string(FILE *f)
{
    ui64 i;
    char *s = (char *)malloc(sizeof(char) * 128);
    fread(&i, sizeof(ui64), 1, f);
    fread(s, sizeof(char), i, f);
    s[i] = '\0';
    return s;
}

void write_string(char *s, FILE *f)
{
    ui64 i = strlen(s);
    fwrite(&i, sizeof(ui64), 1, f);
    fwrite(s, sizeof(char), i, f);
}

ui64 read_ui64(FILE *f)
{
    ui64 i;
    fread(&i, sizeof(ui64), 1, f);
    return i;
}

void write_ui64(ui64 i, FILE *f)
{
    fwrite(&i, sizeof(ui64), 1, f);
}

void filecp(FILE *dest, FILE *src, long bytes)
{
    while (1) {
        memset(cache, '\0', BLOCK_SIZE);
        if (bytes < BLOCK_SIZE) {
            fread(cache, sizeof(bit), bytes, src);
            fwrite(cache, sizeof(bit), bytes, dest);
            break;
        } else {
            fread(cache, sizeof(bit), BLOCK_SIZE, src);
            fwrite(cache, sizeof(bit), BLOCK_SIZE, dest);
            bytes -= BLOCK_SIZE;
        }
    }
}

void execute()
{
    printf("%s\n", read_string(fp));
    printf("%s\n", read_string(fp));
    printf("%s\n", read_string(fp));
    printf("%s\n", read_string(fp));
    ui64 size = read_ui64(fp);
    printf("%llu\n", size);
    FILE *op = fopen("out.jpg", "wb+");
    filecp(op, fp, size);
}

ui64 file_size(char *file)
{
    struct stat info = { 0 };
    stat(file, &info);
    return info.st_size;
}

int main(int argc, char **argv)
{
    if (argc < 3)
        return 0;
    if (strcmp(argv[1], "gen") == 0) {
        fp = fopen(argv[2], "wb");
        write_string("filecpre", fp);
        write_string("v0.0.1", fp);
        write_string("NeoClear", fp);
        return 0;
    } else if (strcmp(argv[1], "append") == 0 || strcmp(argv[1], "add") == 0) {
        fp = fopen(argv[2], "a+");
        fseek(fp, 0, SEEK_END);
        for (int i = 3; i < argc; i++) {
            FILE *f = fopen(argv[i], "rb");
            ui64 size = file_size(argv[i]);
            write_string(argv[i], fp);
            write_ui64(size, fp);
            filecp(fp, f, size);
        }
        return 0;
    } else if (strcmp(argv[1], "info") == 0) {
        fp = fopen(argv[2], "rb");
        ui64 siz = file_size(argv[2]);
        printf("%s\n", read_string(fp));
        printf("%s\n", read_string(fp));
        printf("%s\n", read_string(fp));
        while (1) {
            printf("filename: %s -> ", read_string(fp));
            ui64 skip = read_ui64(fp);
            printf("size: %lu bit\n", skip);
            fseek(fp, skip, SEEK_CUR);
            if (ftell(fp) >= siz)
                break;
        }
        return 0;
    } else if (strcmp(argv[1], "extract") == 0) {
        fp = fopen(argv[2], "rb");
        ui64 siz = file_size(argv[2]);
        read_string(fp);
        read_string(fp);
        read_string(fp);
        while (1) {
            FILE *op = fopen(read_string(fp), "wb");
            ui64 size = read_ui64(fp);
            filecp(op, fp, size);
            if (ftell(fp) >= siz)
                break;
        }
        return 0;
    }
}