#!/bin/bash
DIR="$(dirname "$(readlink -f "$0")")"
export LD_LIBRARY_PATH="$DIR/libs:$LD_LIBRARY_PATH"
exec "$DIR/KoScheduler" "$@"