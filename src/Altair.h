#pragma once
#include "MsfsInterface.h"
#include "Controller.h"

class Altair
{
public:
    Altair();

private:
    std::unique_ptr<MsfsInterface> _msfsIntf;
    Controller _controller;
};

