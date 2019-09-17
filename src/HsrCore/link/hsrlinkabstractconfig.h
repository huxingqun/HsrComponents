#ifndef HSRLINKABSTRACTCONFIG_H
#define  HSRLINKABSTRACTCONFIG_H

/*
*@brief  连接配置的基类
*/

#include <QObject>
#include "HsrCore/hsrcore_global.h"

class HSRCORE_EXPORT HsrLinkAbstractConfig : public QObject
{
	Q_OBJECT

public:
	enum LinkType{
		UNKNOW = 0,					//未知类型,一般用于初始化
		SERIAL_PORT = 1,			//串口连接类型
		TCP_SOCKET = 2,				//tcp连接
        UDP_SOCKET = 3,				//udp连接
	};
    HsrLinkAbstractConfig(QObject *parent = nullptr);
    ~HsrLinkAbstractConfig();

	//连接类型
	//@see LinkType
    virtual HsrLinkAbstractConfig::LinkType linkType() = 0;

    virtual void copyFrom(HsrLinkAbstractConfig* other);

protected:
	LinkType m_linktype;
};



#endif
