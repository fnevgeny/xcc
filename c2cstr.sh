#!/bin/sh

echo "static char *bundle_str[] = {"

sed '
s/\\/\\\\/g
s/\\$//g
s/"/\\"/g
s/^/"/
s/$/",/
' "$@"

echo "NULL};"
