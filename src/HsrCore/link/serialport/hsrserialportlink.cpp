#include <QThread>
#include "hsrserialportlink.h"
#include "link/hsrlinkabstractconfig.h"
#include "link/serialport/hsrserialportlinkthread.h"

HsrSerialPortLink::HsrSerialPortLink(QObject *parent)
    : HsrAbstractLink(parent)
{
    m_link_thread_ = new HsrSerialPortLinkThread(this);
    connect(m_link_thread_, &HsrSerialPortLinkThread::workerReady, this, &HsrSerialPortLink::ready);
    connect(m_link_thread_, &HsrSerialPortLinkThread::serialPortOpenResp, this, &HsrSerialPortLink::_respOpenLink);
    connect(m_link_thread_, &HsrSerialPortLinkThread::serialPortClosed, this, &HsrSerialPortLink::_respCloseLink);
    connect(m_link_thread_, &HsrSerialPortLinkThread::bytesReceived, this, &HsrSerialPortLink::dataReceived);
	//m_link_thread_->start();
}

HsrSerialPortLink::~HsrSerialPortLink()
{
	if (m_link_thread_->isRunning()) {
		m_link_thread_->quit();
		m_link_thread_->wait(500);
	}
}

bool HsrSerialPortLink::start()
{
	m_link_thread_->start();
	return true;
}

void HsrSerialPortLink::open(const std::shared_ptr<HsrLinkAbstractConfig>& linkConfig)
{
    m_link_thread_->setLinkConfig(linkConfig);
	//判断线程是否已经开启
	m_link_thread_->openLink();
}

void HsrSerialPortLink::close()
{
	m_link_thread_->closeLink();
}

bool HsrSerialPortLink::sendData(const QByteArray& data)
{
	return m_link_thread_->sendData(data);
}

void HsrSerialPortLink::registerRecvdDataHandleFunc(ReceivedDataHandleFunc func)
{
	if (m_link_thread_)
		m_link_thread_->registerRecvdDataHandleFunc(func);
    HsrAbstractLink::registerRecvdDataHandleFunc(func);
}

void HsrSerialPortLink::unregisterRecvdDataHandleFunc()
{
	if (m_link_thread_)
		m_link_thread_->unregisterRecvdDataHandleFunc();

    HsrAbstractLink::unregisterRecvdDataHandleFunc();
}

bool HsrSerialPortLink::workerIsReady() const
{
	return m_link_thread_->workerIsReady();
}


