#include "stm32f10x.h"                  // Device header
#include "dht11.h"
#include "delay.h"
#include "Serial.h"
#define debug_mode 0	// 1: �򿪵���ģʽ, 0: �رյ���ģʽ

/**
  * @brief  ��ʼ��DHT11
	* @param  ��
	* @retval ��
	*/
void DH11_GPIO_Init(void)
{
	// ����APB2�е�GPIOBʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	// ����GPIO PB12
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_OD; // ��©���
	GPIO_InitStructure.GPIO_Pin   = DHT11_GPIO_Pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	GPIO_Init(DHT11_GPIO_Port, &GPIO_InitStructure);

	#if debug_mode
	Serial_Printf("DHT11��ʼ���ɹ�, ��ǰΪ: ��©���ģʽ\r\n");
	#endif
}

/**
  * @brief  ��λ�ͼ����Ӧ
	* @param  ��
	* @retval 1:��Ӧ�ɹ�, 0:��Ӧʧ��
	*/
uint8_t DHT11RstAndCheck(void)
{
	uint8_t timer = 0;		// ������

	__set_PRIMASK(1);     // �����ж�
	dht11_low; 	          // ����͵�ƽ
	Delay_ms(20);    	    // ��������18ms
	dht11_high; 	        // ����ߵ�ƽ
	Delay_us(30);     	  // ����20~40us
	
	// ����Ƿ���ڵ�һ���͵�ƽ
	while (!DHT11_IN)     // �ȴ��������ͣ�DHT11������40~80us��Ϊ��Ӧ�ź�
	{
		timer++;            // ��������ʱ����
		Delay_us(1);
	}
	if (timer>100 || timer<20) // �ж���Ӧʱ��
	{
		__set_PRIMASK(0);      // �����ж�
		return 0;
	}

	// ����Ƿ���ڵڶ����ߵ�ƽ
	timer = 0;	// ���ü�����
	while (DHT11_IN)          // �ȴ�DHT11�ͷ����ߣ�����ʱ��40~80us
	{
		timer++;               	// ��������ʱ����
		Delay_us(1);
	}
	__set_PRIMASK(0);          // �����ж�
	if (timer>100 || timer<20) // �����Ӧ�ź�֮��ĸߵ�ƽ
	{
		return 0;
	}

	// ������, �򷵻�1, ��Ӧ����
	return 1;
}

/**
  * @brief  ��ȡһ�ֽ�����
	* @param  ��
	* @retval ����������
	*/
uint8_t DHT11ReadByte(void)
{
	uint8_t i;
	uint8_t byt = 0;

	__set_PRIMASK(1);      // �����ж�
	for (i=0; i<8; i++)
	{
		while (DHT11_IN);  // �ȴ��͵�ƽ������λǰ����50us�͵�ƽʱ϶

		while (!DHT11_IN); // �ȴ��ߵ�ƽ����ʼ��������λ

		Delay_us(40);
		byt <<= 1;         // ���λ��ǰ����������byt�����λ��0
		if (DHT11_IN)      // �����ߵ�ƽֵ��ȡ��byt���λ��
		{
			byt |= 0x01;
		}
	}
	__set_PRIMASK(0);      // �����ж�

	return byt;
}

/**
  * @brief  ��ȡ����
	* @param  Temp_H �¶���������
	* @param  Temp_L �¶�С������
  * @param  Humi_H ʪ����������
  * @param  Humi_L ʪ��С������
	* @retval 0-�ɹ���1-ʧ��
	*/
uint8_t DHT11ReadData(uint8_t *Temp_H,uint8_t *Temp_L,uint8_t *Humi_H,uint8_t *Humi_L)
{
	uint8_t sta = 0;
	uint8_t i;
	uint8_t buf[5];

	if (DHT11RstAndCheck())       // �����Ӧ�ź�
	{
		for(i=0;i<5;i++)            // ��ȡ40λ����
		{
			buf[i]=DHT11ReadByte(); 	// ��ȡ1�ֽ�����
		}
		if(buf[0]+buf[1]+buf[2]+buf[3] == buf[4]) // У��ɹ�
		{
			*Humi_H = buf[0]; // ʪ�� ������������
			*Humi_L = buf[1]; // ʪ�� С����������
			*Temp_H = buf[2]; // �¶� ������������
			*Temp_L = buf[3]; // �¶� С����������
		}
		sta = 0;
	}
	else // ��Ӧʧ�ܷ���-1
	{
		*Temp_H = 0;
		*Temp_L = 0;
		*Humi_H = 0;
		*Humi_L = 0;
		sta = 1;
	}

	return sta;    
}
