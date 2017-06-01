/* main.cpp
 * by kimht
 */
#include "./tracer/tracer.h"

int main(int argc, char *argv[]) {
    if ( argc != 2 ) {
        cout << "usage >> " << argv[0] << " [program_path]" << endl;
        return 0;
    }

    Tracer *tracer = new Tracer();
    cout << "starting tracing rip of " << argv[1]<< endl;
    tracer->TraceProgram(argv[1], NULL, NULL);
    delete(tracer);

    return 0;
}

