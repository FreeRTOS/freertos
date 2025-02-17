#!/bin/bash

# This script rewrites a given source in-pace such that the result can be
# processed by VeriFast. Each rewrite below concerns a specific construct
# VeriFast cannot handle. When VeriFast will be extended to handle a
# problematic construct we encountered, the corresponding rewirte below can be
# deleted.
#
# This scirpt expects the following arguments:
# $1 : The absolute path to the source file to be rewritten in place.
#
# Note: Callers are responsible to back up the rewritten source file beforehand.


SOURCE_FILE="$1"


# IMPORTANT:
# None of the provided regexes must contain the unescaped character '|'
#
# $1 : sed 'find' regex
# $2 : sed 'replace' regex
rewrite()
{
  FIND_REGEX=$1
  REPLACE_REGEX=$2
  echo "Rewrite pattern: \"$FIND_REGEX\" -> \"$REPLACE_REGEX\""
  sed -i "" "s|$FIND_REGEX|$REPLACE_REGEX|g" $SOURCE_FILE
  echo
}


echo "Commenting out line/file pragmas"
rewrite "^#" "// &"

echo "Fixing order of 'long', 'unsigned'"
echo "Reported issue 338:"
echo "https://github.com/verifast/verifast/issues/338"
rewrite "long unsigned int" "unsigned long int"

echo "Delete fixed-sized array typedefs"
echo "Reported issue 339:"
echo "https://github.com/verifast/verifast/issues/339"
rewrite "typedef .*\[[0-9]*\];" ""

echo "Delete attributes"
echo "Reported issue 340:"
echo "https://github.com/verifast/verifast/issues/340"
rewrite "__attribute__(([_a-z]*))" ""
# Note: `\s` or `:space:` not work on MacOs.
rewrite "__attribute__( ( [_a-z]* ) )" ""

echo "Delete void casts (used to suppress compiler warnings)"
echo "Reported issue 335"
echo "https://github.com/verifast/verifast/issues/335"
rewrite "( void ) memset" "memset"

echo "Removing const qualifiers from pointers"
echo "Reported issue 333:"
echo "https://github.com/verifast/verifast/issues/333"
rewrite "[*] const" "*"
rewrite "const [*]" "*"

echo "Uncomment special includes to allow VeriFast proofs to refer to config macros"
rewrite "//VF_include #include" "#include"
rewrite "//VF_macro #" "#"
