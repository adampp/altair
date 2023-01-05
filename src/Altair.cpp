#include "Altair.h"

Altair::Altair()
{
    auto controllerIterate = std::bind(&Controller::iterate, _controller, std::placeholders::_1);
    _msfsIntf.reset(new MsfsInterface(controllerIterate));
    _msfsIntf->start();
}