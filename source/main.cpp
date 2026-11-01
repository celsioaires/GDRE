#include "application.h"

int main() {
    src::Application application;

    if (!application.execute()) {
        return 1;
    }

    return 0;
}
