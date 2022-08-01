#ifndef BASECLASS_H
#define BASECLASS_H

class BaseClass
{
public:
    virtual void setup() = 0;
    virtual void setupConnected() = 0;
    virtual void loop() = 0;
    virtual void loopConnected() = 0;
};

#endif //BASECLASS_H