#include "hsrudpsocketlink.h"
#include "hsrudpsocketlinkthread.h"

HsrUdpSocketLink::HsrUdpSocketLink(QObject *parent)
    : HsrAbstractLink (parent)
{
    m_link_thread_ = new HsrUdpSocketLinkThread(this);

    connect(m_link_thread_, &HsrUdpSocketLinkThread::workerReady, this, &HsrUdpSocketLink::ready);
    connect(m_link_thread_, &HsrUdpSocketLinkThread::udpSocketOpenResp, this, &HsrUdpSocketLink::_respOpenLink);
    connect(m_link_thread_, &HsrUdpSocketLinkThread::udpSocketClosed, this, &HsrUdpSocketLink::_respCloseLink);

}

HsrUdpSocketLink::~HsrUdpSocketLink()
{
    if (m_link_thread_->isRunning()) {
        m_link_thread_->quit();
        m_link_thread_->wait(500);
    }
}

bool HsrUdpSocketLink::start()
{
    m_link_thread_->start();
    return true;
}

void HsrUdpSocketLink::open(const std::shared_ptr<HsrLinkAbstractConfig> &linkConfig)
{
    m_link_thread_->setLinkConfig(linkConfig);
    m_link_thread_->openLink();
}

void HsrUdpSocketLink::close()
{
    m_link_thread_->closeLink();
}

bool HsrUdpSocketLink::sendData(const QByteArray &data)
{
    return m_link_thread_->sendData(data);
}

void HsrUdpSocketLink::registerRecvdDataHandleFunc(ReceivedDataHandleFunc func)
{
    if (m_link_thread_)
        m_link_thread_->registerRecvdDataHandleFunc(func);

    HsrAbstractLink::registerRecvdDataHandleFunc(func);
}

void HsrUdpSocketLink::unregisterRecvdDataHandleFunc()
{
    if (m_link_thread_)
        m_link_thread_->unregisterRecvdDataHandleFunc();

    HsrAbstractLink::unregisterRecvdDataHandleFunc();
}

void HsrUdpSocketLink::registerRecvdDatagramHandleFunc(UdpReceivedDataHandleFunc func)
{
    m_recvd_datagram_handle_func_ = func;
    if (m_link_thread_)
        m_link_thread_->registerRecvdDatagramHandleFunc(func);
}

void HsrUdpSocketLink::unregisterRecvdDatagramHandleFunc()
{
    m_recvd_datagram_handle_func_ = nullptr;
    if (m_link_thread_)
        m_link_thread_->unregisterRecvdDatagramHandleFunc();
}

bool HsrUdpSocketLink::workerIsReady() const
{
	return m_link_thread_->workerIsReady();
}
