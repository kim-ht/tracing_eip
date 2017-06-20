/* main.cpp
 * -lby kimht
 */
#include "./tracer/tracer.h"

int main(int argc, char *argv[]) {
    Logger::GetInstance();
    Tracer::GetInstance();

    Tracer::GetInstance()->StartTracingProgram("./challenge", NULL, NULL);

    return 0;
}

