#include "./tracer/tracer.h"

int main(void) {
    Tracer *tracer = new Tracer();

    tracer->RunProgram("./challenge", NULL, NULL);
    cout << "starting StepInto()" << endl;
    while ( tracer->StepInto() != FAILED_TO_STEP_INTO );
    cout << "starting PrintRIPs()" << endl;
    tracer->PrintRIPs();
    delete(tracer);

    return 0;
}

