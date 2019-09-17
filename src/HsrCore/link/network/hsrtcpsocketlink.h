#ifndef TCPSOCKETLINK_H
#define TCPSOCKETLINK_H

#include "link/hsrabstractlink.h"


class HsrLinkAbstractConfig;

class HsrTcpSocketLinkThread;
class HSRCORE_EXPORT HsrTcpSocketLink : public HsrAbstractLink
{
	Q_OBJECT

public:
    HsrTcpSocketLink(QObject *parent = nullptr);
    ~HsrTcpSocketLink() override;

	virtual bool start() override;

	//! 打开连接
    virtual void open(const std::shared_ptr<HsrLinkAbstractConfig>& linkConfig) override;

	//! 关闭连接
	virtual void close() override;

	//! 发送信息
	//! data 发送消息内容  消息长度
	virtual bool sendData(const QByteArray& data) override;

	//! 注册数据接收回调函数
	virtual void registerRecvdDataHandleFunc(ReceivedDataHandleFunc func) override;

	//! 取消注册数据接收回调函数
	virtual void unregisterRecvdDataHandleFunc() override;

	//! worker is ready
	bool workerIsReady() const;

private:
    HsrTcpSocketLinkThread* m_link_thread_;
};


#endif 
