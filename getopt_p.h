/*
getopt_p.h
==========

https://github.com/muspell/getopt_p

POSIX compliant getopt() as a single header file interface / implementation.

This software is intended to provide a POSIX compliant getopt() to platforms
that do not have it (Windows). On non-Windows platforms the header file
just includes the <unistd.h> implementation supplied by the platform.


License
-------

SPDX-License-Identifier: Unlicense OR 0BSD

This software is released to the public domain via "The Unlicense".
As an alternative, if the receipient of the software does not acknowledge
public domain software, the recipient may choose to use the software
under the terms of the "BSD Zero Clause License".

See the comment blocks at the end of the header file for the full text
of these licenses.


Library Usage
-------------

The library does not need installation or configuration.
It is suggested to simply copy (embed) the single "getopt_p.h" file
directly in to your source tree; it is small and free-standing.

The single header file can be included in multiple translation units.

Exactly one tranlsation unit must "#define GETOPT_P_IMPLEMENTATION"
before including the header file in order to define the actual
implementation.


"option string" Variants
------------------------

This getopt() implementation supports POSIX compliant option strings :
* Supports ':' following an option character to require an argument
* Supports ':' as the first character to return ':' for missing argument
* Does not support '-' as an option character (POSIX compliance)
* Does not support '::' optional-arguments to options (POSIX compliance)
* Does not support '+' as first character (already POSIX behaviour)
* Does not support '-' as first character (POSIX compliance)

The getopt() function does not permute argv; as per POSIX behaviour it
stops parsing argv at the first non-option argument.

See the POSIX documentation of getopt() for more detailed usage notes :
https://pubs.opengroup.org/onlinepubs/009696799/functions/getopt.html


Use Case
--------

The main use case is for porting / compiling unix-like utility programs
for a Windows platform. If this is a relevant use case for you, the MSYS2
development environment may also be of interest : https://www.msys2.org/

The other use case is a pragmatic programmer who needs a simple lightweight
argument parser, does not need complicated options, prefers standards
compliance / portability and would prefer an unencumbered implementation.

If you require complicated argument parsing with long verbose options,
this is not the library for you. See below for some alternatives.


Implentation Notes
------------------

* There is no configuration option for a "static" internal linkage
  implementation of the library
* The library pollutes the global namespace
* You interact with getopt() via global variables
* The getopt() function is not re-entrant
* The library does not use any dynamic memory
* Any returned strings are pointers into the existing argv string
* The code compiles cleanly at high warning levels


Rationale for Implementation
----------------------------

The POSIX standard <unistd.h> getopt() operates via global variables.
Conforming to standards enhances portability.
Documentation for the POSIX standard getopt() is available at :
https://pubs.opengroup.org/onlinepubs/009696799/functions/getopt.html

Namespace pollution is small and controlled so typically is not a problem.

Argument processing is typically performed once at program initialisation,
so the lack of re-entrancy is not an issue for common use cases.


History
-------
Based on the AT&T Public Domain getopt() ditributed at the 1985 UNIFORUM
conference in Dallas.

The source was posted to newsgroup mod.std.unix on 1985-11-03 :
https://www.linux.co.cr/unix-source-code/review/1985/1103.html

* Updated to ANSI C
* Ported to the Windows platform
* Modified for POSIX compliant behaviour
* Modified to a "single header file" style C library


Alternative Argument Parsers
----------------------------
* parg - Lightweight C argument parser :
  https://github.com/jibsen/parg
* Arg_parser - C/C++ argument parsers that support long options :
  http://www.nongnu.org/arg-parser/arg_parser.html
* boost::program_options - Boost library argument parser :
  https://www.boost.org/doc/libs/1_76_0/doc/html/program_options.html
* TCLAP - Template based C++ argument parser :
  http://tclap.sourceforge.net/

 */



/* ****************************************************************************/
/* Header File Section                                                        */
/* ****************************************************************************/
#ifndef GETOPT_P_H_INCLUDED
#define GETOPT_P_H_INCLUDED

#ifndef _WIN32
#include <unistd.h>	        /* Not Windows - use platform implementation */
#else /* #ifndef _WIN32 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


extern const char * optarg; /* Pointer in to argv to return option argument */
extern int optind;          /* Index in argv of next element to be processed */
extern int opterr;          /* Flag to indicate if getopt() prints errors */
extern int optopt;          /* Variable to return erroneous option character */

int getopt (int argc, char * const argv[], const char * opt_str);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifndef _WIN32 */

#endif /* #ifndef GETOPT_P_H_INCLUDED */



/* ****************************************************************************/
/* Implementation Section                                                     */
/* ****************************************************************************/

#ifdef GETOPT_P_IMPLEMENTATION

/* Only build anything on Windows */
#ifdef _WIN32

#include <windows.h>			/* _get_pgmptr */
#include <string.h>				/* strcmp, strchr, strrchr */
#include <stdio.h>				/* For printing errors (if opterr==1) */
#include <stddef.h>				/* NULL pointer */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/* Global variables controlling the state of parsing. */
const char * optarg = NULL; /* Pointer into argv to returns option argument */
int optind = 1;             /* Index in argv of next element to be processed */
int opterr = 1;             /* Flag to indicate if getopt() prints errors */
int optopt = (int)'?';      /* Variable to return erroneous option character */

/* Utility functions are static (internal linkage). */
static void getopt_p_print_err (const char * opt_str, const char * msg,
    int option_char);

/* Constants for return values in error states (internal linkage). */
static const int getopt_p_option_unknown = (int)'?';
static const int getopt_p_option_missing = (int)':';


int getopt (int argc, char * const argv[], const char * opt_str)
{
    static int arg_idx = 0; /* Character index into current argv entry */

    optarg = NULL;          /* Default to no (empty) argument to option */

    /* If starting a new argv, check if we already parsed all the options */
    if (arg_idx == 0) {
        if (optind >= argc ||           /* No more entries in argv */
            argv[optind] == NULL ||     /* Null pointer in argv vector */
            argv[optind][0] != '-' ||   /* First non-option in argv */
            strcmp(argv[optind], "-") == 0) {   /* POSIX compliance */
            return (int)-1;             /* Return "parsing complete" */
        }
        if (strcmp(argv[optind], "--") == 0) {  /* End of options */
            optind++;                   /* Finished this argv entry, move on */
            return (int)-1;             /* Return "parsing complete" */
        }
        arg_idx++;                      /* Advance index to option character */
    }

    /* Get option character from argv entry */
    int c = argv[optind][arg_idx];  /* Character to consider as an option */
    optopt = c;

    /* Check if current option character is one that was specified */
    const char * cp = strchr(opt_str, (int)c);  /* Ptr to option in opt_str */
    if (c == ':' || cp == NULL) {
        getopt_p_print_err(opt_str, "invalid option", c);
        arg_idx++;
        if (argv[optind][arg_idx] == '\0') {
            optind++;       /* Finished this argv entry, move on */
            arg_idx = 0;    /* Reset to look at start of next argv entry */
        }
        return getopt_p_option_unknown;
    }

    /* Check if this option is specified to require an argument */
    if (*(cp+1) == ':') {
        /* Option string specifies the option needs an argument */
        if (argv[optind][arg_idx+1] != '\0') {
            /* Argument for this option embedded within this argv entry */
            optarg = &argv[optind][arg_idx+1];
        } else if ((optind+1) < argc) {
            /* Argument for this option is in the next argv */
            optind++;       /* Advance to next argv to find the argument */
            optarg = argv[optind];
        } else {
            /* Argument for this option not in this argv and no more argv */
			getopt_p_print_err(opt_str, "argument required for option", c);
            optind++;       /* Finished this argv entry, move on */
            arg_idx = 0;    /* Reset to look at start of next argv entry */
            if (opt_str[0] == ':') {    /* POSIX compliant behaviour */
                return getopt_p_option_missing;
            } else {
                return getopt_p_option_unknown;
            }
        }
        optind++;           /* Finished this argv entry, move on */
        arg_idx = 0;        /* Reset to look at start of next argv entry */
    } else {
        /* No argument expected */
        arg_idx++;
        if (argv[optind][arg_idx] == '\0') {
            optind++;       /* Finished this argv entry, move on */
            arg_idx = 0;    /* Reset to look at start of next argv entry */
        }
        optarg = NULL;
    }

    /* Return the option character that we found */
    return c;
}


static void getopt_p_print_err (const char * opt_str, const char * msg,
    int option_char)
{
    /* Report the error, based on runtime configuration */
    if (opterr && (opt_str[0] != ':')) {
        /* Get the program name to use while reporting the error */
        char * name_ptr;    /* Pointer rather than buffer is OK */
        if (_get_pgmptr(&name_ptr) == 0) {
            const char * short_name = strrchr(name_ptr, (int)'\\');
            if (short_name) {
                name_ptr = (char *)(short_name+1);
            }
        } else {
            name_ptr = "Error";
        }
        /* Now report the error */
        (void)fprintf(stderr, "%s : %s '-%c'\n", name_ptr, msg,
            (char)option_char);
    }
    return;
}


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* #ifdef _WIN32 */

#endif /* #ifdef GETOPT_P_IMPLEMENTATION */



/* ****************************************************************************/
/* Full License Text                                                          */
/* ****************************************************************************/

/*
This software is released to the public domain via "The Unlicense".
As an alternative, if the receipient of the software does not acknowledge
public domain software, the recipient may choose to use the software
under the terms of the "BSD Zero Clause License".
 */

/*
The Unlicense

This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or distribute
this software, either in source code form or as a compiled binary, for
any purpose, commercial or non-commercial, and by any means.

In jurisdictions that recognize copyright laws, the author or authors of
this software dedicate any and all copyright interest in the software to
the public domain. We make this dedication for the benefit of the public
at large and to the detriment of our heirs and successors. We intend this
dedication to be an overt act of relinquishment in perpetuity of all
present and future rights to this software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

/*
BSD Zero Clause License

Copyright (C) 2021 by Mark Smith

Permission to use, copy, modify, and/or distribute this software for any
purpose with or without fee is hereby granted.

THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.
 */

/* END OF FILE */
