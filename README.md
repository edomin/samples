This repository contain samples of using various thing than I want to test 
before using in my projects. The samples are quick, dirty and probably buggy. 
So they are not recommended to use in any project as is without corrections, 
refactoring etc.

Samples:

* [**posix_signal_handling**](https://github.com/edomin/samples/tree/master/posix_signal_handling) - 
attempt to implement signal handling with backtraces using addr2line utility. 
Based on 
[tutorial by Job Vranish](https://spin.atomicobject.com/2013/01/13/exceptions-stack-traces-c/) 
(Some code copypasted. Original code from that tutorial 
[released to public domain](https://gist.github.com/jvranish/4441299#gistcomment-1385251)).
Also implemented correct (relatively) output of line numbers of function loaded 
from shared object via dlopen.
* [**inih**](https://github.com/edomin/samples/tree/master/posix_signal_handling) - 
attempt to inplement library-like API for working with ini files using inih 
library. Sample consist: Ini structure with sections containing key-value data, 
addition and deletion sections and keys, access to sections and keys by index, 
counting sections in ini and keys in sections, possibility to use custom i/o 
functions for save/load ini data (for example if you use PhysFS library).

Source code of samples released to public domain (license CC0). 
Bundled third-party dependencies have their own licenses. Typically its' 
license texts can be found in "contrib/<third_party_name>" directory in files 
COPYING, LICENSE and so on. For example, license text for inih bundled with 
samples can be found in file "contrib/inih/LICENSE.txt".