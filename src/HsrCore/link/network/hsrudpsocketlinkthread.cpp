#include <QUdpSocket>
#include <QDateTime>
#include "hsrudpsocketlinkthread.h"
#include "hsrudpsocketlinkconfig.h"

HsrUdpSocketLinkThread::HsrUdpSocketLinkThread(QObject *parent) 
	: QThread(parent)
	, m_ready_(false)
	, m_waitto_open_flag_(false)
	, m_link_worker(nullptr)
{
	connect(this, &HsrUdpSocketLinkThread::workerReady, this, &HsrUdpSocketLinkThread::onWorkerReady);
}

void HsrUdpSocketLinkThread::setLinkConfig(const std::shared_ptr<HsrLinkAbstractConfig> &cfgPtr)
{
    m_udpsocket_link_cfg_shared_ = cfgPtr;
    if(m_link_worker)
        m_link_worker->setLinkConfig(static_cast<HsrUdpSocketLinkConfig*>(m_udpsocket_link_cfg_shared_.get()));

}

void HsrUdpSocketLinkThread::openLink()
{
	if(m_ready_)
		emit _invokeOpen();
	else
	{
		m_waitto_open_flag_ = true;
		this->start();
	}
}

void HsrUdpSocketLinkThread::closeLink()
{
    emit _invokeClose();
}

bool HsrUdpSocketLinkThread::sendData(const char *bytes, int len)
{
    if (!m_link_worker)
        return false;

    emit _invokeSendData(QByteArray(bytes, len));
    return true;
}

bool HsrUdpSocketLinkThread::sendData(const QByteArray &bytes)
{
    if (!m_link_worker)
        return false;

    emit _invokeSendData(bytes);
    return true;
}

bool HsrUdpSocketLinkThread::sendDatagram(const QByteArray &bytes, QStringList iplist, QList<quint16> portlist)
{
    if(!m_link_worker)
        return false;

    emit _invokeSendDatagram(bytes, iplist, portlist);
    return true;
}

void HsrUdpSocketLinkThread::registerRecvdDataHandleFunc(ReceivedDataHandleFunc func)
{
    m_recvd_data_handle_func_ = func;
}

void HsrUdpSocketLinkThread::unregisterRecvdDataHandleFunc()
{
    m_recvd_data_handle_func_ = nullptr;
}

void HsrUdpSocketLinkThread::registerRecvdDatagramHandleFunc(UdpReceivedDataHandleFunc func)
{
    m_recvd_datagram_handle_func_ = func;
}

void HsrUdpSocketLinkThread::unregisterRecvdDatagramHandleFunc()
{
    m_recvd_datagram_handle_func_ = nullptr;
}

bool HsrUdpSocketLinkThread::workerIsReady() const
{
	return m_ready_;
}

void HsrUdpSocketLinkThread::run()
{
    m_link_worker = new HsrUdpSocketLinkWorker();
    //! 设置连接配置
    m_link_worker->registerRecvdDataHandleFunc(m_recvd_data_handle_func_);
    m_link_worker->registerRecvdDatagramHandleFunc(m_recvd_datagram_handle_func_);

    connect(this, &HsrUdpSocketLinkThread::_invokeOpen, m_link_worker, &HsrUdpSocketLinkWorker::openLink);
    connect(m_link_worker, &HsrUdpSocketLinkWorker::udpSocketOpenResp, this, &HsrUdpSocketLinkThread::udpSocketOpenResp);
    connect(this, &HsrUdpSocketLinkThread::_invokeClose, m_link_worker, &HsrUdpSocketLinkWorker::closeLink);
    connect(m_link_worker, &HsrUdpSocketLinkWorker::udpSocketClosed, this, &HsrUdpSocketLinkThread::udpSocketClosed);
    connect(this, &HsrUdpSocketLinkThread::_invokeSendData, m_link_worker, &HsrUdpSocketLinkWorker::sendData);
    connect(this, &HsrUdpSocketLinkThread::_invokeSendDatagram, m_link_worker, &HsrUdpSocketLinkWorker::sendDatagram);
    //connect(m_link_worker, &TcpSocketLinkWorker::bytesReceived, this, &TcpSocketLinkThread::bytesReceived);

	m_ready_ = true;
    emit workerReady(true);
    exec();

	m_ready_ = false;
    m_link_worker->deleteLater();
    m_link_worker = nullptr;
}

void HsrUdpSocketLinkThread::onWorkerReady(bool success)
{
	if (!success) {
		qCritical() << "program error udpsocket linkworker not ready";
		return;
	}

	if (m_waitto_open_flag_) {
		m_waitto_open_flag_ = false;
		m_link_worker->setLinkConfig(static_cast<HsrUdpSocketLinkConfig*>(m_udpsocket_link_cfg_shared_.get()));
		emit _invokeOpen();
	}
}

HsrUdpSocketLinkWorker::HsrUdpSocketLinkWorker(QObject *parent)
    : QObject (parent)
    , m_udpsocket_link_cfg_(new HsrUdpSocketLinkConfig(9000, 9012, this))
    , m_recvd_data_handle_func_(nullptr)
{
    m_udpsocket_ = new QUdpSocket(this);

    connect(m_udpsocket_, &QIODevice::readyRead, this, &HsrUdpSocketLinkWorker::readBytes, Qt::DirectConnection);
}

HsrUdpSocketLinkWorker::~HsrUdpSocketLinkWorker()
{

}

void HsrUdpSocketLinkWorker::setLinkConfig(HsrUdpSocketLinkConfig *cfg)
{
    if (!cfg)
        return;

    m_udpsocket_link_cfg_->copyFrom(cfg);
}

void HsrUdpSocketLinkWorker::registerRecvdDataHandleFunc(ReceivedDataHandleFunc func)
{
    m_recvd_data_handle_func_ = func;
}

void HsrUdpSocketLinkWorker::unregisterRecvdDataHandleFunc()
{
    m_recvd_data_handle_func_ = nullptr;
}

void HsrUdpSocketLinkWorker::registerRecvdDatagramHandleFunc(UdpReceivedDataHandleFunc func)
{
    m_recvd_datagram_handle_func_ = func;
}

void HsrUdpSocketLinkWorker::unregisterRecvdDatagramHandleFunc()
{
    m_recvd_datagram_handle_func_ = nullptr;
}

void HsrUdpSocketLinkWorker::openLink()
{
	/*
	//设置参数
	m_tcpsocket_->connectToHost(m_tcpsocket_link_cfg_->address(), m_tcpsocket_link_cfg_->port());
	bool b = m_tcpsocket_->waitForConnected(8000);

	if (!b) {
	m_error_string = m_tcpsocket_->errorString();
	qDebug() << m_error_string;
	}

	emit tcpSocketOpenResp(b);
	*/
	if (!m_udpsocket_link_cfg_) {
		qCritical("udp socket not config");
		emit udpSocketOpenResp(false);
		return;
	}

    bool b = m_udpsocket_->bind(m_udpsocket_link_cfg_->recvPort(), QUdpSocket::ShareAddress | QAbstractSocket::ReuseAddressHint);
    if(!b)
        qCritical() << "udp bind port failed....";

    emit udpSocketOpenResp(b);
}

void HsrUdpSocketLinkWorker::closeLink()
{

}

void HsrUdpSocketLinkWorker::sendData(QByteArray data)
{
    if(!m_udpsocket_)
        return;

    // 这里需要设置接收方是预设的还是局域网内所有
    qint64 ret = m_udpsocket_->writeDatagram(data, QHostAddress::Broadcast, m_udpsocket_link_cfg_->sendPort());
    if(ret < 0)
        qCritical() << "write Datagram failed: " << data;
}

void HsrUdpSocketLinkWorker::sendDatagram(QByteArray data, QStringList iplist, QList<quint16> portlist)
{
    if(!m_udpsocket_)
        return;

    int len = iplist.length();
    if(len != portlist.length()){
        qCritical() << "sendDatagram failed: length of iplist not equal to portlist";
        return;
    }

    qint64 ret = -1;
    for(int i = 0; i < len; ++i){
        ret = m_udpsocket_->writeDatagram(data, QHostAddress(iplist[i]), portlist[i]);
        if(ret < 0)
          qCritical() << QString("write Datagram to host(%1:%2)failed: ").arg(iplist[i], portlist[i]);
    }
}

void HsrUdpSocketLinkWorker::readBytes()
{
    QByteArray buffer;
    while (m_udpsocket_->hasPendingDatagrams()) {
        buffer.resize(m_udpsocket_->pendingDatagramSize());
        QHostAddress senderAddress;
		senderAddress.clear();
        quint16 senderPort;
        m_udpsocket_->readDatagram(buffer.data(), buffer.size(), &senderAddress, &senderPort);
		//qDebug() << senderAddress << QDateTime::currentSecsSinceEpoch();
        if(m_recvd_datagram_handle_func_)
                m_recvd_datagram_handle_func_(QHostAddress(senderAddress.toIPv4Address()).toString(), buffer);
        buffer.clear();
        QThread::msleep(1);
    }
}
