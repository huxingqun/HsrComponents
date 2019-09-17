#ifndef SERIALPORTLINKTHREAD_H
#define SERIALPORTLINKTHREAD_H

#include <QThread>
#include <memory>
#include <QFile>
#include <atomic>
#include <QTimer>
#include <queue>
#include "link/hsrabstractlink.h"


class QSerialPort;
/*
*@brief 串口工作线程类
*/

class HsrLinkAbstractConfig;
class HsrSerialPortLinkWorker;
class HsrSerialPortLinkThread : public QThread
{
	Q_OBJECT

public:
    explicit HsrSerialPortLinkThread(QObject *parent = nullptr);
    ~HsrSerialPortLinkThread();

	/*
	*@brief 设置连接配置
	*/
    void setLinkConfig(const std::shared_ptr<HsrLinkAbstractConfig> &cfgPtr);

	//! 打开设备
	//! @emit serialPortOpenResp
	void openLink();

	//! 关闭设备 
	//! @emit serialPortClosed
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

	//! 通讯设备是否打开
	bool linkIsOpen();

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
	void serialPortOpenResp(bool success);

	//! 通知设备关闭
	void serialPortClosed();

	//! 线程准备是否成功
	void workerReady(bool success);

protected:
	void run() override;

private /*slots*/:
	void onWorkerReady(bool success);

private:
    HsrSerialPortLinkWorker* m_link_worker;

    std::shared_ptr<HsrLinkAbstractConfig> m_serialport_link_cfg_shared_;

    ReceivedDataHandleFunc m_recvd_data_handle_func_;

	bool m_ready_;					// 线程工作者是否已经ready
	bool m_waitto_open_flag_;		//是否正在等待连接
};

class HsrSerialPortLinkConfig;
/*
*@brief 串口工作类
*/
class HsrSerialPortLinkWorker : public QObject
{
	Q_OBJECT
public:
    explicit HsrSerialPortLinkWorker(QObject* parent = nullptr);
    ~HsrSerialPortLinkWorker();

	//! 设置串口连接配置
    void setLinkConfig(HsrSerialPortLinkConfig* cfg);

	//! 是否打开
	bool isOpen();

public /*slots*/:
	/*
	*@brief 打开串口设备
	*@emit serialportOpenResp
	*/
	void openLink();

	/*
	*@brief 关闭串口设备
	*/
	void closeLink();

	//! 注册数据接收回调函数
	void registerRecvdDataHandleFunc(ReceivedDataHandleFunc func);

	//! 取消注册数据接收回调函数
	void unregisterRecvdDataHandleFunc();

	/*
	*@brief 发送数据
	*/
	void sendData(QByteArray data);

protected:
	void onWriteTimout();

protected slots:
	void readBytes();

signals:
	//! 传递从串口接收到的数据
	void bytesReceived(QByteArray data);

	//! 通知串口的打开回复
	//! success: true 成功; false 失败
	void serialportOpenResp(bool success);

	//! 通知串口设备已关闭
	void serialPortClosed();

private:
	QSerialPort* m_serialport;

    HsrSerialPortLinkConfig* m_serialport_link_cfg_;

	QString m_error_string;

    ReceivedDataHandleFunc m_recvd_data_handle_func_;

	QFile* m_save_send_file_;

	QFile* m_save_recv_file_;

	std::atomic<bool> m_is_open_;				//记录通讯设备是否开启
};


#endif
