/* main.cpp
 * by kimht
 */
#include "./tracer/tracer.h"

int main(int argc, char *argv[]) {
    Tracer *tracer = new Tracer();
    cout << "starting tracing rip of " << endl;
    tracer->TraceProgram("./test", NULL, NULL);
    delete(tracer);

    return 0;
}

