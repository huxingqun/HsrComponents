#ifndef HSRUDPSOCKETLINK_H
#define HSRUDPSOCKETLINK_H

#include "hsrabstractlink.h"

class HsrUdpSocketLinkThread;
class HsrUdpSocketLink : public HsrAbstractLink
{
    Q_OBJECT
public:
    explicit HsrUdpSocketLink(QObject* parent = nullptr);
    ~HsrUdpSocketLink() override;

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

    //! 注册数据接收回调函数
    virtual void registerRecvdDatagramHandleFunc(UdpReceivedDataHandleFunc func);

    //! 取消注册数据接收回调函数
    virtual void unregisterRecvdDatagramHandleFunc();

	//! worker is ready
	bool workerIsReady() const;

private:
    HsrUdpSocketLinkThread* m_link_thread_;

    UdpReceivedDataHandleFunc m_recvd_datagram_handle_func_;
};

#endif // HSRUDPSOCKETLINK_H
