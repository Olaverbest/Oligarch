g++ main.cpp src/interpreter.cpp src/stack.cpp src/logger.cpp -o oligarch -std=c++23 -Wall

if [ $? -eq 0 ]; then
    echo "Build succeeded: ./oligarch"
else
    echo "Build failed."
fi
