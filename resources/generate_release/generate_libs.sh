#!/bin/bash

# This file is part of the QuickSleepWait distribution
# (https://github.com/peads/QuickSleepWait).
# Copyright (c) 2026 Patrick Eads.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, version 3.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.

readarray -d '' dlls < <(find . -type f -name *.dll -print0)

for dll in "${dlls[@]}"; do
    readarray -d '/' dir <<< ${dll}
    filename="${dll##*/}"
    filename="${filename%.*}"
    printf -v outDir "%s" "${dir[@]::${#dir[@]}-1}"
    outName="${outDir}${filename}"
    dumpbin -exports "${dll}" "-out:${outName}.dbin"
    sed -i '1,16d' "${outName}.dbin"
    head -n -8 "${outName}.dbin" > "${outName}.def"
    sed -i "s/^\s\+[0-9]\+\s\+[0-9A-F]\+\s\+[0-9A-F]\+\s\+\([^[:space:]]\+\).*$/\1/g" "${outName}.def"
    rm "${outName}.dbin"
    sed -i '1iEXPORTS' "${outName}.def"
    sed -i "s/^\s\+[0-9]\+\s\+[0-9A-F]\+\s\+\[NONAME\]//g" "${outName}.def"
    lib "-nologo" "-def:${outName}.def" "-out:${outName}.lib" "-machine:x64"
done

