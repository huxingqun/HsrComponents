#include <QTcpSocket>
#include <QCoreApplication>
#include "hsrtcpsocketlinkthread.h"
#include "link/network/hsrtcpsocketlink.h"
#include "link/network/hsrtcpsocketlinkconfig.h"
#include <QDir>

HsrTcpSocketLinkThread::HsrTcpSocketLinkThread(QObject *parent)
	: QThread(parent)
	, m_link_worker(nullptr)
	, m_recvd_data_handle_func_(nullptr)
	, m_ready_(false)
	, m_waitto_open_flag_(false)
{
	connect(this, &HsrTcpSocketLinkThread::workerReady, this, &HsrTcpSocketLinkThread::onWorkerReady);
}

HsrTcpSocketLinkThread::~HsrTcpSocketLinkThread()
{
}

void HsrTcpSocketLinkThread::setLinkConfig(const std::shared_ptr<HsrLinkAbstractConfig>& cfgPtr)
{
	m_tcpsocket_link_cfg_shared_ = cfgPtr;
    if(m_link_worker)
        m_link_worker->setLinkConfig(static_cast<HsrTcpSocketLinkConfig*>(m_tcpsocket_link_cfg_shared_.get()));
}

void HsrTcpSocketLinkThread::openLink()
{
	if(m_ready_)
		emit _invokeOpen();
	else
	{
		m_waitto_open_flag_ = true;
		this->start();
	}
}

void HsrTcpSocketLinkThread::closeLink()
{
	emit _invokeClose();
}

bool HsrTcpSocketLinkThread::sendData(const char * bytes, int len)
{
	if (!m_link_worker)
		return false;

	emit _invokeSendData(QByteArray(bytes, len));
	return true;
}

bool HsrTcpSocketLinkThread::sendData(const QByteArray & bytes)
{
	if (!m_link_worker)
		return false;

	emit _invokeSendData(bytes);
	return true;
}

void HsrTcpSocketLinkThread::registerRecvdDataHandleFunc(ReceivedDataHandleFunc func)
{
	m_recvd_data_handle_func_ = func;
}

void HsrTcpSocketLinkThread::unregisterRecvdDataHandleFunc()
{
	m_recvd_data_handle_func_ = nullptr;
}

bool HsrTcpSocketLinkThread::workerIsReady() const
{
	return m_ready_;
}

HsrTcpSocketLinkWorker::HsrTcpSocketLinkWorker(QObject* parent)
	: QObject(parent)
    , m_tcpsocket_link_cfg_(new HsrTcpSocketLinkConfig(QHostAddress("127.0.0.1"), 2019, this))
	, m_recvd_data_handle_func_(nullptr)
{
	m_tcpsocket_ = new QTcpSocket(this);
	
    connect(m_tcpsocket_, &QIODevice::readyRead, this, &HsrTcpSocketLinkWorker::readBytes, Qt::DirectConnection);
    connect(m_tcpsocket_, &QIODevice::aboutToClose, this, &HsrTcpSocketLinkWorker::tcpSocketClosed, Qt::DirectConnection);

	QString file_path = QCoreApplication::applicationDirPath() + "/data/origin";
	QDir dir;
	if (!dir.exists(file_path))
		dir.mkpath(file_path);

#ifdef ALLOW_SAVE_TCP_RECV_DATA
	m_read_save_file_ = new QFile(file_path + "/tcp-recv-data.txt", this);
	m_read_save_file_->open(QFile::ReadWrite | QFile::Append);
#endif
#ifdef ALLOW_SAVE_TCP_SEND_DATA
	m_write_save_file_ = new QFile(file_path + "/tcp-send-data.txt", this);
	m_write_save_file_->open(QFile::ReadWrite | QFile::Append);
#endif
}

HsrTcpSocketLinkWorker::~HsrTcpSocketLinkWorker()
{
    disconnect(m_tcpsocket_, &QIODevice::aboutToClose, this, &HsrTcpSocketLinkWorker::tcpSocketClosed);
#ifdef ALLOW_SAVE_TCP_RECV_DATA
	m_read_save_file_->flush();
	m_read_save_file_->close();
#endif
#ifdef ALLOW_SAVE_TCP_SEND_DATA
	m_write_save_file_->flush();
	m_write_save_file_->close();
#endif
	if (m_tcpsocket_->isOpen()) {
		m_tcpsocket_->close();
	}
	else {
		m_tcpsocket_->abort();
	}
}

void HsrTcpSocketLinkWorker::setLinkConfig(HsrTcpSocketLinkConfig * cfg)
{
	if (!cfg)
		return;

    m_tcpsocket_link_cfg_->copyFrom(cfg);
}

void HsrTcpSocketLinkWorker::registerRecvdDataHandleFunc(ReceivedDataHandleFunc func)
{
	m_recvd_data_handle_func_ = func;
}

void HsrTcpSocketLinkWorker::unregisterRecvdDataHandleFunc()
{
	m_recvd_data_handle_func_ = nullptr;
}

void HsrTcpSocketLinkWorker::openLink()
{
	if (!m_tcpsocket_link_cfg_) {
        qCritical("tcp socket not config");
		emit tcpSocketOpenResp(false);
		return;
	}

	if (m_tcpsocket_->state() == QTcpSocket::ConnectedState) {
		emit tcpSocketOpenResp(true);
		return;
	}

	//设置参数
	qDebug() << m_tcpsocket_link_cfg_->address().toString() << m_tcpsocket_link_cfg_->port();
	m_tcpsocket_->connectToHost(m_tcpsocket_link_cfg_->address(), m_tcpsocket_link_cfg_->port());
	bool b = m_tcpsocket_->waitForConnected(8000);
	
	if (!b) {
		m_error_string = m_tcpsocket_->errorString();
		qDebug() << m_error_string;
	}
		
	emit tcpSocketOpenResp(b);
}

void HsrTcpSocketLinkWorker::closeLink()
{
	if (!m_tcpsocket_->isOpen()) {
		return;
	}

	m_tcpsocket_->disconnectFromHost();
	m_tcpsocket_->waitForDisconnected(3000);
	m_tcpsocket_->close();
	//emit tcpSocketClosed();
}

void HsrTcpSocketLinkWorker::sendData(QByteArray data)
{
	if (!m_tcpsocket_ || !m_tcpsocket_->isOpen())
		return;

#ifdef ALLOW_SAVE_TCP_SEND_DATA
	m_write_save_file_->write(data.toHex());
	m_write_save_file_->write("\n");
#endif
	//qDebug() << "send data:" << data.toHex();
	m_tcpsocket_->write(data);
}

void HsrTcpSocketLinkWorker::readBytes()
{
	QByteArray buffer;
	buffer = m_tcpsocket_->readAll();

#ifdef ALLOW_SAVE_TCP_RECV_DATA
	m_read_save_file_->write(buffer.toHex());
	m_read_save_file_->write("\n");
#endif
	if (m_recvd_data_handle_func_)
		m_recvd_data_handle_func_(buffer);
}

void HsrTcpSocketLinkThread::run()
{
    m_link_worker = new HsrTcpSocketLinkWorker();
	//! 设置连接配置
	m_link_worker->registerRecvdDataHandleFunc(m_recvd_data_handle_func_);

    connect(this, &HsrTcpSocketLinkThread::_invokeOpen, m_link_worker, &HsrTcpSocketLinkWorker::openLink);
    connect(m_link_worker, &HsrTcpSocketLinkWorker::tcpSocketOpenResp, this, &HsrTcpSocketLinkThread::tcpSocketOpenResp);
    connect(this, &HsrTcpSocketLinkThread::_invokeClose, m_link_worker, &HsrTcpSocketLinkWorker::closeLink);
    connect(m_link_worker, &HsrTcpSocketLinkWorker::tcpSocketClosed, this, &HsrTcpSocketLinkThread::tcpSocketClosed);
    connect(this, &HsrTcpSocketLinkThread::_invokeSendData, m_link_worker, &HsrTcpSocketLinkWorker::sendData);
	//connect(m_link_worker, &TcpSocketLinkWorker::bytesReceived, this, &TcpSocketLinkThread::bytesReceived);

	m_ready_ = true;
	emit workerReady(true);
	exec();

	m_ready_ = false;
	m_link_worker->deleteLater();
	m_link_worker = nullptr;
}

void HsrTcpSocketLinkThread::onWorkerReady(bool success)
{
	if (!success) {
		qCritical() << "program error tcpsocket linkworker not ready";
		return;
	}

	if (m_waitto_open_flag_) {
		m_waitto_open_flag_ = false;
		m_link_worker->setLinkConfig(static_cast<HsrTcpSocketLinkConfig*>(m_tcpsocket_link_cfg_shared_.get()));
		emit _invokeOpen();
	}
}
