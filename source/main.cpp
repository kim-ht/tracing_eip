/* main.cpp
 * -lby kimht
 */
#include "./tracer/tracer.h"

int main(int argc, char *argv[]) {
    Logger::GetInstance();
    Tracer::GetInstance();
    Disassembler::GetInstance();

    Tracer::GetInstance()->StartTracingProgram("/home/kimht/gits/tracing_eip/source/challenge", NULL, NULL);

    return 0;
}

