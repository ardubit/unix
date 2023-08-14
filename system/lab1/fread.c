#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main(int argc, char const *argv[])
{
    char *filename = "_in";
    FILE *file = fopen(filename, "rb");
    char buffer[4];
    
    if (file) {
        /* File was opened successfully. */
        /* Attempt to read */
        size_t fr;
        printf("sizeof %zd \n", sizeof *buffer);
        
        // read to, size of one element, how many elements to read, from
        while ((fr = fread(buffer, sizeof *buffer, 4, file)) == 4) {
            
            /* byte swap here */
            for (size_t i = 0; i < fr; i++)
            {
                printf("%c \n", buffer[i]);
            }
        }

        fclose(file);
    }

    return 0;
}


