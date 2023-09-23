# FilteringAndThresholding

1. Introduction

The Real-time Data Filtering and Thresholding project is designed to demonstrate the real-time processing of data streams. It involves generating data, applying a filtering algorithm, and thresholding the results to make binary decisions. The project is implemented in C++ and utilizes inter-process communication (IPC) to pass data between a data generation process and a data filtering and thresholding process.

2. Project Description/Components

The project consists of three main components:

2.1. Data Generation
- Responsible for simulating data generation or reading data from a CSV file.
- Generates random pixel values or reads pixel values from a CSV file.
- Emulates the behavior of a data source, sending pixel values to the filtering process in real-time.

2.2. Data Filtering and Thresholding
- Receives pixel values from the data generation process in real-time.
- Applies a convolutional filtering operation using a predefined filter window.
- Thresholds the filtered value to make a binary decision (1 if above the threshold, 0 otherwise).
- Outputs the original pixel value, filtered value, and threshold result.

2.3. Main Control Flow
- The main program orchestrates the entire process:
   - Accepts user input for parameters such as the number of columns (m), scan width, time interval (T), and threshold.
   - Creates a pipe for communication between the parent (data generation) and child (data filtering) processes.
   - Forks a child process to handle filtering and thresholding.
   - In the parent process:
     - Chooses between generating random data or reading data from a CSV file based on user input.
     - Passes data through the pipe to the child process.
   - In the child process:
     - Performs data filtering and thresholding.
     - Outputs results to the console.

3. Usage Instructions
To use the Real-time Data Filtering and Thresholding project:
--> Compile the code using a C++ compiler.
--> Run the compiled executable.
--> Follow the on-screen prompts to input parameters:
   - Number of columns (m)
   - Scan width
   - Time interval (T) in milliseconds
   - Threshold value
   - Select data source (random data generation or CSV file input)
--> The program will create two processes:
   - The parent process handles data generation or CSV reading.
   - The child process handles data filtering and thresholding.
--> Observe the console output, which displays the original pixel value, filtered value, and threshold result in real-time.
--> To exit the program, press `Ctrl + C` or close the console window.

Ensure your folder structure resembles the following:

Curr_Folder/
│
├── vision.cpp            # Your main C++ source file
│
├── functions.h             #header files used
│
├── sample_data.csv         # Sample data file in CSV format
│

