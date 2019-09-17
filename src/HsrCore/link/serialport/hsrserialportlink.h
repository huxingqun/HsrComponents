#ifndef HSRSERIALPORTLINK_H
#define HSRSERIALPORTLINK_H

#include "link/hsrabstractlink.h"

class QThread;

class HsrLinkAbstractConfig;
class HsrSerialPortLinkThread;
	//@note 这里需要使用SerialLinkConfig来构造
class HSRCORE_EXPORT HsrSerialPortLink : public HsrAbstractLink
{
	Q_OBJECT
public:
    explicit HsrSerialPortLink(QObject *parent = nullptr);
    ~HsrSerialPortLink() override;

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
protected:

signals:

private:
    HsrSerialPortLinkThread* m_link_thread_;

};

#endif

