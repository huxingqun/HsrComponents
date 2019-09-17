#include <QDebug>
#include "hsrtcpsocketlinkconfig.h"

HsrTcpSocketLinkConfig::HsrTcpSocketLinkConfig(const QHostAddress &address, quint16 port, QObject *parent)
    : HsrLinkAbstractConfig(parent)
	, m_hostadress_(address)
	, m_port_(port)
{

}

HsrTcpSocketLinkConfig::HsrTcpSocketLinkConfig(HsrTcpSocketLinkConfig *other)
    : HsrLinkAbstractConfig (other)
{
    m_hostadress_ = other->address();
    m_port_ = other->port();
}

HsrTcpSocketLinkConfig::~HsrTcpSocketLinkConfig()
{
}

HsrLinkAbstractConfig::LinkType HsrTcpSocketLinkConfig::linkType()
{
    return HsrLinkAbstractConfig::TCP_SOCKET;
}

QHostAddress HsrTcpSocketLinkConfig::address() const
{
	return m_hostadress_;
}

quint16 HsrTcpSocketLinkConfig::port() const
{
    return m_port_;
}

void HsrTcpSocketLinkConfig::copyFrom(HsrLinkAbstractConfig *other)
{
    HsrLinkAbstractConfig::copyFrom(other);
    HsrTcpSocketLinkConfig* other_cfg = static_cast<HsrTcpSocketLinkConfig*>(other);
    m_hostadress_ = other_cfg->address();
    m_port_ = other_cfg->port();
}


