#ifndef IEVENTHANDLER_H
#define IEVENTHANDLER_H

#include<cstdint>
#include<iostream>

class IEventHandler
{
private:
    /* data */
public:
    IEventHandler() {};
    virtual ~IEventHandler() = default;


    virtual void handleEvents(int fd, uint32_t events) = 0;


};

#endif // IEVENTHANDLER_Hff