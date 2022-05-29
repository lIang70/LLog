#include "llog.h"

#include <cstdio>
#include <cstdlib>
#include <memory>
#include <thread>

clock_t start, end;
static int LOGNUM = 20000;
static int THREADNUM = 10;

void clean() {
    LLog::terminal();
    end = clock();
    printf("time: %lf s/ %dp \n", double(end - start)/CLOCKS_PER_SEC, LOGNUM * 10 * (THREADNUM / 2));
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
    LLog::setThreadNum(1);
    LLog::start();
    atexit(clean);

    if (argc > 1) {
        THREADNUM = atoi(argv[1]);
        LOGNUM = atoi(argv[2]);
    }
    
    start = clock();
    for (size_t i = 0; i < THREADNUM / 2; i++) {
        std::make_shared<std::thread>(test)->join();
        std::make_shared<std::thread>(test2)->join();
    }
    return 0;
}