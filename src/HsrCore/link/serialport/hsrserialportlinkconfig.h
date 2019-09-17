#ifndef SERIALPORTLINKCONFIG_H
#define SERIALPORTLINKCONFIG_H

#include "HsrCore/hsrcore_global.h"
#include "HsrCore/link/hsrlinkabstractconfig.h"

/*
*@brief 串口连接配置
*/

class HSRCORE_EXPORT HsrSerialPortLinkConfig : public HsrLinkAbstractConfig
{
	Q_OBJECT

public:
    explicit HsrSerialPortLinkConfig(QObject *parent = nullptr);
    ~HsrSerialPortLinkConfig() override;

	//! 连接类型 serialport
    HsrLinkAbstractConfig::LinkType linkType() override;

	QString portName() const;

	void setPortName(const QString& pname);

	//这里相关的类型参数之所以不使用Qt串口中的数据类型或者枚举，是为了底层兼容使用
	//各个系统自带的串口操作接口
	//!波特率
	int baundrate() const;

	//! 可以支持 QSerialPort::BaudRate
	void setBaudrate(int rate);

	//! 数据位
	int dataBits() const;

	//! 可以支持 QSerialPort::DataBits
	void setDataBits(int data_bits);

	//! 流控制
	int flowControl() const;

	//! 可以支持 QSerialPort::FlowControl 
	void setFlowControl(int flow_control);

	//校验
	int parity() const;

	//! 可以支持 QSerialPort::Parity  
	void setParity(int parity);

	//! 停止位
	int stopBits() const;

	//! 可以支持 QSerialPort::StopBits
	void setStopBits(int stop_bits);
	
    void copyFrom(HsrLinkAbstractConfig* other) override;

private:
	QString m_portname;
	int m_baudrate;					//波特率
	int m_databits;					//数据位
	int m_flow_control;				//流控制
	int m_parity;					//校验
	int m_stopbits;					//停止位
};

#endif

