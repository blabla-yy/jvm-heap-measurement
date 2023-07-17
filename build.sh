rm -rf ./out
rm -rf ./build
cmake -S . -B build
cmake --build build --config Release