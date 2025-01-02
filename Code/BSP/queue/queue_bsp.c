/**
 * Copyright (c) 2024 - present LibDriver All rights reserved
 * 
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 * \file    P_Queue.c
 * \brief   
 * \version 1.0.0
 * \author  chen.guojin2@byd.com
 * 
 * Revision history
 * Date         Version     Author      Discription
 * 2024-10-14   1.0.0       CGJ    
 * 
 * Par
 * \warning
 */

#include "P_Queue.h"
#include "bare_heap4.h"
Queue Queue_Creat(uint16_t capacity, uint16_t ItemSize)
{
	Queue temp = (Queue)pvPortMalloc(sizeof(Queue_t));
	//申请失败
	if (temp == NULL)
	{
		//printf("队列申请失败");
		return NULL;
	}

	temp->data = (void*)pvPortMalloc((capacity + 1)* ItemSize);
	if (temp->data == NULL)
	{
		vPortFree(temp);
		return NULL;
	}

	memset(temp->data, 0, capacity + 1);
	temp->front = 0;
	temp->rear = 0;
	temp->capacity = capacity + 1;
	temp->ItemSize = ItemSize;
	temp->occupied_len = 0;
	return temp;
}

uint8_t Queue_isEmpty(Queue pxQueue)
{
	if (pxQueue->front == pxQueue->rear) return 1;
	return 0;
}

uint8_t Queue_isFull(Queue pxQueue)
{
	if ((pxQueue->rear + 1) % pxQueue->capacity == pxQueue->front)return 1;
	return 0;
}

uint32_t Queue_Occupied_Length(Queue pxQueue)
{
	return pxQueue->occupied_len;
}

uint8_t Queue_Push(Queue pxQueue, void* data)
{
	if (!Queue_isFull(pxQueue))
	{
		//目标地址=列表数据基地址+地址偏移量
		//地址偏移量=列表尾索引值 * 单个列表元素的字节数
		memcpy((uint8_t*)pxQueue->data + (pxQueue->rear * pxQueue->ItemSize), data, pxQueue->ItemSize);
		pxQueue->rear = (pxQueue->rear + 1) % pxQueue->capacity;
		pxQueue->occupied_len++;
		return 0;
	}
	return 1;
}

uint8_t Queue_Pop(Queue pxQueue,void* data)
{
	if (!Queue_isEmpty(pxQueue))
	{
		//出队数据存放地址=队列数据基地址+队列头部元素地址偏移
		//队列头部元素地址偏移=队列头索引值 * 单个列表元素的字节数
		memcpy(data, (uint8_t*)pxQueue->data + (pxQueue->front * pxQueue->ItemSize), pxQueue->ItemSize);
		pxQueue->front = (pxQueue->front + 1) % pxQueue->capacity;
		pxQueue->occupied_len--;
		return 0;
	}
	else return 1;
}

uint8_t Queue_Push_Items(Queue pxQueue,void* buffer,uint32_t item_num)
{
	if(pxQueue->capacity - pxQueue->occupied_len - 1 >= item_num)
	{
		uint32_t push_num = item_num;
		uint8_t* pxData = (uint8_t*)buffer;
		while(push_num)
		{
			memcpy((uint8_t*)pxQueue->data + (pxQueue->rear * pxQueue->ItemSize), pxData, pxQueue->ItemSize);
			pxQueue->rear = (pxQueue->rear + 1) % pxQueue->capacity;
			pxData += pxQueue->ItemSize;
			pxQueue->occupied_len++;
			push_num--;
		}
		return 0;
	}
	return 1;
}

uint8_t Queue_Pop_Items(Queue pxQueue,void* buffer,uint32_t item_num)
{
	if(pxQueue->occupied_len >= item_num)
	{
		uint32_t pop_num = item_num;
		uint8_t* pxData = (uint8_t*)buffer;
		while(pop_num)
		{
			memcpy((void*)pxData, (uint8_t*)pxQueue->data + (pxQueue->front * pxQueue->ItemSize), pxQueue->ItemSize);
			pxQueue->front = (pxQueue->front + 1) % pxQueue->capacity;
			pxData += pxQueue->ItemSize;
			pxQueue->occupied_len--;
			pop_num--;
		}
		return 0;
	}
	return 1;
}

uint8_t Queue_Push_Overwrite(Queue pxQueue, void* data)
{
	if (!Queue_isFull(pxQueue))
	{
		memcpy((uint8_t*)pxQueue->data + (pxQueue->rear * pxQueue->ItemSize), data, pxQueue->ItemSize);
		pxQueue->rear = (pxQueue->rear + 1) % pxQueue->capacity;
		pxQueue->occupied_len++;
		return 1;
	}

	memcpy((uint8_t*)pxQueue->data + (pxQueue->rear * pxQueue->ItemSize), data, pxQueue->ItemSize);
	pxQueue->rear = (pxQueue->rear + 1) % pxQueue->capacity;
	pxQueue->front = (pxQueue->front + 1) % pxQueue->capacity;

	return 0;
}

void* Queue_Index(Queue pxQueue,uint32_t index_base_tail)
{
	uint32_t offset = (pxQueue->rear + pxQueue->capacity - (index_base_tail))% pxQueue->capacity;
	return ((uint8_t*)pxQueue->data + (offset * pxQueue->ItemSize));
}

void Queue_Delet(Queue pxQueue)
{
	vPortFree(pxQueue->data);
	vPortFree(pxQueue);
}
