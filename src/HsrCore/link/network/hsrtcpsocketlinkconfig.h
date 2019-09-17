#ifndef TCPSOCKETLINKCONFIG_H
#define TCPSOCKETLINKCONFIG_H

#include <QObject>
#include <QHostAddress>
#include "HsrCore/link/hsrlinkabstractconfig.h"
#include "HsrCore/hsrcore_global.h"

/*
*Tcp 连接配置
*/
class HSRCORE_EXPORT HsrTcpSocketLinkConfig : public HsrLinkAbstractConfig
{
	Q_OBJECT

public:
    HsrTcpSocketLinkConfig(const QHostAddress &address, quint16 port, QObject *parent = nullptr);
    HsrTcpSocketLinkConfig(HsrTcpSocketLinkConfig* other);
    ~HsrTcpSocketLinkConfig() override;

	//连接类型
	//@see LinkType
    virtual HsrLinkAbstractConfig::LinkType linkType() override;

	QHostAddress address() const;

	quint16 port() const;

    void copyFrom(HsrLinkAbstractConfig* other) override;

private:
	QHostAddress m_hostadress_;
	quint16 m_port_;
};

#endif // TCPSOCKETLINKCONFIG_H
