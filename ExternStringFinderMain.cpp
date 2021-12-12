#include <iostream>

#include <stdio.h>
#include <fcntl.h>

#include "src/ExternStringFinder.hpp"
#include "src/Timer.hpp"

using std::cout;
using std::endl;

void removeRAMCache() {
    int fd;
    sync();
    fd = open("/proc/sys/vm/drop_caches", O_WRONLY);
    write(fd, "3", 1);
    close(fd);
}

char textBuffer[MAX_BUFFER_SIZE+1+BUFFER_OVERFLOW];

int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "No input provided." << endl;
        return 1;
    }

    cout << "Buffersize: " << MAX_BUFFER_SIZE << endl;

    Timer timer;
/*
    removeRAMCache();


    timer.start(true);
    readBuffer1(argv[1]);
    timer.stop();
    cout << "readBuffer1(): " << timer.elapsedSeconds() << endl;

    removeRAMCache();

    timer.start(true);
    readBuffer2(argv[1]);
    timer.stop();
    cout << "readBuffer3(): " << timer.elapsedSeconds() << endl;

    removeRAMCache();

    {
        Timer timer;
        FILE *fp = fopen(argv[1], "r");
        int s;
        timer.start(true);
        while (true) {
            s = nextBuffer1(fp, textBuffer);
            // printf("%s", textBuffer);
            if (s <= 0) {
                break;
            } 
        }
        timer.stop();
        fclose(fp);
        cout << timer.elapsedSeconds() << endl;
    }

    cout << endl;

    {
        Timer timer;
        int fd = open(argv[1], O_RDONLY);
        int s;
        timer.start(true);
        while (true) {
            s = nextBuffer2(fd, textBuffer);
            // printf("%s", textBuffer);
            if (s <= 0) {
                break;
            } 
        }
        timer.stop();
        close(fd);
        cout << timer.elapsedSeconds() << endl;
    }
*/

    cout << endl;
    char* filename = argv[1];
    char* pattern = argv[2];

    int matches;
    matches = find1(pattern, filename);

    cout << endl << matches << " Matches found" << endl;

    return 0;
}
