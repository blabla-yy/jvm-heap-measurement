
rm -rf ./build
rm -rf ./out
cmake -S . -B build
cmake --build build --config Release
ctest --test-dir build -C Release