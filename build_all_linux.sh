#!/bin/bash

PROJECT_ROOT="$(dirname "$(readlink -f "$0")")"
cd "$PROJECT_ROOT"

echo "====================================="
echo "[INFO] Building Elypso hub in Release mode..."
echo "====================================="
echo ""

bash "./build_linux_release.sh"
if [ $? -ne 0 ]; then
    echo "[ERROR] Release build failed."
    read -p "Press enter to continue..."
    exit 1
fi

echo ""
echo "====================================="
echo "[INFO] Building Elypso hub in Debug mode..."
echo "====================================="
echo ""

bash "./build_linux_debug.sh"
if [ $? -ne 0 ]; then
    echo "[ERROR] Debug build failed."
    read -p "Press enter to continue..."
    exit 1
fi

echo ""
echo "====================================="
echo "[SUCCESS] Finished building and installing Elypso hub!"
echo "====================================="
echo ""

read -p "Press enter to continue..."
exit 0
