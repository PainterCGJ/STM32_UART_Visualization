/************************************************************
 * \file    bare_heap4.c
 * \brief   heap4�ڴ��������freertos��������������ڴ����8�ֽڶ���
 * 			ͨ�� configTOTAL_HEAP_SIZE ����heap��С
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
 * @brief     ���нṹ��
 * @author    Painter
 * @date      2023-9
 * @version 2.0
 **/
typedef struct
{
	uint32_t front;			/* ����ͷ��Ԫ������ֵ */
	uint32_t rear;			/* ����β��Ԫ������ֵ */
	void *   data;			/* ���д洢�ռ����ַ */
	uint32_t ItemSize;		/* �����е���Ԫ�صĴ�С */
	uint32_t capacity;		/* ������ */
	uint32_t occupied_len;	/* ��ռ������ */
}Queue_t;

typedef Queue_t* Queue;

/**
 * Queue_Creat
 * @brief     �����µĶ���
 * @param     capacity��������е�����
 * @param     ItemSize�������е���Ԫ�صĴ�С
 * @return    �����Ķ���ָ��
 * @exception none
 * @author    Painter
 * @date      2023-9
 * @version 2.0 
 **/
Queue Queue_Creat(uint16_t capacity, uint16_t ItemSize);

/**
 * Queue_isEmpty
 * @brief     �ж϶����Ƿ�Ϊ��
 * @param     pxQueue������
 * @return    1�����пգ�0�����зǿ�
 * @exception none
 * @author    Painter
 * @date      2023-6
 * @version 1.0
 **/
uint8_t Queue_isEmpty(Queue pxQueue);

/**
 * Queue_isFull
 * @brief     �ж϶����Ƿ�����
 * @param     pxQueue������
 * @return    1������������0�����з���
 * @exception none
 * @author    Painter
 * @date      2023-6
 * @version 1.0
 **/
uint8_t Queue_isFull(Queue pxQueue);

/**
 * Queue_Occupied_Length
 * @brief     ��ѯ���г���
 * @param     pxQueue������
 * @return    ��ռ�ó���
 * @exception none
 * @author    Painter
 * @date      2023-6
 * @version 1.0
 **/
uint32_t Queue_Occupied_Length(Queue pxQueue);

/**
 * Queue_Push
 * @brief     ���
 * @param     pxQueue������
 * @param     data�����Ԫ�ص�ַ
 * @return    0����ӳɹ���1�����ʧ��
 * @exception none
 * @author    Painter
 * @date      2023-9
 * @version 2.0
 **/
uint8_t Queue_Push(Queue pxQueue, void* data);

/**
 * Queue_Push_Overwrite
 * @brief     ��д��ӣ������ӣ����Զ���д
 * @param     pxQueue������
 * @param     data�����Ԫ�ص�ַ
 * @return    0����д��1��û�и�д
 * @exception none
 * @author    Painter
 * @date      2023-9
 * @version 2.0
 **/
uint8_t Queue_Push_Overwrite(Queue pxQueue, void* data);

/**
 * Queue_Pull
 * @brief     ����
 * @param     pxQueue������
 * @param     data�����ӵ����ݴ�ŵ�ַ
 * @return    0�����ӳɹ���1������ʧ��
 * @exception none
 * @author    Painter
 * @date      2023-9
 * @version 2.0
 **/
uint8_t Queue_Pop(Queue pxQueue, void* data);

/**
 * Queue_Push_Items
 * @brief     �������Ԫ��
 * @param     pxQueue  ����
 * @param     buffer   ��ӻ�������ַ
 * @param	  item_num ���Ԫ�ظ���
 * @return    0����ӳɹ���1�����ʧ��
 * @exception none
 * @author    Painter
 * @date      2024-10
 * @version 2.0
 **/
uint8_t Queue_Push_Items(Queue pxQueue,void* buffer,uint32_t item_num);

/**
 * Queue_Pop_Items
 * @brief     �Ӷ�����ȡ������Ԫ��
 * @param     pxQueue  ����
 * @param     buffer   Ŀ�껺������ַ
 * @param	  item_num ȡ��Ԫ�ظ���
 * @return    0�����ӳɹ���1������ʧ��
 * @exception none
 * @author    Painter
 * @date      2024-10
 * @version 2.0
 **/
uint8_t Queue_Pop_Items(Queue pxQueue,void* buffer,uint32_t item_num);

void Queue_Delet(Queue pxQueue);

void* Queue_Index(Queue pxQueue,uint32_t index);
