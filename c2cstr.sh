#!/bin/sh

echo "static char *bundle_str[] = {"

sed '
s/\\/\\\\/g
s/\\$//g
s/"/\\"/g
s/^/"/
s/$/",/
' "$@" |

grep -v '#include \\"xcc.h\\"'

echo "NULL};"
