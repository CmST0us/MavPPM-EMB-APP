//
// Created by cmst0us on 19-3-5.
//

#include <signal.h>

#include "mavppm.hpp"
int main(int argc, char *argv[]) {
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigaction( SIGPIPE, &sa, 0 );

    mavppm::MavPPM app;
    app.run();

    return 0;
}