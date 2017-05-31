#include "./tracer/tracer.h"

int main(void) {
    Tracer tracer;

    tracer.RunProgram("./test", NULL, NULL);
    while ( tracer.StepInto() != FAILED_TO_STEP_INTO );

    return 0;
}

