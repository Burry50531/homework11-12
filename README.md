# Signal Timeline Controller

This project demonstrates a controller process that reacts to signals from multiple child processes. It logs events with timestamps and generates a timeline graph of the signal flow.

 Features

- Handles `SIGUSR1` and `SIGUSR2` from child processes
- Captures the exact time each signal is received
- Sorts events by timestamp
- Generates a timeline graph in DOT format
- Outputs a PNG visualization via Graphviz

 Build and Run

gcc -o signal-timeline-controller controller.c
./signal-timeline-controller
