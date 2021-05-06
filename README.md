This repository contain samples of using various things that I want to test 
before using in my projects. The samples are quick, dirty and probably buggy. 
So they are not recommended to use in any project as is without corrections, 
refactoring etc.

Samples:

* [**awk**](https://github.com/edomin/samples/tree/master/awk) - I learning 
awk. I found some exercises in the network and trying to complete it.
Directory structure:
    - awk/numbers - simple exercises for processing 2d array of integer numbers.
    - awk/people - sample of parsing people data file
    - awk/exercism - exercises from 
[exercism.io](https://exercism.io/tracks/bash/exercises) adapted from bash to 
awk
* [**compound_literals**](https://github.com/edomin/samples/tree/master/compound_literals) - 
Test support of compound literals for initialization of struct members which 
are pointers. 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Usage: 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Compile with gcc and 
-std=c99 option:
```
        $ make c99
```
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Compile with gcc and 
-std=c89 option:
```
        $ make c89
```
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Compile with gcc and 
-std=c89 and -ansi options:
```
        $ make ansi
```
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Compile with gcc and 
-std=c99 and -Wpedantic options:
```
        $ make pedantic99
```
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Compile with gcc and 
-std=c89 and -Wpedantic options (must fail):
```
        $ make pedantic89
```
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Compile with tcc:
```
        $ make tcc
```
* [**inih**](https://github.com/edomin/samples/tree/master/inih) - 
attempt to inplement library-like API for working with ini files using inih 
library. Sample consist: Ini structure with sections containing key-value data, 
addition and deletion sections and keys, access to sections and keys by index, 
counting sections in ini and keys in sections, possibility to use custom i/o 
functions for save/load ini data (for example if you use PhysFS library).
* [**ketopt**](https://github.com/edomin/samples/tree/master/ketopt) - parsing 
commandline options with ketopt library.
* [**libsir**](https://github.com/edomin/samples/tree/master/libsir) - logging 
with libsir library.
* [**posix_signal_handling**](https://github.com/edomin/samples/tree/master/posix_signal_handling) - 
attempt to implement signal handling with backtraces using addr2line utility. 
Based on 
[tutorial by Job Vranish](https://spin.atomicobject.com/2013/01/13/exceptions-stack-traces-c/) 
(Some code copypasted. Original code from that tutorial 
[released to public domain](https://gist.github.com/jvranish/4441299#gistcomment-1385251)).
Also implemented correct (relatively) output of line numbers of function loaded 
from shared object via dlopen.
* [**sockets/time_client**](https://github.com/edomin/samples/tree/master/sockets/time_client) - 
incomplete sample of getting time using various protocols. Currently 
implemented TIME and DAYTIME protocols. TODO NTP.
* [**tasm**](https://github.com/edomin/samples/tree/master/tasm) - writing 
small programs with TASM while learning assembly for DOS. Not recommended for 
use as third-party code/snippet/library because writen by Assembly-noob. 
Directory structure:
    - tasm/scripts - some common scripts for building and running examples with 
DosBox
    - tasm/dos_input_hex - Example of input HEX byte in DL.
* [**unix/shell**](https://github.com/edomin/samples/tree/master/unix/shell) - 
envoking unix utils by typing its name in stdin. It is not complete shell 
because it can not do piping and can not process shell syntax.
* [**xlib**](https://github.com/edomin/samples/tree/master/xlib) - learning 
xlib. Currently incomplete sample.
* [**xmempool**](https://github.com/edomin/samples/tree/master/xmempool) - 
learning how to use xmempool library. Task - allocate memory for structure, 
then free memory and then check that structure freed.

Source code of samples released to public domain (license CC0).