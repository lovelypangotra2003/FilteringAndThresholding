#include"functions.h"    
//all the header files are in functions.h in the same directory
using namespace std;

// Function to simulate the data generation process and send data through a pipe
void generateData(int m, int scanWidth, int T, int pipe_fd) {
    srand(time(0));
    
    while (true) {
        // Generate an infinite row of random uint8_t values
        vector<uint8_t> row(m);
        for (int i = 0; i < m; ++i) {
            row[i] = rand() % 256;
        }
        // Simulate scanning each row of pixels
        for (int i = 0; i < m; i += scanWidth) {
            // Output 'scanWidth' pixels at a time
            for (int j = i; j < min(i + scanWidth, m); ++j) {
                uint8_t pixelValue = row[j];
                // Send the pixel value through the pipe to the child process
                write(pipe_fd, &pixelValue, sizeof(uint8_t));
            }
            
            // Sleep for 'T' milliseconds to emulate the time interval
            this_thread::sleep_for(chrono::milliseconds(T));
        }
    }
}

// Function to read data from a CSV file and send it through a pipe
void readDataFromCSV(int T, int pipe_fd) {
    ifstream inputFile("sample_data.csv");
    if (!inputFile) {
        cerr << "Failed to open sample_data.csv" << endl;
        return;
    }

    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);
        string token;
        while (getline(iss, token, ',')) {
            uint8_t pixelValue = stoi(token);
            write(pipe_fd, &pixelValue, sizeof(uint8_t));
            // Sleep for 'T' milliseconds to emulate the time interval
            this_thread::sleep_for(chrono::milliseconds(T));
        }
    }
    inputFile.close();
    exit(0);
}

// Function to perform filtering and thresholding on data received through a pipe
void filterAndThreshold(int m, int threshold, int pipe_fd) {
    // Define the filter window
    double filterWindow[] = {0.00025177, 0.008666992, 0.078025818, 0.24130249, 0.343757629, 0.24130249, 0.078025818, 0.008666992, 0.00025177};
    int filterSize = sizeof(filterWindow) / sizeof(filterWindow[0]);
    
    vector<uint8_t> buffer(filterSize, 0);
    int final_val = 0;

    while (true) {
        // Read data from the pipe (data generation process)
        uint8_t pixelValue;
        ssize_t bytesRead = read(pipe_fd, &pixelValue, sizeof(uint8_t));

        if (bytesRead == sizeof(uint8_t)) {
            // Add the new pixelValue to the buffer
            buffer.push_back(pixelValue);

            // Check if the buffer has enough data for filtering
            if (buffer.size() >= filterSize) {
                // Apply the filter
                double filteredValue = 0.0;
                for (int i = 0; i < filterSize; ++i) {
                    filteredValue += buffer[i] * filterWindow[i];
                }
                if (filteredValue > threshold){
                    final_val = 1;
                }
                else{
                    final_val = 0;
                }
                // Output the filtered value
                cout << "Pixel Value: " << static_cast<int>(pixelValue)<< "    Filtered Value: " << static_cast<int>(filteredValue) << "    Threshold Value: " << final_val << endl ;
                // Remove the oldest pixelValue from the buffer
                buffer.erase(buffer.begin());
            }
        }
        // Sleep for a while before processing the next batch
        this_thread::sleep_for(chrono::milliseconds(1000));
    }
}
int main() {
    int m;
    cout << "Enter the number of columns (m): ";
    cin >> m;

    int scanWidth;
    cout << "Enter the scan width: ";
    cin >> scanWidth;

    int T;
    cout << "Enter the time interval (T) in milliseconds: ";
    cin >> T;

    int threshold;
    cout << "Enter the threshold value: ";
    cin >> threshold;

    int choice;
    cout << "Select data source:" << endl;
    cout << "1. Generate random data" << endl;
    cout << "2. Use sample 2D array from a CSV file" << endl;
    cout << "Enter your choice (1 or 2): ";
    cin >> choice;

    // Create a pipe for communication between parent and child processes
    int pipe_fds[2];
    if (pipe(pipe_fds) == -1) {
        cerr << "Pipe creation failed." << endl;
        return 1;
    }

    // Create a new process for filtering and thresholding
    pid_t pid = fork();

    if (pid < 0) {
        cerr << "Fork failed." << endl;
        return 1;
    } else if (pid == 0) {
        // This is the child process (filtering and thresholding)
        close(pipe_fds[1]); // Close the write end of the pipe in the child process
        filterAndThreshold(m, threshold, pipe_fds[0]);
        close(pipe_fds[0]); // Close the read end of the pipe in the child process
    } else {
        // This is the parent process (data generation)
        close(pipe_fds[0]); // Close the read end of the pipe in the parent process

        if (choice == 1) {
            // Generate random data
            generateData(m, scanWidth, T, pipe_fds[1]);
        } else if (choice == 2) {
            // Read data from a CSV file
            readDataFromCSV(T, pipe_fds[1]);
        } else {
            cerr << "Invalid choice. Exiting..." << endl;
            close(pipe_fds[1]);
            return 1;
        }

        close(pipe_fds[1]); // Close the write end of the pipe in the parent process
    }

    return 0;
}
//gurubaran.elango@cynlr.com
