g++ main.cpp src/interpreter.cpp -o oligarch

if [ $? -eq 0 ]; then
    echo "Build succeeded: ./oligarch"
else
    echo "Build failed."
fi
