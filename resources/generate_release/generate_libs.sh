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

echo "${PWD}"
readarray -d '' dlls < <(find . -type f -name UE4SS.dll -print0)
readarray -d '' pdbs < <(find . -type f -name UE4SS.pdb -print0)

for dll in "${dlls[@]}"; do
    readarray -d '/' dir <<< ${dll}
    #outName="${dir[1]:0:-1}"
    printf -v outDir "%s" "${dir[@]::${#dir[@]}-1}"
    outName="${outDir}UE4SS"
    dumpbin -exports "${dll}" "-out:${outName}.dbin"
    sed -i '1,16d' "${outName}.dbin"
    head -n -8 "${outName}.dbin" > "${outName}.def"
    sed -i "s/^\s\+[0-9]\+\s\+[0-9A-F]\+\s\+[0-9A-F]\+\s\+\([^[:space:]]\+\).*$/\1/g" "${outName}.def"
    rm "${outName}.dbin"
    sed -i '1iEXPORTS' "${outName}.def"
    lib "-nologo" "-def:${outName}.def" "-out:${outName}.lib" "-machine:x64"
done

