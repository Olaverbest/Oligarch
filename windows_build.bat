g++ main.cpp src\interpreter.cpp src\stack.cpp src\logger.cpp -o oligarch.exe -std=c++23 -Wall

IF %ERRORLEVEL% EQU 0 (
    ECHO Build succeeded: oligarch.exe
) ELSE (
    ECHO Build failed.
)
