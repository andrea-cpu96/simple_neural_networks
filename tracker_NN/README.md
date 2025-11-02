# Tracker

## Description
This system uses a neural network with a single neuron to determine the direction of a target. It is designed to be lightweight and efficient, suitable for embedded applications or simple directional tracking tasks.

## Compilation
To compile the project using GCC with warnings enabled, run the following command from the project root:

```bash
gcc -Wall -Wextra ./src/tracker.c ./src/NN_1N.c -o ./src/tracker.exe
```

## Execution
To execute the compiled program:

```bash
./src/tracker.exe
```
