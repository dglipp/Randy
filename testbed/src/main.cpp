#include <core/logger.h>
#include <core/asserts.h>

//TODO: Test
#include <core/application.h>

void main(){
    Application app("Randy Engine Testbed", 100, 100, 1280, 720);
    app.run();
}