#include "Altair.h"

Altair::Altair()
{
    auto controllerIterate = std::bind(&Controller::iterate, _controller, std::placeholders::_1, std::placeholders::_2);
    _msfsIntf.reset(new MsfsInterface(controllerIterate));
    _msfsIntf->start();
}