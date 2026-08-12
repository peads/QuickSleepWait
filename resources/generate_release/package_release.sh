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

source /etc/profile
source ~/.bash_profile
export PATH="${PATH}:/c/Program Files/7-Zip"

DEPLOY_PATH="$(sed 's/^\([[:alpha:]]\):[\\\/]/\/\1\//g' <<< ${1,})"
OPWD="${PWD}"

cd "${DEPLOY_PATH}"

MD5="$(sed 's/^\([[:alnum:]]\+\)\s\+.\+$/\1/g' < <(md5sum QuickSleepWait/dlls/main.dll | tee QuickSleepWait/dlls/main.dll.md5))"
SHA="$(sha256sum QuickSleepWait/dlls/main.dll | tee QuickSleepWait/dlls/main.dll.sha256)"
GAMEPASS_NAME="QuickSleepWait-${MD5}.zip"

7z a -stl -spf2 -mx0 "${GAMEPASS_NAME}"  "QuickSleepWait/"
7z rn "${GAMEPASS_NAME}" "QuickSleepWait" "ue4ss\Mods\QuickSleepWait"

cd "${OPWD}"
