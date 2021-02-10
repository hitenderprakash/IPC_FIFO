#include <vector>
#include <unistd.h>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include <pwd.h>
#include "fd.h"
#include <iostream>

bool FileHandle::IsValid(){
    return (pipeFd != -1);
}

int FileHandle::GetPipeFd(){
    return pipeFd;
}

std::string FileHandle::GetPipeName(){
    return pipeName;
}

bool FileHandle::CreatePipe(){
    if(STDIN_FILENO == pipeFd || STDOUT_FILENO == pipeFd){
        //nothing to be done for stdin and stdout
        return true;
    }
    int result = mkfifo(pipeName.c_str(),0666);
    if((result == -1) && (errno != EEXIST)){
        return false;
    }
    return true;
}

bool FileHandle::OpenPipe(){
    if(STDIN_FILENO == pipeFd || STDOUT_FILENO == pipeFd || IsValid()){
        //nothing to be done for stdin / stdout of already a valid descriptor
        return true;
    }
    pipeFd = open(pipeName.c_str(),oFlag);
    if(-1 == pipeFd){
        return false;
    }
    return true;
}

void FileHandle::ClosePipe(){
    if(IsValid()){
        close(pipeFd);
        unlink(pipeName.c_str());
        pipeFd = -1;
    }
}

int FileHandle::ReadFromPipe(int cancelIOFd, uint8_t *buf, size_t count){
    if(!OpenPipe() || -1 == cancelIOFd){
        return -1;
    }
    fd_set readset;
    FD_ZERO(&readset);

    FD_SET(pipeFd, &readset); 
    FD_SET(cancelIOFd, &readset); 

    int maxFd = pipeFd > cancelIOFd ? pipeFd : cancelIOFd;
    std::cerr<<"in select() "<<std::endl;
    struct timeval tv;
    tv.tv_sec = 10;
    tv.tv_usec = 0;
    int result = select(maxFd + 1, &readset, NULL, NULL, &tv);
    std::cerr<<"select result: "<<result<<"read fd "<< FD_ISSET(pipeFd, &readset)<<"cancelFd "<< FD_ISSET(cancelIOFd, &readset)<<std::endl;
    if(result == 0 ){
        std::cerr<<"timeout happened"<<std::endl;
    }
    if(result > 0 && FD_ISSET(cancelIOFd, &readset)){
        std::cerr<<"cancelIOFd (aux pipe) was closed from another thread"<<std::endl;
    }
    if(result > 0 && FD_ISSET(pipeFd, &readset)){
        FD_CLR(pipeFd, &readset);
        result = read(pipeFd, buf, count);
        if(result > 0){
            return result; //data available 
        }
        else if(result < 0){
            //error in read
        }
        else if(result == 0 ){
            std::cerr<<"write end of pipe was closed"<<std::endl;           
        }
    }
    
    return -1;
}
int FileHandle::WriteToPipe(uint8_t *buf, size_t count){
    if(!OpenPipe()){
        return -1;
    }
    return write(pipeFd, buf, count);
}

std::vector<uint8_t> GetMessageLengthNativeByteOrder(uint32_t len){
    return std::vector<uint8_t>{static_cast<uint8_t>(len<<0),static_cast<uint8_t>(len<<8),static_cast<uint8_t>(len<<16),static_cast<uint8_t>(len<<24)};
}

uint32_t GetMessageLength(const std::vector<uint8_t>& inVec){
    uint32_t length = 0; 

    if(4 != inVec.size()){
        return length; //Error 
    }
    for(size_t i = 0; i<4; ++i){
        length |= inVec[i] << i*8;
    }
    return length;
}