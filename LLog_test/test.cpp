#include "llog.h"

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <thread>

#define MAXTHREAD 100
clock_t start, end;
static int LOGNUM = 20000;
static int THREADNUM = 10;
std::shared_ptr<std::thread> _thread[MAXTHREAD];

void clean() {
    for (size_t i = 0; i < THREADNUM; i++) {
        if (_thread[i]->joinable())
            _thread[i]->join();
    }
    LLog::terminal();
    end = clock();
    printf("time: %.3lf p/s \n", (double)(LOGNUM * 10 * (THREADNUM / 2) * CLOCKS_PER_SEC)/(end - start));
}

void test() {
    for (size_t i = 0; i < LOGNUM; i++) {
        llDebug << 2131231231222222 << " sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssdasdaasdasdasdasdasdasdasdasddddddddddddddwawdwwwwwwwwwwwwwwwwwwasdasdassssssssssssdasdasdasdsssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssb";
        llWarning << 2131231231222222 << " ";
        llError << 123 << " hello " << "llog" << " error!";
        llCritical << 123 << " hello " << "llog" << " critical!";
        llFatal << 123 << " hello " << "llog" << " fatal!";
    }
}

void test2() {
    for (size_t i = 0; i < LOGNUM; i++) {
        llInfo << 2131231231222222 << " ";
        llWarning << 2131231231222222 << " ";
        llError << 123 << " hello " << "llog" << " error!";
        llCritical << 123 << " hello " << "llog" << " critical!";
        llFatal << 123 << " hello " << "llog" << " fatal!";
    }
}

int main(int argc, char* argv[]) {
    printf("========Welcome to LLog test!========\n");
    LLog::setThreadNum(2);
    LLog::start();
    atexit(clean);

    if (argc > 1) {
        THREADNUM = atoi(argv[1]) < MAXTHREAD ? atoi(argv[1]) : MAXTHREAD;
        LOGNUM = atoi(argv[2]);
    }
    
    start = clock();
    for (size_t i = 0; i < THREADNUM; i+=2) {
        _thread[i] = std::make_shared<std::thread>(test);
        _thread[i + 1] = std::make_shared<std::thread>(test2);
    }
    return 0;
}