#ifndef TCPSOCKETLINKTHREAD_H
#define TCPSOCKETLINKTHREAD_H

#include <QThread>
#include <memory>
#include <QFile>
#include "link/hsrabstractlink.h"

/*
*@brief tcp工作线程类
*/
class QTcpSocket;
class HsrLinkAbstractConfig;
class HsrTcpSocketLinkWorker;
class HsrTcpSocketLinkThread : public QThread
{
	Q_OBJECT

public:
    HsrTcpSocketLinkThread(QObject *parent = nullptr);
    ~HsrTcpSocketLinkThread() override;

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

	//! 注册数据接收回调函数
	void registerRecvdDataHandleFunc(ReceivedDataHandleFunc func);

	//! 取消注册数据接收回调函数
	void unregisterRecvdDataHandleFunc();

	//! worker is ready
	bool workerIsReady() const;

signals:
	//! 将要发送的数据传递给工作线程
	void _invokeSendData(QByteArray data);

	//! 接收到数据，通知用户
	void bytesReceived(QByteArray data);

	//! 打开设备
	void _invokeOpen();

	//! 关闭设备
	void _invokeClose();

	//! 通知打开设备的结果响应
	void tcpSocketOpenResp(bool success);

	//! 通知设备关闭
	void tcpSocketClosed();

	//! 线程准备是否成功
	void workerReady(bool success);

protected:
	void run() override;

private /*slots*/:
	void onWorkerReady(bool success);

private:
    HsrTcpSocketLinkWorker* m_link_worker;

    std::shared_ptr<HsrLinkAbstractConfig> m_tcpsocket_link_cfg_shared_;

    ReceivedDataHandleFunc m_recvd_data_handle_func_;

	bool m_ready_;

	bool m_waitto_open_flag_;		//是否正在等待连接
};

/*
*@brief TCP工作类
*/
class HsrTcpSocketLinkConfig;
class HsrTcpSocketLinkWorker : public QObject
{
	Q_OBJECT
public:
    explicit HsrTcpSocketLinkWorker(QObject* parent = nullptr);
    ~HsrTcpSocketLinkWorker();

	//! 设置tcp连接配置
    void setLinkConfig(HsrTcpSocketLinkConfig* cfg);

	//! 注册数据接收回调函数
	void registerRecvdDataHandleFunc(ReceivedDataHandleFunc func);

	//! 取消注册数据接收回调函数
	void unregisterRecvdDataHandleFunc();

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
	*@brief 发送数据
	*/
	void sendData(QByteArray data);

protected slots:
	void readBytes();

signals:
	//! 传递从tcp接收到的数据
	void bytesReceived(QByteArray data);

	//! 通知tcp的打开回复
	//! success: true 成功; false 失败
	void tcpSocketOpenResp(bool success);

	//! 通知tcp设备已关闭
	void tcpSocketClosed();

private:
	QTcpSocket* m_tcpsocket_;

    HsrTcpSocketLinkConfig* m_tcpsocket_link_cfg_;

	QString m_error_string;

    ReceivedDataHandleFunc m_recvd_data_handle_func_;

	QFile* m_read_save_file_;

	QFile* m_write_save_file_;
};


#endif
