cd ..
cmake --build cmake-build-release -j 8 --target pyBumbleBee
cd python
pytest test/