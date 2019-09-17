#ifndef HSRABSTRACTLINK_H
#define HSRABSTRACTLINK_H

#include <QThread>
#include <memory>
#include <functional>
#include "hsrcore_global.h"

// 链路数据回调函数
typedef std::function<void(const QByteArray& data)> ReceivedDataHandleFunc;

// udp通讯时候，暂时以ip来区分设备
typedef std::function<void(QString ip, QByteArray data)> UdpReceivedDataHandleFunc;

class HsrLinkAbstractConfig;
class HSRCORE_EXPORT HsrAbstractLink : public QObject
{
	Q_OBJECT

public:
	enum LinkOperateState {
		LINK_UNKNOW = 0,			//未知
		LINK_OPEN_FAILED = 1,		//打开失败
		LINK_OPEN_SUCCESSED = 2,	//打开成功
		LINK_CLOSED = 3,			//已关闭
	};
    explicit HsrAbstractLink(QObject* parent = nullptr);
    virtual ~HsrAbstractLink();

	virtual bool start() = 0;

	//! 打开连接
    virtual void open(const std::shared_ptr<HsrLinkAbstractConfig>& linkConfigPtr) = 0;

	//! 关闭连接
	virtual void close() = 0;

	//! 注册数据接收回调函数
	virtual void registerRecvdDataHandleFunc(ReceivedDataHandleFunc func);

	//! 取消注册数据接收回调函数
	virtual void unregisterRecvdDataHandleFunc();

	//! 是否已经打开
	virtual bool isOpen();

	//! 发送信息
	//! data 发送消息内容  消息长度
	virtual bool sendData(const QByteArray& data) = 0;

	//! 返回连接配置指针
    HsrLinkAbstractConfig* linkConfig();

signals:
	//! 准备工作信号,所有的操作都是基于该动作成功发出之后才可以进行
	void ready(bool success);

	//! 收到串口数据时候发出
	void dataReceived(QByteArray data);

	//! 打开连接结果
	//! opened: true 打开成功; false 打开失败
	void openLinkResp(bool opened);

	//! 通知连接已关闭
	void linkClosed();

protected slots:
	//响应打开回应
	void _respOpenLink(bool opened);

	//! 响应关闭连接
	void _respCloseLink();

protected:
    std::shared_ptr<HsrLinkAbstractConfig> m_linkconfig_ptr_;

	bool m_isopen_;					//记录link是否已经打开

	ReceivedDataHandleFunc m_recvd_data_handle_func_;

};

#endif

