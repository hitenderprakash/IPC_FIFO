#include <iostream>
#include <string>
#include <sys/stat.h>
#include <fcntl.h>
#include "fd.h"
#include <string.h>
#include <pwd.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <cstdio>
#include <vector>

//driver code for testing
int main(){
    
    std::string homedir(getpwuid(getuid())->pw_dir);
    FileHandle writepipe(-1,homedir+"/mypipe",O_WRONLY);
    int res = writepipe.CreatePipe();
    //res = writepipe.OpenPipe();

    
    //for cancelIo
    FileHandle auxpipe(-1,"/tmp/auxPipe",O_RDONLY|O_NONBLOCK);
    res = auxpipe.CreatePipe();
    
    //explicitly open the auxpipe as we do not read/write it
    res = auxpipe.OpenPipe();

    int count =0;
    
    while(count < 100){
        std::string msg("hello from pipe");
        uint32_t sz = strlen(msg.c_str());
        std::vector<uint8_t> vec = GetMessageLengthNativeByteOrder(sz);
        //write msg length to pipe
        res = writepipe.WriteToPipe(&vec[0], 4);
        //write msg
        //add a little delay in consecutive writes
        usleep(500*1000);
        res = writepipe.WriteToPipe((uint8_t*)msg.c_str(), sz);
        std::cout<<"msg write result: "<<res<<std::endl;
        std::cout<<"writer done"<<std::endl; 
        count++;

    }
    auxpipe.ClosePipe();
    writepipe.ClosePipe(); //will unblockselect in reader thread as read end of pipe will be set with 0 data
    return 0;
    
}