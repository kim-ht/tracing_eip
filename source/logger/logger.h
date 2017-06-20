/* ./logger/logger.h
 * by kimht
 */
#ifndef _LOGGER_LOGGER_H_
#define _LOGGER_LOGGER_H_

//////////////
/// include
//////////////
#include "../base/base.h"

/////////////
/// define
/////////////
#define FAILED_TO_OPEN_LOG_FILE     -1
#define SUCCESS_TO_OPEN_LOG_FILE    0

////////////
/// class
////////////
class Logger {
public:
    static Logger *GetInstance();
    int OpenLogFile(const char *log_path, size_t pc_size);
    void RecordPC(long pc);
    void CloseLogFile();

private:
    static Logger *instance;
    ofstream log_file_;
    size_t pc_size_;

    Logger();
};
#endif


