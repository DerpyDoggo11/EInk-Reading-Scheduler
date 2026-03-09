#!/usr/bin/env bash
set -e

echo "Building..."
meson compile -C build

echo "Cleaning old dist..."
rm -rf ./dist
mkdir -p ./dist/libs

echo "Copying binary..."
cp ./build/KoScheduler ./dist/

echo "Copying libraries..."
ldd ./build/KoScheduler | grep "=> /" | awk '{print $3}' | xargs -I{} cp {} ./dist/libs/

if [ ! -d "./dist/emulator" ]; then
    echo "Copying emulator data..."
    cp -r ./emulator ./dist/
fi

echo "Creating launcher script..."
cat > ./dist/run.sh << 'RUNEOF'
#!/bin/bash
DIR="$(dirname "$(readlink -f "$0")")"
export LD_LIBRARY_PATH="$DIR/libs:$LD_LIBRARY_PATH"
exec "$DIR/KoScheduler" "$@"
RUNEOF
chmod +x ./dist/run.sh

echo "Done. Run with: ./dist/run.sh"
