#include <QSerialPort>
#include <QtGlobal>
#include <QIODevice>
#include <QDebug>
#include <QDir>
#include <QCoreApplication>
#include <ctime>
#include <chrono>
#include <thread>
#include "hsrserialportlinkthread.h"
#include "link/serialport/hsrserialportlink.h"
#include "link/serialport/hsrserialportlinkconfig.h"

HsrSerialPortLinkThread::HsrSerialPortLinkThread(QObject *parent)
	: QThread(parent)
	, m_link_worker(nullptr)
	, m_ready_(false)
	, m_waitto_open_flag_(false)
{
	connect(this, &HsrSerialPortLinkThread::workerReady, this, &HsrSerialPortLinkThread::onWorkerReady);
}

HsrSerialPortLinkThread::~HsrSerialPortLinkThread()
{

}

void HsrSerialPortLinkThread::setLinkConfig(const std::shared_ptr<HsrLinkAbstractConfig>& cfgPtr)
{
	m_serialport_link_cfg_shared_ = cfgPtr;
    if(m_link_worker)
        m_link_worker->setLinkConfig(static_cast<HsrSerialPortLinkConfig*>(m_serialport_link_cfg_shared_.get()));
}

void HsrSerialPortLinkThread::openLink()
{
	//这里判断worker是否已经创建
	if(m_ready_)
		emit _invokeOpen();
	else
	{
		m_waitto_open_flag_ = true;
		this->start();
	}
}

void HsrSerialPortLinkThread::closeLink()
{
	emit _invokeClose();
}

bool HsrSerialPortLinkThread::sendData(const char* bytes, int len)
{
	if (!m_link_worker)
		return false;
	
	emit _invokeSendData(QByteArray(bytes, len));
	return true;
}

bool HsrSerialPortLinkThread::sendData(const QByteArray& bytes)
{
	if (!m_link_worker)
		return false;

	emit _invokeSendData(bytes);
	return true;
}

void HsrSerialPortLinkThread::registerRecvdDataHandleFunc(ReceivedDataHandleFunc func)
{
	m_recvd_data_handle_func_ = func;
	if (m_link_worker)
		m_link_worker->registerRecvdDataHandleFunc(func);
}

void HsrSerialPortLinkThread::unregisterRecvdDataHandleFunc()
{
	if (m_link_worker)
		m_link_worker->unregisterRecvdDataHandleFunc();
	m_recvd_data_handle_func_ = nullptr;
}

bool HsrSerialPortLinkThread::linkIsOpen()
{
	if (!m_link_worker)
		return false;

	return m_link_worker->isOpen();
}

bool HsrSerialPortLinkThread::workerIsReady() const
{
	return m_ready_;
}

void HsrSerialPortLinkThread::run()
{
    m_link_worker = new HsrSerialPortLinkWorker();
	m_link_worker->registerRecvdDataHandleFunc(m_recvd_data_handle_func_);
	
    connect(this, &HsrSerialPortLinkThread::_invokeOpen, m_link_worker, &HsrSerialPortLinkWorker::openLink);
    connect(m_link_worker, &HsrSerialPortLinkWorker::serialportOpenResp, this, &HsrSerialPortLinkThread::serialPortOpenResp);
    connect(this, &HsrSerialPortLinkThread::_invokeClose, m_link_worker, &HsrSerialPortLinkWorker::closeLink);
    connect(m_link_worker, &HsrSerialPortLinkWorker::serialPortClosed, this, &HsrSerialPortLinkThread::serialPortClosed);
    connect(this, &HsrSerialPortLinkThread::_invokeSendData, m_link_worker, &HsrSerialPortLinkWorker::sendData);
	connect(m_link_worker, &HsrSerialPortLinkWorker::bytesReceived, this, &HsrSerialPortLinkThread::bytesReceived);

	//稍稍延时一点  防止调用者初始化尚未完成,后续采用其他方案
	m_ready_ = true;
	emit workerReady(true);
	exec();
	
	m_ready_ = false;
	m_link_worker->deleteLater();
	m_link_worker = nullptr;
}

void HsrSerialPortLinkThread::onWorkerReady(bool success)
{
	if (!success) {
		qCritical() << "program error serialport linkworker not ready";
		return;
	}

	if (m_waitto_open_flag_) {
		m_waitto_open_flag_ = false;
		m_link_worker->setLinkConfig(static_cast<HsrSerialPortLinkConfig*>(m_serialport_link_cfg_shared_.get()));
		emit _invokeOpen();
	}
}

HsrSerialPortLinkWorker::HsrSerialPortLinkWorker(QObject* parent)
	:QObject(parent)
    , m_serialport_link_cfg_(new HsrSerialPortLinkConfig(this))
	, m_recvd_data_handle_func_(nullptr)
	, m_is_open_(false)
{
	m_serialport = new QSerialPort(this);

    connect(m_serialport, &QIODevice::readyRead, this, &HsrSerialPortLinkWorker::readBytes, Qt::DirectConnection);
    connect(m_serialport, &QIODevice::aboutToClose, this, &HsrSerialPortLinkWorker::serialPortClosed, Qt::DirectConnection);
    //connect(&m_write_timer_, &QTimer::timeout, this, &HsrSerialPortLinkWorker::onWriteTimout);
	QString file_path = QCoreApplication::applicationDirPath() + "/data/origin";
	QDir dir;
	if (!dir.exists(file_path))
		dir.mkpath(file_path);

#ifdef ALLOW_SAVE_SERIAL_RECV_DATA
	m_save_recv_file_ = new QFile(file_path + "/serial-recv-data.txt", this);
	m_save_recv_file_->open(QFile::ReadWrite | QFile::Append);
#endif
#ifdef ALLOW_SAVE_SERIAL_SEND_DATA
	m_save_send_file_ = new QFile(file_path + "/serial-send-data.txt", this);
	m_save_send_file_->open(QFile::ReadWrite | QFile::Append);
#endif
}

HsrSerialPortLinkWorker::~HsrSerialPortLinkWorker()
{
    disconnect(m_serialport, &QIODevice::aboutToClose, this, &HsrSerialPortLinkWorker::serialPortClosed);
#ifdef ALLOW_SAVE_SERIAL_RECV_DATA
	m_save_recv_file_->flush();
	m_save_recv_file_->close();
#endif
#ifdef ALLOW_SAVE_SERIAL_SEND_DATA
	m_save_send_file_->flush();
	m_save_send_file_->close();
#endif

	if (m_serialport->isOpen())
		m_serialport->close();
}

void HsrSerialPortLinkWorker::setLinkConfig(HsrSerialPortLinkConfig * cfg)
{
	if (!cfg)
		return;

    m_serialport_link_cfg_->copyFrom(cfg);
}

bool HsrSerialPortLinkWorker::isOpen()
{
	return m_is_open_;
}

void HsrSerialPortLinkWorker::openLink()
{
	if (!m_serialport_link_cfg_) {
		qCritical("serial port not config");
		emit serialportOpenResp(false);
		return;
	}

	if (m_serialport->isOpen()) {
		emit serialportOpenResp(true);
		m_is_open_ = true;
		return;
	}

	//设置参数
	m_serialport->setBaudRate((QSerialPort::BaudRate)(m_serialport_link_cfg_->baundrate()));
	m_serialport->setDataBits((QSerialPort::DataBits)(m_serialport_link_cfg_->dataBits()));
	m_serialport->setFlowControl((QSerialPort::FlowControl)(m_serialport_link_cfg_->flowControl()));
	m_serialport->setParity((QSerialPort::Parity)(m_serialport_link_cfg_->parity()));
	m_serialport->setPortName(m_serialport_link_cfg_->portName());
	m_serialport->setStopBits((QSerialPort::StopBits)(m_serialport_link_cfg_->stopBits()));
	
	bool b = m_serialport->open(QIODevice::ReadWrite);
	if (!b) {
		m_is_open_ = false;
		m_error_string = m_serialport->errorString();
	}
	else
		m_is_open_ = true;

	emit serialportOpenResp(b);
}

void HsrSerialPortLinkWorker::closeLink()
{
	if (!m_serialport->isOpen())
		return;

	m_is_open_ = false;
	m_serialport->close();
}

void HsrSerialPortLinkWorker::registerRecvdDataHandleFunc(ReceivedDataHandleFunc func)
{
	m_recvd_data_handle_func_ = func;
}

void HsrSerialPortLinkWorker::unregisterRecvdDataHandleFunc()
{
	m_recvd_data_handle_func_ = nullptr;
}

void HsrSerialPortLinkWorker::sendData(QByteArray data)
{
	if (!m_serialport || !m_serialport->isOpen())
		return;

	//std::this_thread::sleep_for(std::chrono::milliseconds(20));

#ifdef ALLOW_SAVE_SERIAL_SEND_DATA
	m_save_send_file_->write(data.toHex());
	m_save_send_file_->write("\n");
#endif

    m_serialport->write(data);
}

void HsrSerialPortLinkWorker::readBytes()
{
	QByteArray buffer;

	buffer = m_serialport->readAll();
	
#ifdef ALLOW_SAVE_SERIAL_RECV_DATA
	m_save_recv_file_->write(buffer.toHex());
	m_save_recv_file_->write("\n");
#endif
	//qDebug() << buffer.toHex();
	if (m_recvd_data_handle_func_)
		m_recvd_data_handle_func_(buffer);
	
}


