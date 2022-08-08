#include <chrono>           // needed for sleep

// function prototypes
long get_time(void);        // get milliseconds
void sleep(long millis);  //delay milliseconds

/* ----------------------------------------------------- */

// function definitions

// get milliseconds
long get_time(void) {
    auto time = std::chrono::system_clock::now();
    auto duration = time.time_since_epoch();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    long now = millis.count();
    return now;
}

// delay code execution
void sleep(long millis) {
    long start = get_time();
    long current = get_time();
    while (current - start < millis) {
        current = get_time();
    }
    return;
}
