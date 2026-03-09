cd ..
cmake -S . -B cmake-build-release -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-build-release -j 8 --target pyBumbleBee
cd python
pytest test/