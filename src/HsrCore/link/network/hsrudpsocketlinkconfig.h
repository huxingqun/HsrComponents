#ifndef HSRUDPSOCKETLINKCONFIG_H
#define HSRUDPSOCKETLINKCONFIG_H

#include "hsrlinkabstractconfig.h"

class HsrUdpSocketLinkConfig : public HsrLinkAbstractConfig
{
    Q_OBJECT
public:
    explicit HsrUdpSocketLinkConfig(quint16 sendPort, quint16 recvPort, QObject* parent = nullptr);

    //连接类型
    //@see LinkType
    virtual HsrLinkAbstractConfig::LinkType linkType() override;

    quint16 sendPort() const;

    quint16 recvPort() const;

    void copyFrom(HsrLinkAbstractConfig* other) override;

private:
    quint16 m_send_port_;                //发送端口
    quint16 m_recv_port_;                //接收端口

};

#endif // HSRUDPSOCKETLINKCONFIG_H
