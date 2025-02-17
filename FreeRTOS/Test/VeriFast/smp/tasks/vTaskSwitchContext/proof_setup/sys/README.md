This directory contains a copy of the system header file `cdefs.h`.
The specific version does not matter to our proof, but he have to include it to suppress VeriFast errors.
We cannot put the system directory containing the header on VeriFast's include path because it contains the file `stdbool.h` which VeriFast cannot parse.

More specifically, VeriFast cannot parse the defines `#define false 0` and `#define true 1` contained in the header `stdbool.h`.
Therefore, by default, it skips all includes of `stdbool.h` and uses its built-in definitions of `true` and `false`. However, if we manually specify an include path (via VeriFast's `-I` option) that contains `stdbool.h`, this behaviour changes. It stops skipping these includes which leads to parse errors.
