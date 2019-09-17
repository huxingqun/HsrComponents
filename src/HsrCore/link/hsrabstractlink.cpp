#include <QThread>
#include "hsrabstractlink.h"

HsrAbstractLink::HsrAbstractLink(QObject* parent)
    : QObject (parent)
	, m_recvd_data_handle_func_(nullptr)
{
	//connect(this, &AbstractLink::_invokeSendData, this, &AbstractLink::sendDataByThreadSafe)
}

HsrAbstractLink::~HsrAbstractLink()
{

}

void HsrAbstractLink::registerRecvdDataHandleFunc(ReceivedDataHandleFunc func)
{
	m_recvd_data_handle_func_ = func;
}

void HsrAbstractLink::unregisterRecvdDataHandleFunc()
{
	m_recvd_data_handle_func_ = nullptr;
}
bool HsrAbstractLink::isOpen()
{
	return m_isopen_;
}
HsrLinkAbstractConfig * HsrAbstractLink::linkConfig()
{
	return m_linkconfig_ptr_.get();
}
void HsrAbstractLink::_respCloseLink()
{
	m_isopen_ = false;
	emit linkClosed();
}
void HsrAbstractLink::_respOpenLink(bool opened)
{
	m_isopen_ = opened;
	emit openLinkResp(opened);
}

