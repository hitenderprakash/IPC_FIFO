#include <iostream>
#include <string>
#include "fd.h"
#include <pwd.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include <cstdio>
#include <fcntl.h>
#include <sys/types.h>
#include <thread>
#include <chrono>

//driver code for testing
void ReadLoop(FileHandle& readpipe,FileHandle& auxpipe){
    while(true){
        std::vector<uint8_t> readLenBuff(4,0x00U);


        int res = readpipe.ReadFromPipe(auxpipe.GetPipeFd(),&readLenBuff[0],4);
        if(res <= 0){
            std::cout<<"IO Canceled before read: "<<res<<std::endl;
            break ;
        }

        uint32_t len = GetMessageLength(readLenBuff);

        //std::cout<<"Length to be read from pipe: "<<len<<std::endl;

        std::vector<uint8_t> readBuff(len,0x00U);
        res = readpipe.ReadFromPipe(auxpipe.GetPipeFd(),&readBuff[0],len);
        if(res <= 0){
            std::cout<<"IO Canceled before read"<<std::endl;
            break;
        }
        std::string msg = std::string(readBuff.begin(), readBuff.end());
        std::cout<<"["+msg+"]"<<std::endl;
        if(msg == "hello from pipe"){
            std::cout<<"true"<<std::endl;
        }
        else {
            std::cout<<"false"<<std::endl;
        }
    }
}

void CancelIOEX(FileHandle& auxfd){
    FileHandle auxfdWr(-1,auxfd.GetPipeName().c_str(),O_WRONLY|O_NONBLOCK);
    auxfdWr.CreatePipe();
    auxfdWr.OpenPipe();
    auxfdWr.ClosePipe();
}

int main(){
    std::string homedir(getpwuid(getuid())->pw_dir);
    FileHandle readpipe(-1,homedir+"/mypipe",O_RDONLY|O_NONBLOCK);
    int res = readpipe.CreatePipe();

    FileHandle auxpipe(-1,"/tmp/auxPipe",O_RDONLY|O_NONBLOCK);
    res = auxpipe.CreatePipe();
    //explicitly open the auxpipe as we do not read/write it
    res = auxpipe.OpenPipe();

    std::thread t = std::thread(&ReadLoop, std::ref(readpipe),std::ref(auxpipe));
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::cerr <<"going to kill from main"<<std::endl;

    //to initiate cancel read from the same process
    CancelIOEX(auxpipe);

    t.join();
    readpipe.ClosePipe(); //or comment this
    return 0;
    
}