#include "hsrudpsocketlinkconfig.h"

HsrUdpSocketLinkConfig::HsrUdpSocketLinkConfig(quint16 sendPort, quint16 recvPort, QObject* parent)
    : HsrLinkAbstractConfig (parent)
    , m_send_port_(sendPort)
    , m_recv_port_(recvPort)
{

}

HsrLinkAbstractConfig::LinkType HsrUdpSocketLinkConfig::linkType()
{
    return HsrLinkAbstractConfig::UDP_SOCKET;
}

quint16 HsrUdpSocketLinkConfig::sendPort() const
{
    return m_send_port_;
}

quint16 HsrUdpSocketLinkConfig::recvPort() const
{
    return m_recv_port_;
}

void HsrUdpSocketLinkConfig::copyFrom(HsrLinkAbstractConfig *other)
{
    HsrLinkAbstractConfig::copyFrom(other);
    HsrUdpSocketLinkConfig* other_cfg = static_cast<HsrUdpSocketLinkConfig*>(other);
    m_send_port_ = other_cfg->sendPort();
    m_recv_port_ = other_cfg->recvPort();
}
