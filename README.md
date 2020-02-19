This repository contain samples of using various thing than I want to test 
before using in my projects.

Samples:

* [**posix_signal_handling**](https://github.com/edomin/samples/tree/master/posix_signal_handling) - 
attempt to implement signal handling with backtraces using addr2line utility. 
Based on 
[tutorial by Job Vranish](https://spin.atomicobject.com/2013/01/13/exceptions-stack-traces-c/) 
(Some code copypasted. Original pieces of code by Job Vanish 
[released to public domain](https://gist.github.com/jvranish/4441299#gistcomment-1385251)).
Also implemented correct (relatively) output of line numbers of function loaded 
from shared object via dlopen.

All code in this repo released to public domain (license CC0).