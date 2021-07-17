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
compliance / portability, and would prefer an unencumbered implementation.

If you require complicated argument parsing with long verbose options,
this is not the library for you. See below for some alternatives.


Implentation Notes
------------------

* There is no configuration option for a "static" internal linkage
  implementation of this library
* This library pollutes the global namespace
* You interact with getopt() via global variables
* The getopt() function is not re-entrant
* The library does not use any dynamic memory
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
* Arg_parser - C/C++ argument parsers that supports long options :
  http://www.nongnu.org/arg-parser/arg_parser.html
* boost::program_options - Boost library argument parser :
  https://www.boost.org/doc/libs/1_76_0/doc/html/program_options.html
* TCLAP - Template based C++ argument parser :
  http://tclap.sourceforge.net/
