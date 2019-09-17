#include <QSerialPort>
#include "hsrserialportlinkconfig.h"

HsrSerialPortLinkConfig::HsrSerialPortLinkConfig(QObject *parent)
    : HsrLinkAbstractConfig(parent)
	, m_baudrate(QSerialPort::Baud115200)
	, m_databits(QSerialPort::Data8)
	, m_parity(QSerialPort::NoParity)
	, m_flow_control(QSerialPort::NoFlowControl)
	, m_stopbits(QSerialPort::OneStop)
{
}

HsrSerialPortLinkConfig::~HsrSerialPortLinkConfig()
{
}

HsrLinkAbstractConfig::LinkType HsrSerialPortLinkConfig::linkType()
{
    return HsrLinkAbstractConfig::SERIAL_PORT;
}

QString HsrSerialPortLinkConfig::portName() const
{
	return m_portname;
}

void HsrSerialPortLinkConfig::setPortName(const QString & pname)
{
	m_portname = pname;
}

int HsrSerialPortLinkConfig::baundrate() const
{
	return m_baudrate;
}

void HsrSerialPortLinkConfig::setBaudrate(int rate)
{
	m_baudrate = rate;
}

int HsrSerialPortLinkConfig::dataBits() const
{
	return m_databits;
}

void HsrSerialPortLinkConfig::setDataBits(int data_bits)
{
	m_databits = data_bits;
}

int HsrSerialPortLinkConfig::flowControl() const
{
	return m_flow_control;
}

void HsrSerialPortLinkConfig::setFlowControl(int flow_control)
{
	m_flow_control = flow_control;
}

int HsrSerialPortLinkConfig::parity() const
{
	return m_parity;
}

void HsrSerialPortLinkConfig::setParity(int parity)
{
	m_parity = parity;
}

int HsrSerialPortLinkConfig::stopBits() const
{
	return m_stopbits;
}

void HsrSerialPortLinkConfig::setStopBits(int stop_bits)
{
    m_stopbits = stop_bits;
}

void HsrSerialPortLinkConfig::copyFrom(HsrLinkAbstractConfig *other)
{
    HsrLinkAbstractConfig::copyFrom(other);
    HsrSerialPortLinkConfig* other_cfg = static_cast<HsrSerialPortLinkConfig*>(other);
    m_portname = other_cfg->portName();
    m_baudrate = other_cfg->baundrate();
    m_databits = other_cfg->dataBits();
    m_flow_control =other_cfg->flowControl();
    m_parity = other_cfg->parity();
    m_stopbits = other_cfg->stopBits();
}

