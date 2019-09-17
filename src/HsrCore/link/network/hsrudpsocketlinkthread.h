#ifndef HSRUDPSOCKETTHREAD_H
#define HSRUDPSOCKETTHREAD_H

#include <QThread>
#include "hsrabstractlink.h"

class QUdpSocket;
class HsrUdpSocketLinkConfig;
class HsrUdpSocketLinkWorker;
class HsrUdpSocketLinkThread : public QThread
{
    Q_OBJECT
public:
    explicit HsrUdpSocketLinkThread(QObject *parent = nullptr);

    /*
    *@brief 设置连接配置
    */
    void setLinkConfig(const std::shared_ptr<HsrLinkAbstractConfig> &cfgPtr);

    //! 打开设备
    //! @emit tcpSocketOpenResp
    void openLink();

    //! 关闭设备
    //! @emit tcpSocketClosed
    void closeLink();

    //! 发送信息
    //! data 发送消息内容  消息长度
    bool sendData(const char* bytes, int len);

    //! 发送信息
    //! bytes 发送数据
    bool sendData(const QByteArray& bytes);

    /*
     * @brief 发送数据, 该接口可以定制接收对象
     * @param [in] data 发送数据
     * @param [in] iplist 接收对象的ip
     * @param [in] portlist 发送数据的端口
     * @note 这里的iplist和portlist是按顺序一一对应的
     */
    bool sendDatagram(const QByteArray &bytes, QStringList iplist, QList<quint16> portlist);

    //! 注册数据接收回调函数
    void registerRecvdDataHandleFunc(ReceivedDataHandleFunc func);

    //! 取消注册数据接收回调函数
    void unregisterRecvdDataHandleFunc();

    //! 注册数据接收回调函数
    void registerRecvdDatagramHandleFunc(UdpReceivedDataHandleFunc func);

    //! 取消注册数据接收回调函数
    void unregisterRecvdDatagramHandleFunc();

	//! worker is ready
	bool workerIsReady() const;

signals:
    //! 将要发送的数据传递给工作线程
    void _invokeSendData(QByteArray data);

    //! 发送数据报文
    void _invokeSendDatagram(QByteArray data, QStringList iplist, QList<quint16> portlist);

    //! 接收到数据，通知用户
    void bytesReceived(QByteArray data);

    //! 打开设备
    void _invokeOpen();

    //! 关闭设备
    void _invokeClose();

    //! 通知打开设备的结果响应
    void udpSocketOpenResp(bool success);

    //! 通知设备关闭
    void udpSocketClosed();

    //! 线程准备是否成功
    void workerReady(bool success);

protected:
    void run() override;

private /*slots*/:
	void onWorkerReady(bool success);

private:
    HsrUdpSocketLinkWorker* m_link_worker;

    std::shared_ptr<HsrLinkAbstractConfig> m_udpsocket_link_cfg_shared_;

    ReceivedDataHandleFunc m_recvd_data_handle_func_;

    UdpReceivedDataHandleFunc m_recvd_datagram_handle_func_;

	bool m_ready_;

	bool m_waitto_open_flag_;		//是否正在等待连接
};


class HsrUdpSocketLinkWorker : public QObject{
    Q_OBJECT
public:
    explicit HsrUdpSocketLinkWorker(QObject* parent = nullptr);

    ~HsrUdpSocketLinkWorker();

    //! 设置tcp连接配置
    void setLinkConfig(HsrUdpSocketLinkConfig* cfg);

    //! 注册数据接收回调函数
    void registerRecvdDataHandleFunc(ReceivedDataHandleFunc func);

    //! 取消注册数据接收回调函数
    void unregisterRecvdDataHandleFunc();

    //! 注册数据接收回调函数
    void registerRecvdDatagramHandleFunc(UdpReceivedDataHandleFunc func);

    //! 取消注册数据接收回调函数
    void unregisterRecvdDatagramHandleFunc();

signals:
    //! 通知udp绑定端口是否成功
    //! success: true 成功; false 失败
    void udpSocketOpenResp(bool success);

    //! 通知tcp设备已关闭
    void udpSocketClosed();

public slots:
    /*
    *@brief 打开tcp设备
    *@emit tcpSocketOpenResp
    */
    void openLink();

    /*
    *@brief 关闭tcp设备
    */
    void closeLink();

    /*
    *@brief 发送数据, 该接口直接进行广播
    */
    void sendData(QByteArray data);

    /*
     * @brief 发送数据, 该接口可以定制接收对象
     * @param [in] data 发送数据
     * @param [in] iplist 接收对象的ip
     * @param [in] portlist 发送数据的端口
     * @note 这里的iplist和portlist是按顺序一一对应的
     */
    void sendDatagram(QByteArray data, QStringList iplist, QList<quint16> portlist);

protected slots:
    void readBytes();

private:
    QUdpSocket* m_udpsocket_;

    HsrUdpSocketLinkConfig* m_udpsocket_link_cfg_;

    QString m_error_string;

    ReceivedDataHandleFunc m_recvd_data_handle_func_;

    UdpReceivedDataHandleFunc m_recvd_datagram_handle_func_;
};

#endif // HSRUDPSOCKETTHREAD_H
