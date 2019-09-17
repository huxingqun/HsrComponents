#include "hsrlinkabstractconfig.h"


HsrLinkAbstractConfig::HsrLinkAbstractConfig(QObject *parent)
	: QObject(parent)
    , m_linktype(HsrLinkAbstractConfig::UNKNOW)
{
}

HsrLinkAbstractConfig::~HsrLinkAbstractConfig()
{
}

void HsrLinkAbstractConfig::copyFrom(HsrLinkAbstractConfig *other)
{
    (void)other;
}
