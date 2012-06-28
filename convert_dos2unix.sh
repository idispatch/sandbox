#!/bin/sh
find . \( -name '*.[ch]' -or -name '*.[hc]pp' -or -name '*.mk' \) -exec dos2unix {} \;

