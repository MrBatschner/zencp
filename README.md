# zencp - A command line utility for the Creative Nomad Zen player

## Foreword

This program has been written in the year 2005. I just bought my first MP3 player, a Creative Zen Touch and wanted to have a nice command line utility for it. Given that it is 2021 now, this code is more or less obsolete as I do not believe there are still Creative Zen MP3 players out there (although I still have mine somewhere in a box), but still, I would like to conserve this code somewhere. 

Everything that follows is the content of the original `README` file back from 2005. All the code and the comments in it also dates back to 2005. To my surprise, the program still builds today in 2021.

## What is zencp?

_zencp_ is a program that lets you copy MP3 files to a Creative Zen MP3 Player.

The Creative MediaVision software that came with the player was Windows only and to me, it was not really intuitive to use. So I was looking for a replacement that ideally would work on Linux. Of course, there is gnomad2 that would do the trick. However, what I was looking for was a sleak and simple tool for the command line instead of a fancy GUI. So in a nutshell, I wanted to have was a small program that can be used like cp.

Usage of `zencp` is quite straight forward. You just call `zencp` followed by the names of the MP3 files you wish to transfer and the rest should be done by the program.

## Disclaimer

_zencp_ is a quick and dirty piece of code that was written on several train rides. It works fine for me on my PC (AMD64) and my laptop (IA32) but I do not know if it will work for anybody else. It does not have any automated build mechanism like the autotools or something similar, nor does it have any good Makefile.

Therefore, **BEWARE**: If you want to use `zencp`, you do it at your own risk. I will not hold any responsibility for deleted MP3 files, broken Zen Players or corrupted hard drives (but I would like to mention that none of these ever happened to me).

## Compiling

Zencp is written in C and comes with a C++ wrapper for a function in id3lib that has not been made available for C by the id3lib guys. Thus you will need both, a working C compiler and a C++ compiler.

Zencp relies on two libraries: libnjb (>=2.0) and id3lib (>= 3.8). 

libnjb controls the communication to the MP3 player and implements the whole protocol stuff. Get it from http://libnjb.sourceforge.net/. You need the development files, too.

id3lib is used for ID3 tag handling since Creative players rely on that piece of information. Get it from http://id3lib.sourceforge.net/. You need the development files, too.

On Debian/Ubuntu, both libraries along with their development headers can be installed with:

    apt-get install libid3-dev libnjb-dev

As already mentioned, there is no automated build mechanism. There is no file discovery, no validation of library dependencies and no compiler check. There is a rudimental `Makefile` but you might need to adapt library and linker paths within.

To compile, type `make`. If everything works fine, you will get a `zencp` executable.

## Bugs

Plenty, probably. There are several TODOs and FIXMEs in the code and I am sure that I included several possibilities for null-pointers and leaks.
