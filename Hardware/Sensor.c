#include "stm32f10x.h"                  // Device header

uint32_t Timer_Count = 0;  // ��ʱ������ֵ
uint8_t Sensor_Tag = 0;    // ��������־λ
uint8_t Sensor_Stable = 0; // �������ȶ���־λ

/**
  * ��    ������������ʼ��
  * ��    ������
  * �� �� ֵ����
  */
void Sensor_Init(void)
{
	/*����ʱ��*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//����GPIOB��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);		//����AFIO��ʱ�ӣ��ⲿ�жϱ��뿪��AFIO��ʱ��
	
	/*GPIO��ʼ��*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//��PB0��PB1���ų�ʼ��Ϊ��������
	
	/*AFIOѡ���ж�����*/
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource0);//���ⲿ�жϵ�0����ӳ�䵽GPIOB����ѡ��PB0Ϊ�ⲿ�ж�����
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource1);//���ⲿ�жϵ�1����ӳ�䵽GPIOB����ѡ��PB1Ϊ�ⲿ�ж�����
	
	/*EXTI��ʼ��*/
	EXTI_InitTypeDef EXTI_InitStructure;						//����ṹ�����
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1;		//ѡ�������ⲿ�жϵ�0���ߺ�1����
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;					//ָ���ⲿ�ж���ʹ��
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//ָ���ⲿ�ж���Ϊ�ж�ģʽ
	EXTI_InitStructure.EXTI_Trigger =  EXTI_Trigger_Rising_Falling;		//ָ���ⲿ�ж���Ϊ�½��ش���
	EXTI_Init(&EXTI_InitStructure);								//���ṹ���������EXTI_Init������EXTI����
	
	/*NVIC�жϷ���*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);				//����NVICΪ����2
																//����ռ���ȼ���Χ��0~3����Ӧ���ȼ���Χ��0~3
																//�˷������������������н������һ��
																//���ж���жϣ����԰Ѵ˴������main�����ڣ�whileѭ��֮ǰ
																//�����ö�����÷���Ĵ��룬���ִ�е����ûḲ����ִ�е�����
	
	/*NVIC����*/
	NVIC_InitTypeDef NVIC_InitStructure;						//����ṹ�����
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//ѡ������NVIC��EXTI0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ָ��NVIC��·ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//ָ��NVIC��·����ռ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//ָ��NVIC��·����Ӧ���ȼ�Ϊ1
	NVIC_Init(&NVIC_InitStructure);								//���ṹ���������NVIC_Init������NVIC����

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//ѡ������NVIC��EXTI1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ָ��NVIC��·ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	//ָ��NVIC��·����ռ���ȼ�Ϊ1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;			//ָ��NVIC��·����Ӧ���ȼ�Ϊ2
	NVIC_Init(&NVIC_InitStructure);								//���ṹ���������NVIC_Init������NVIC����

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //����TIM3��ʱ��

	TIM_TimeBaseInitTypeDef TIM_InitStructure;
	TIM_InitStructure.TIM_Prescaler = (SystemCoreClock / 1000) - 1; // ��ʱ��ʱ��Ƶ��Ϊ1kHz
	TIM_InitStructure.TIM_Period = 1; // ��ʱ����Ϊ1ms
	TIM_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_InitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM3, &TIM_InitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM3, ENABLE);

  Adc_Init(); // ADC��ʼ��
}


/**
  * ��    ����ʱ��
  * ��    ������
  * �� �� ֵ����־λ�������
  */
void TIM3_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) {  // TIM3�жϴ���
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);       // ����TIM3�жϱ�־λ
        Timer_Count++;																		// �ۼ�
        
        if (Timer_Count >= 1000) {    // �������
            Sensor_Stable = 1;        // ����500ms����Ϊ1
        }
    }
}

/**
  * ��    ������������ȡ��־λ
  * ��    ������
  * �� �� ֵ�����ϴ˵��ô˺����󣬴������ı�־λ
  */
int16_t Sensor_Get(void)
{
	if(Sensor_Stable == 1){      //��־λ����������Χ�����ر�־λ
		
			return Sensor_Tag;
	}
	else{
			return 0;

	}
}


/**
  * ��    ����EXTI0�ⲿ�жϺ���
  * ��    ������
  * �� �� ֵ����
  * ע������˺���Ϊ�жϺ�����������ã��жϴ������Զ�ִ��
  *           ������ΪԤ����ָ�����ƣ����Դ������ļ�����
  *           ��ȷ����������ȷ���������κβ��죬�����жϺ��������ܽ���
  */
void EXTI0_IRQHandler(void)
{
			Timer_Count = 0; // ���ö�ʱ������
		Sensor_Stable = 0; // ���ô������ȶ���־λ 
	if (EXTI_GetITStatus(EXTI_Line0) == SET)		//�ж��Ƿ����ⲿ�ж�0���ߴ������ж�
	{		
		/*
			��B0�˿��ź���Ϊ��λ��⣬B1�˿��ź���Ϊ��λ���								
		*/
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0)     //����Թܳɹ��Խ��ź�ʱ�����ظߵ�ƽ
		{
																														//��־λΪ1ʱ����λ��ⴥ��
			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
			{
					Sensor_Tag = 0;	
			}
			else{
					Sensor_Tag = 1;	
			}
		}
		else{
			Sensor_Tag = 0;
		}
		EXTI_ClearITPendingBit(EXTI_Line0);			//����ⲿ�ж�0���ߵ��жϱ�־λ
													//�жϱ�־λ�������
													//�����жϽ��������ϵش�����������������
	}
}

/**
  * ��    ����EXTI1�ⲿ�жϺ���
  * ��    ������
  * �� �� ֵ����
  * ע������˺���Ϊ�жϺ�����������ã��жϴ������Զ�ִ��
  *           ������ΪԤ����ָ�����ƣ����Դ������ļ�����
  *           ��ȷ����������ȷ���������κβ��죬�����жϺ��������ܽ���
  */
void EXTI1_IRQHandler(void)
{
			Timer_Count = 0; // ���ö�ʱ������
		Sensor_Stable = 0; // ���ô������ȶ���־λ
	if (EXTI_GetITStatus(EXTI_Line1) == SET)		//�ж��Ƿ����ⲿ�ж�1���ߴ������ж�
	{
		/*��������������������󣬿��ٴ��ж����ŵ�ƽ���Ա��ⶶ��*/
		if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_1) == 0)
		{
					Sensor_Tag = 0;	
		}
		else{
			if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_0) == 0){
					Sensor_Tag = 1;	
			}
			else{
					Sensor_Tag = 0;	
			}
		}
		EXTI_ClearITPendingBit(EXTI_Line1);			//����ⲿ�ж�1���ߵ��жϱ�־λ
													//�жϱ�־λ�������
													//�����жϽ��������ϵش�����������������
	}
}
