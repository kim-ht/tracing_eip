/* ./logger/logger.cpp
 * by kimht
 */
#include "./logger.h"

Logger *Logger::instance = 0;

/////////////
/// public
/////////////

/*
 * GetInstance - For singleton.
 */
Logger *Logger::GetInstance() {
    if ( !instance ) {
        instance = new Logger();
    }
    return instance;
}

/*
 * CreateLogFile    - Open/creates log file.
 *
 * @log_path        - The path of log file.
 * @pc_size         - The size of program counter. i386 should be 4(byte) and x86-64 should be 8(byte).
 * @return          - SUCCESS_TO_OPEN_LOG_FILE(0) on success; FAILED_TO_OPEN_LOG_FILE(-1) on failure.
 */
int Logger::OpenLogFile(const char *log_path, size_t pc_size) {
    pc_size_ = pc_size;
    // create log file that its name is tracee's path.pc
    log_file_.open(log_path, ios::out | ios::binary);
    if ( log_file_.is_open() == false ) {
        perror("Logger::OpenLogFile() failed");
        cout << "  --> log_file_.is_open() == false" << endl;
        return FAILED_TO_OPEN_LOG_FILE;
    }
    return SUCCESS_TO_OPEN_LOG_FILE;
}

/*
 * RecordPC - Record the program counter.
 *
 * @pc      - The program counter which will be logged.
 */
void Logger::RecordPC(long pc) {
    log_file_.write((char *)&pc, pc_size_);
}

/*
 * CloseLogFile - Closes the log file opened.
 */
void Logger::CloseLogFile() {
    log_file_.close();
}

//////////////
/// private
//////////////

/*
 * Logger - The constructor.
 */
Logger::Logger() {
}

