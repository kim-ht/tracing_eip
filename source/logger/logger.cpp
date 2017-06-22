/* ./logger/logger.cpp
 * by kimht
 */
///////////////////////////////////////////////////////////////////////////////
/// include
///////////////////////////////////////////////////////////////////////////////
#include "./logger.h"

Logger *Logger::instance = 0;

///////////////////////////////////////////////////////////////////////////////
/// public
///////////////////////////////////////////////////////////////////////////////

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
 * @pc_size         - The size of program counter. i386 should be 4(byte) and
 *                    x86-64 should be 8(byte).
 * @return          - true on success; false on failure.
 */
bool Logger::OpenLogFile(const char *log_path, size_t pc_size) {
    pc_size_ = pc_size;

    // Open/creates log file.
    log_file_.open(log_path, ios::out | ios::binary);
    if ( !log_file_.is_open() ) {
        HANDLE_ERROR("Logger::OpenLogFile() failure", false);
    }

    return true;
}

/*
 * RecordCycle  - Record the program counter and disassembled code.
 *
 * @pc          - The program counter which will be logged.
 */
void Logger::RecordCycle(uint64_t pc, const string disas) {
    log_file_ << hex << pc << "\t" << disas << "\n";
}

/*
 * CloseLogFile - Closes the log file opened.
 */
void Logger::CloseLogFile() {
    log_file_.close();
}

///////////////////////////////////////////////////////////////////////////////
/// private
///////////////////////////////////////////////////////////////////////////////

/*
 * Logger - The constructor.
 */
Logger::Logger() {
}

