#include "hsrtcpsocketlink.h"
#include "hsrtcpsocketlinkthread.h"


HsrTcpSocketLink::HsrTcpSocketLink(QObject *parent)
    : HsrAbstractLink(parent)
{
    m_link_thread_ = new HsrTcpSocketLinkThread(this);

    connect(m_link_thread_, &HsrTcpSocketLinkThread::workerReady, this, &HsrTcpSocketLink::ready);
    connect(m_link_thread_, &HsrTcpSocketLinkThread::tcpSocketOpenResp, this, &HsrTcpSocketLink::_respOpenLink);
    connect(m_link_thread_, &HsrTcpSocketLinkThread::tcpSocketClosed, this, &HsrTcpSocketLink::_respCloseLink);
    //connect(m_link_thread_, &TcpSocketLinkThread::bytesReceived, this, &HsrTcpSocketLink::dataReceived);
	//m_link_thread_->start();
}

HsrTcpSocketLink::~HsrTcpSocketLink()
{
	if (m_link_thread_->isRunning()) {
		m_link_thread_->quit();
		m_link_thread_->wait(500);
	}
}

bool HsrTcpSocketLink::start()
{
	m_link_thread_->start();
	return true;
}

void HsrTcpSocketLink::open(const std::shared_ptr<HsrLinkAbstractConfig>& linkConfig)
{
    m_link_thread_->setLinkConfig(linkConfig);
	m_link_thread_->openLink();
}

void HsrTcpSocketLink::close()
{
	m_link_thread_->closeLink();
}

bool HsrTcpSocketLink::sendData(const QByteArray& data)
{
	return m_link_thread_->sendData(data);
}

void HsrTcpSocketLink::registerRecvdDataHandleFunc(ReceivedDataHandleFunc func)
{
	if (m_link_thread_)
		m_link_thread_->registerRecvdDataHandleFunc(func);

    HsrAbstractLink::registerRecvdDataHandleFunc(func);
}

void HsrTcpSocketLink::unregisterRecvdDataHandleFunc()
{
	if (m_link_thread_)
		m_link_thread_->unregisterRecvdDataHandleFunc();

    HsrAbstractLink::unregisterRecvdDataHandleFunc();
}

bool HsrTcpSocketLink::workerIsReady() const
{
	return m_link_thread_->workerIsReady();
}

