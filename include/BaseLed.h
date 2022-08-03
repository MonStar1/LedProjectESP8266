#include "LedConfig.h"

#ifndef BASELED_H
#define BASELED_H

class BaseLed
{
public:
    virtual void loop() = 0;
};

#endif // BASELED_H