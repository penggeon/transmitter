#include "stm32f10x.h"                  // Device header

/**
 * @brief  USART1初始化
 * @param  BaudRate: 波特率
 * @retval 无
*/
void Usart1_Init(uint32_t BaudRate)
{
  /*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);	// 开启USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  // 开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// 将PA9引脚初始化为复用推挽输出
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// 将PA10引脚初始化为上拉输入
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					  // 定义结构体变量
	USART_InitStructure.USART_BaudRate = BaudRate;		// 波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	// 模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		          // 奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	        // 停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		  // 字长，选择8位
	USART_Init(USART1, &USART_InitStructure);				    // 将结构体变量交给USART_Init，配置USART1
	
	/*中断输出配置*/
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);			// 开启串口接收数据的中断
	// USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);			// 开启串口接收数据的中断
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					      // 定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;		// 选择配置NVIC的USART1线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// 指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;		// 指定NVIC线路的抢占优先级为0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		// 指定NVIC线路的响应优先级为0
	NVIC_Init(&NVIC_InitStructure);							// 将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*USART使能*/
	USART_Cmd(USART1, ENABLE);								// 使能USART1，串口开始运行
}

/**
 * @brief  USART2初始化
 * @param  无
 * @retval 无
*/
void Usart2_Init(void)
{
  /*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	// 开启USART2的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	  // 开启GPIOA的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// 将PA2引脚初始化为复用推挽输出
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);					// 将PA3引脚初始化为上拉输入
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					  // 定义结构体变量
	USART_InitStructure.USART_BaudRate = 9600;				// 波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	// 模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		          // 奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	        // 停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		  // 字长，选择8位
	USART_Init(USART2, &USART_InitStructure);				// 将结构体变量交给USART_Init，配置USART2
	
	/*中断输出配置*/
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);			// 开启串口接收数据的中断
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					// 定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;		// 选择配置NVIC的USART2线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// 指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;		// 指定NVIC线路的抢占优先级为2
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		// 指定NVIC线路的响应优先级为2
	NVIC_Init(&NVIC_InitStructure);							// 将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*USART使能*/
	USART_Cmd(USART2, ENABLE);								// 使能USART2，串口开始运行
}

/**
 * @brief  USART3初始化
 * @param  BaudRate: 波特率
 * @retval 无
*/
void Usart3_Init(uint32_t BaudRate)
{
  /*开启时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	// 开启USART3的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	  // 开启GPIOB的时钟
	
	/*GPIO初始化*/
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					// 将PB10引脚初始化为复用推挽输出
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);					// 将PB11引脚初始化为上拉输入
	
	/*USART初始化*/
	USART_InitTypeDef USART_InitStructure;					  // 定义结构体变量
	USART_InitStructure.USART_BaudRate = BaudRate;				// 波特率
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	// 硬件流控制，不需要
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;	// 模式，发送模式和接收模式均选择
	USART_InitStructure.USART_Parity = USART_Parity_No;		          // 奇偶校验，不需要
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	        // 停止位，选择1位
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;		  // 字长，选择8位
	USART_Init(USART3, &USART_InitStructure);				// 将结构体变量交给USART_Init，配置USART3
	
	/*中断输出配置*/
	// USART_ITConfig(USART3, USART_IT_RXNE|USART_IT_IDLE, ENABLE);			// 开启串口接收数据的中断
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);			// 开启串口接收数据的中断
	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);			// 开启串口接收数据的中断
	
	/*NVIC配置*/
	NVIC_InitTypeDef NVIC_InitStructure;					// 定义结构体变量
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;		// 选择配置NVIC的USART3线
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			// 指定NVIC线路使能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		// 指定NVIC线路的抢占优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		// 指定NVIC线路的响应优先级为1
	NVIC_Init(&NVIC_InitStructure);							// 将结构体变量交给NVIC_Init，配置NVIC外设
	
	/*USART使能*/
	USART_Cmd(USART3, ENABLE);								// 使能USART3，串口开始运行
}
