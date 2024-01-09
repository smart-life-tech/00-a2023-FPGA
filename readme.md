Please note that this code uses the existence of the TTYs(), TTYi(), and TTYo() functions to handle UART communication, and it includes the file "a10fpga.h" for FPGA connection
This code uses the receivedData array to store the received characters. It continues to collect characters until it receives the LF and CR. The received data is null-terminated, making it a valid C string.all from the new added funtion.
This  version uses an enumeration to keep track of the state of data collection. The function now returns 1 when the data collection is successful and 0 otherwise. This allows you to check the status and perform other non-blocking tasks in your main loop.

 breaking down the `collectDataFromFPGA` function step by step:

1. **Enum for State Representation:**
   ```c
   enum DataCollectionState {
       WAIT_START,
       COLLECT_DATA,
       WAIT_LF,
       WAIT_CR
   };
   ```
   This `enum` is used to represent different states in the process of collecting data from the FPGA. The states are:
   - `WAIT_START`: Waiting for the start of a new transmission.
   - `COLLECT_DATA`: Currently collecting data (six ASCII characters).
   - `WAIT_LF`: Waiting for the LF (Line Feed) character.
   - `WAIT_CR`: Waiting for the CR (Carriage Return) character.

2. **State Machine Logic:**
   ```c
   switch (state) {
       // ...
   }
   ```
   The function uses a `switch` statement to implement a state machine. The state machine transitions through the different states based on the received characters and the current state.

3. **WAIT_START State:**
   ```c
   case WAIT_START:
       if (TTYs() == 0xFF) {
           state = COLLECT_DATA;
       }
       break;
   ```
   In the `WAIT_START` state, the function waits until the start of a new transmission is detected (`TTYs() == 0xFF`). If detected, it transitions to the `COLLECT_DATA` state.

4. **COLLECT_DATA State:**
   ```c
   case COLLECT_DATA:
       if (TTYs() == 0xFF) {
           // Read character, convert to uppercase, and store in 'receivedData'
           // Check if collected 6 characters and transition to the next state
       }
       break;
   ```
   In the `COLLECT_DATA` state, the function reads characters from the FPGA, converts lowercase to uppercase, stores them in `receivedData`, and checks if it has collected six characters. If six characters are collected, it transitions to the `WAIT_LF` state.

5. **WAIT_LF State:**
   ```c
   case WAIT_LF:
       if (TTYs() == 0xFF) {
           // Read LF character and check for transition to the next state
       }
       break;
   ```
   In the `WAIT_LF` state, the function waits for the LF character and transitions to the `WAIT_CR` state when detected.

6. **WAIT_CR State:**
   ```c
   case WAIT_CR:
       if (TTYs() == 0xFF) {
           // Read CR character, check for completion, and transition to the WAIT_START state
       }
       break;
   ```
   In the `WAIT_CR` state, the function waits for the CR character. When detected, it checks if the data collection is complete and transitions back to the `WAIT_START` state.

7. **Function Return:**
   ```c
   return 0; // Data collection not complete yet
   return 1; // Successfully collected data
   ```
   The function returns `0` when the data collection is not complete, and `1` when the data collection is successful. This allows the calling code to determine whether the data has been successfully collected.

This function, therefore, represents a finite state machine that efficiently manages the process of collecting specific data from the FPGA in a non-blocking manner.In the context of the provided code, making the data collection from the FPGA non-blocking allows the main program to perform other tasks while waiting for the specific sequence of characters to arrive. The program can periodically check if the data has been successfully collected without waiting for it to finish. This approach is beneficial in systems where responsiveness and the ability to handle multiple tasks concurrently are important.