This repository contain samples of using various things that I want to test 
before using in my projects. The samples are quick, dirty and probably buggy. 
So they are not recommended to use in any project as is without corrections, 
refactoring etc.

Samples:

* [**awk**](https://github.com/edomin/samples/tree/master/awk) - I learning 
awk. I found some exercises in the network and trying to complete it.
Directory structure:
    - awk/numbers - simple exercises for processing 2d array of integer numbers.
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
* [**tasm**](https://github.com/edomin/samples/tree/master/tasm) - writing 
small programs with TASM while learning assembly for DOS. Not recommended for 
use as third-party code/snippet/library because writen by Assembly-noob. 
Directory structure:
    - tasm/scripts - some common scripts for building and running examples with 
DosBox
    - tasm/dos_input_hex - Example of input HEX byte in DL.

Source code of samples released to public domain (license CC0).