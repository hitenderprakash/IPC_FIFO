#pragma once 
#include <string>
#include <vector>
class FileHandle
{
private: 
    int pipeFd = -1;
    std::string pipeName;
    int oFlag;
    

public:
    FileHandle(int fd, std::string path, int flag):pipeFd(fd),pipeName(path),oFlag(flag){}    
    virtual ~FileHandle(){ }

    int GetPipeFd();
    std::string GetPipeName();
    bool IsValid();
    bool CreatePipe();
    bool OpenPipe();
    void ClosePipe();
    int ReadFromPipe(int cancelIOFd, uint8_t *buf, size_t count);
    int WriteToPipe(uint8_t *buf, size_t count);
};

std::vector<uint8_t> GetMessageLengthNativeByteOrder(uint32_t len);

uint32_t GetMessageLength(const std::vector<uint8_t>& inVec);