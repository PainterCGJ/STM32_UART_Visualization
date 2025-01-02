/************************************************************
 * \file    bare_heap4.c
 * \brief   heap4内存管理，改自freertos，适用于裸机的内存管理，8字节对齐
 * 			通过 configTOTAL_HEAP_SIZE 设置heap大小
 * \version 1
 * \author  chen.guojin2@byd.com
 * 
 * Revision history
 * Date         Version     Author      Discription
 * 2023-9       0.0.1		Painter
 * 2024-10		0.0.2       Painter
 * 
 * Par
 * \warning
 * \copyright  CC 4.0 BY-SA 
 ************************************************************/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/**
 * Queue
 * @brief     队列结构体
 * @author    Painter
 * @date      2023-9
 * @version 2.0
 **/
typedef struct
{
	uint32_t front;			/* 队列头部元素索引值 */
	uint32_t rear;			/* 对列尾部元素索引值 */
	void *   data;			/* 队列存储空间基地址 */
	uint32_t ItemSize;		/* 队列中单个元素的大小 */
	uint32_t capacity;		/* 总容量 */
	uint32_t occupied_len;	/* 已占用容量 */
}Queue_t;

typedef Queue_t* Queue;

/**
 * Queue_Creat
 * @brief     创造新的队列
 * @param     capacity，创造队列的容量
 * @param     ItemSize，队列中单个元素的大小
 * @return    创建的队列指针
 * @exception none
 * @author    Painter
 * @date      2023-9
 * @version 2.0 
 **/
Queue Queue_Creat(uint16_t capacity, uint16_t ItemSize);

/**
 * Queue_isEmpty
 * @brief     判断队列是否为空
 * @param     pxQueue，队列
 * @return    1：队列空；0：队列非空
 * @exception none
 * @author    Painter
 * @date      2023-6
 * @version 1.0
 **/
uint8_t Queue_isEmpty(Queue pxQueue);

/**
 * Queue_isFull
 * @brief     判断队列是否已满
 * @param     pxQueue，队列
 * @return    1：队列已满；0：队列非满
 * @exception none
 * @author    Painter
 * @date      2023-6
 * @version 1.0
 **/
uint8_t Queue_isFull(Queue pxQueue);

/**
 * Queue_Occupied_Length
 * @brief     查询队列长度
 * @param     pxQueue，队列
 * @return    已占用长度
 * @exception none
 * @author    Painter
 * @date      2023-6
 * @version 1.0
 **/
uint32_t Queue_Occupied_Length(Queue pxQueue);

/**
 * Queue_Push
 * @brief     入队
 * @param     pxQueue，队列
 * @param     data，入队元素地址
 * @return    0：入队成功；1：入队失败
 * @exception none
 * @author    Painter
 * @date      2023-9
 * @version 2.0
 **/
uint8_t Queue_Push(Queue pxQueue, void* data);

/**
 * Queue_Push_Overwrite
 * @brief     覆写入队，若满队，则自动覆写
 * @param     pxQueue，队列
 * @param     data，入队元素地址
 * @return    0：覆写；1：没有覆写
 * @exception none
 * @author    Painter
 * @date      2023-9
 * @version 2.0
 **/
uint8_t Queue_Push_Overwrite(Queue pxQueue, void* data);

/**
 * Queue_Pull
 * @brief     出队
 * @param     pxQueue，队列
 * @param     data，出队的数据存放地址
 * @return    0：出队成功；1：出队失败
 * @exception none
 * @author    Painter
 * @date      2023-9
 * @version 2.0
 **/
uint8_t Queue_Pop(Queue pxQueue, void* data);

/**
 * Queue_Push_Items
 * @brief     入队若干元素
 * @param     pxQueue  队列
 * @param     buffer   入队缓冲区地址
 * @param	  item_num 入队元素个数
 * @return    0：入队成功；1：入队失败
 * @exception none
 * @author    Painter
 * @date      2024-10
 * @version 2.0
 **/
uint8_t Queue_Push_Items(Queue pxQueue,void* buffer,uint32_t item_num);

/**
 * Queue_Pop_Items
 * @brief     从队列中取出若干元素
 * @param     pxQueue  队列
 * @param     buffer   目标缓冲区地址
 * @param	  item_num 取出元素个数
 * @return    0：出队成功；1：出队失败
 * @exception none
 * @author    Painter
 * @date      2024-10
 * @version 2.0
 **/
uint8_t Queue_Pop_Items(Queue pxQueue,void* buffer,uint32_t item_num);

void Queue_Delet(Queue pxQueue);

void* Queue_Index(Queue pxQueue,uint32_t index);
