/*
example.c
Example file showing use of "getop_p.h" getopt() function.
SPDX-License-Identifier: Unlicense OR 0BSD
*/

#define GETOPT_P_IMPLEMENTATION
#include "getopt_p.h"

#include <stdio.h>
#include <stdlib.h>

void usage_err (void);
void usage_full (void);

int main (int argc, char * argv[])
{
    opterr=0;
    char * opt_str = ":hva1f:";
    int c;
    while ((c = getopt(argc, argv, opt_str)) != -1) {
        switch (c) {
        case '?' :
            fprintf(stderr, "Error : unknown option '%c'\n", (char)optopt);
            usage_err();
            break;
        case ':' :
            fprintf(stderr, "Error : missing argument to option '%c'\n",
                (char)optopt);
            usage_err();
            break;
        case 'h' :
            usage_full();
            break;
        case 'v' :
            printf("Version 1.01\n");
            break;
        case 'a' :
            printf("You supplied the option flag 'a'\n");
            break;
        case '1' :
            printf("You supplied the option flag '1'\n");
            break;
        case 'f' :
            printf("You supplied the filename \"%s\"\n", optarg);
            break;
        default :
            fprintf(stderr, "UNKNOWN RETURN VALUE '%c'\n", (char)c);
        }
    }
    printf("\n");

    if (optind < argc) {
        printf ("non-option argv elements : ");
        while (optind < argc) {
            printf("%s ", argv[optind]);
            optind++;
        }
        printf ("\n");
    }

    exit(EXIT_SUCCESS);
}

void usage_err (void)
{
    fprintf(stderr, "Usage : example [-h] [-v] [-a] [-1] [-f <filename>] "
        "[non-option-arguments]\n");
    fprintf(stderr, "For help : example -h\n");
    return;
}

void usage_full (void)
{
    fprintf(stdout, "Usage : example [-h] [-v] [-a] [-1] [-f <filename>] "
        "[non-option-arguments]\n");
    fprintf(stdout, "    -h Display this help text\n");
    fprintf(stdout, "    -v Display the program version number\n");
    fprintf(stdout, "    -a Set the 'a' flag for the program\n");
    fprintf(stdout, "    -1 Set the '1' flag for the program\n");
    fprintf(stdout, "    -f Specify the filename to operate on\n");
    fprintf(stdout, "non-option-arguments : other arguments not "
        "parsed by getopt()\n");
    return;
}

