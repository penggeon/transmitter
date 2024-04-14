#include "stm32f10x.h"                  // Device header

/**
 * @brief  USART1��ʼ��
 * @param  BaudRate: ������
 * @retval ��
*/
void Usart1_Init(uint32_t BaudRate)
{
  /*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// ����USART1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  // ����GPIOA��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// ��PA9���ų�ʼ��Ϊ�����������
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// ��PA10���ų�ʼ��Ϊ��������
	
	/*USART��ʼ��*/
	USART_InitTypeDef USART_InitStructure;					  // ����ṹ�����
	USART_InitStructure.USART_BaudRate = BaudRate;		// ������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// Ӳ�������ƣ�����Ҫ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	// ģʽ������ģʽ�ͽ���ģʽ��ѡ��
	USART_InitStructure.USART_Parity = USART_Parity_No;		          // ��żУ�飬����Ҫ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	        // ֹͣλ��ѡ��1λ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		  // �ֳ���ѡ��8λ
	USART_Init(USART1, &USART_InitStructure);				    // ���ṹ���������USART_Init������USART1
	
	/*�ж��������*/
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);			// �������ڽ������ݵ��ж�
	// USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			// �������ڽ������ݵ��ж�
	
	/*NVIC����*/
	NVIC_InitTypeDef NVIC_InitStructure;					      // ����ṹ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		// ѡ������NVIC��USART1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// ָ��NVIC��·ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		// ָ��NVIC��·����ռ���ȼ�Ϊ0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// ָ��NVIC��·����Ӧ���ȼ�Ϊ0
	NVIC_Init(&NVIC_InitStructure);							// ���ṹ���������NVIC_Init������NVIC����
	
	/*USARTʹ��*/
	USART_Cmd(USART1, ENABLE);								// ʹ��USART1�����ڿ�ʼ����
}

/**
 * @brief  USART2��ʼ��
 * @param  ��
 * @retval ��
*/
void Usart2_Init(void)
{
  /*����ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	// ����USART2��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  // ����GPIOA��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// ��PA2���ų�ʼ��Ϊ�����������
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// ��PA3���ų�ʼ��Ϊ��������
	
	/*USART��ʼ��*/
	USART_InitTypeDef USART_InitStructure;					  // ����ṹ�����
	USART_InitStructure.USART_BaudRate = 9600;				// ������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// Ӳ�������ƣ�����Ҫ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	// ģʽ������ģʽ�ͽ���ģʽ��ѡ��
	USART_InitStructure.USART_Parity = USART_Parity_No;		          // ��żУ�飬����Ҫ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	        // ֹͣλ��ѡ��1λ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		  // �ֳ���ѡ��8λ
	USART_Init(USART2, &USART_InitStructure);				// ���ṹ���������USART_Init������USART2
	
	/*�ж��������*/
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			// �������ڽ������ݵ��ж�
	
	/*NVIC����*/
	NVIC_InitTypeDef NVIC_InitStructure;					// ����ṹ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		// ѡ������NVIC��USART2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// ָ��NVIC��·ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;		// ָ��NVIC��·����ռ���ȼ�Ϊ2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		// ָ��NVIC��·����Ӧ���ȼ�Ϊ2
	NVIC_Init(&NVIC_InitStructure);							// ���ṹ���������NVIC_Init������NVIC����
	
	/*USARTʹ��*/
	USART_Cmd(USART2, ENABLE);								// ʹ��USART2�����ڿ�ʼ����
}

/**
 * @brief  USART3��ʼ��
 * @param  BaudRate: ������
 * @retval ��
*/
void Usart3_Init(uint32_t BaudRate)
{
  /*����ʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	// ����USART3��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	  // ����GPIOB��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					// ��PB10���ų�ʼ��Ϊ�����������
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					// ��PB11���ų�ʼ��Ϊ��������
	
	/*USART��ʼ��*/
	USART_InitTypeDef USART_InitStructure;					  // ����ṹ�����
	USART_InitStructure.USART_BaudRate = BaudRate;				// ������
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// Ӳ�������ƣ�����Ҫ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	// ģʽ������ģʽ�ͽ���ģʽ��ѡ��
	USART_InitStructure.USART_Parity = USART_Parity_No;		          // ��żУ�飬����Ҫ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	        // ֹͣλ��ѡ��1λ
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		  // �ֳ���ѡ��8λ
	USART_Init(USART3, &USART_InitStructure);				// ���ṹ���������USART_Init������USART3
	
	/*�ж��������*/
	// USART_ITConfig(USART3, USART_IT_RXNE|USART_IT_IDLE, ENABLE);			// �������ڽ������ݵ��ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);			// �������ڽ������ݵ��ж�
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);			// �������ڽ������ݵ��ж�
	
	/*NVIC����*/
	NVIC_InitTypeDef NVIC_InitStructure;					// ����ṹ�����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		// ѡ������NVIC��USART3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// ָ��NVIC��·ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		// ָ��NVIC��·����ռ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		// ָ��NVIC��·����Ӧ���ȼ�Ϊ1
	NVIC_Init(&NVIC_InitStructure);							// ���ṹ���������NVIC_Init������NVIC����
	
	/*USARTʹ��*/
	USART_Cmd(USART3, ENABLE);								// ʹ��USART3�����ڿ�ʼ����
}
