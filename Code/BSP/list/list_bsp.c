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
 * \file    P_List.c
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
#include "P_List.h"
#include "bare_heap4.h"
List List_Init( bool DesEnable)
{
	List xList = (List)pvPortMalloc(sizeof(__List));
	if(xList == NULL)
	{
		printf("fail to initial List");
		return NULL;
	}

	//��ʼ��ͷ�б���
	xList->HeadItem.index = 0;
	xList->HeadItem.value = 0;
	xList->HeadItem.pxElement = NULL;
	xList->HeadItem.pxPrevious = NULL;
	xList->HeadItem.pxNext = &( xList->EndItem);
	xList->HeadItem.pxOwener = &(xList);

	//��ʼ��β�б���
	xList->EndItem.index = 1;
	xList->EndItem.value = 0;
	xList->EndItem.pxElement = NULL;
	xList->EndItem.pxPrevious = &(xList->HeadItem);
	xList->EndItem.pxNext = NULL;
	xList->EndItem.pxOwener = &(xList);

	//����ͷβ�б��2���б����Ա
	xList->size = 2;

	//�Ƿ�ʹ���Զ�����
	xList->Descend = DesEnable;
	return xList;

}


uint8_t ListInsert_Des(List xList, int xvalue,void*pxElement)
{
	list_assert(xList->Descend);//���ԣ�û��ʹ���б���
	
	//ͷβ�������vlaue�����뽵�����У�cur������ָ��ָ��β�������ǰһ��������
	ListItem* cur = xList->EndItem.pxPrevious;
	cur->value = xList->EndItem.pxPrevious->value;
	

	//�����µ�������
	ListItem* newItem = pvPortMalloc(sizeof(ListItem));
	if (newItem == NULL)
	{
		printf("fail to insert ListItem in descending order");
		return 1;
	}

	//��ʼ������
	newItem->pxElement = pxElement;
	newItem->value = xvalue;
	newItem->pxOwener = xList;
	newItem->pxNext = NULL;
	newItem->pxPrevious = NULL;

	if (xList->size == 2)//�������г���ͷβ�û�б����
	{
		newItem->pxPrevious = &(xList->HeadItem);
		newItem->pxNext = &(xList->EndItem);
		newItem->index = 1;
		xList->HeadItem.pxNext = newItem;
		xList->EndItem.pxPrevious = newItem;
	}
	else
	{
		while (cur != &(xList->HeadItem))
		{

			//��������ڵ�ǰ�����
			if (newItem->value <= cur->value)
			{
				newItem->pxPrevious = cur;
				newItem->pxNext = cur->pxNext;
				newItem->index = cur->index + 1;
				cur->pxNext->pxPrevious = newItem;
				cur->pxNext = newItem;
				break;
			}

			//��������������û�з��ֱ�xvalue��С��ֵ�����������ͷ���
			if (cur->pxPrevious == &(xList->HeadItem))
			{
				newItem->pxPrevious = &(xList->HeadItem);
				newItem->pxNext = xList->HeadItem.pxNext;
				newItem->index = 1;
				cur->pxPrevious = newItem;
				xList->HeadItem.pxNext = newItem;
				cur->index++;
				break;
			}

			//xvalue��cur��value�󣬵�ǰcur������Ų������ֵ��һ
			cur->index++;

			//ָ��ǰһ�������һ�αȽ�
			cur = cur->pxPrevious;
		}	
	}
	xList->EndItem.index++;
	xList->size++;
	return 0;
}

uint8_t ListPush_Back(List xList, int xvalue, void* pxElement)
{
	list_assert(!xList->Descend);//���ԣ�ʹ�����б���ʹ���˽������������ֱ����ĩβ��������

	//�����µ�������
	ListItem* newItem = pvPortMalloc(sizeof(ListItem));
	if (newItem == NULL)
	{
		printf("fail to insert ListItem to List back");
		return 1;
	}

	//��ʼ������
	newItem->pxElement = pxElement;
	newItem->value = xvalue;
	newItem->pxOwener = xList;
	newItem->pxNext = &(xList->EndItem);
	newItem->pxPrevious = xList->EndItem.pxPrevious;
	xList->EndItem.pxPrevious->pxNext = newItem;
	xList->EndItem.pxPrevious = newItem;

	newItem->index = xList->EndItem.index;
	xList->EndItem.index++;

	xList->size++;
	return 0;
}

uint8_t ListPush_Front(List xList, int xvalue, void* pxElement)
{
	list_assert(!xList->Descend);//���ԣ�ʹ�����б���ʹ���˽������������ֱ����ͷ����������

	//�����µ�������
	ListItem* newItem = pvPortMalloc(sizeof(ListItem));
	if (newItem == NULL)
	{
		printf("fail to insert ListItem to List front");
		return 1;
	}

	//��ʼ������
	newItem->pxElement = pxElement;
	newItem->value = xvalue;
	newItem->pxOwener = xList;
	newItem->pxNext = xList->HeadItem.pxNext;
	newItem->pxPrevious = &(xList->HeadItem);
	xList->HeadItem.pxNext->pxPrevious = newItem;
	xList->HeadItem.pxNext = newItem;

	newItem->index = 1;
	for (ListItem* cur = newItem->pxNext; cur != NULL; cur = cur->pxNext)
		cur->index++;

	xList->size++;
	return 0;
}

uint8_t ListInsert_FollowIndex(List xList, uint8_t follow, int xvalue, void* pxElement)
{
	list_assert(!xList->Descend);//���ԣ�ʹ�����б���ʹ���˽������������ֱ����follow�����ź����

	ListItem* pre;
	for (pre = &(xList->HeadItem); pre != &(xList->EndItem); pre = pre->pxNext)
	{
		if (pre->index == follow)break;
	}
	if (pre == &(xList->EndItem))return 0;//������û��������Ϊfollow����

	ListItem* newItem = pvPortMalloc(sizeof(ListItem));
	if (newItem == NULL)
	{
		printf("fail to insert ListItem following index");
		return 1;
	}

	newItem->index = follow + 1;
	newItem->pxOwener = xList;
	newItem->pxElement = pxElement;
	newItem->value = xvalue;
	newItem->pxNext = pre->pxNext;
	newItem->pxPrevious = pre;
	pre->pxNext->pxPrevious = newItem;
	pre->pxNext = newItem;

	for (ListItem* cur = &(xList->EndItem); cur != newItem; cur = cur->pxPrevious)
	{
		cur->index++;
	}
	xList->size++;
	return 0;
}

uint8_t ListInsert_FollowItem(List xList, ListItem* pxfollow, int xvalue, void* pxElement)
{
	//list_assert(!xList->Descend);//���ԣ�ʹ�����б���ʹ���˽������������ֱ����follow�����ź����

	ListItem* pre;
	for (pre = &(xList->HeadItem); pre != &(xList->EndItem); pre = pre->pxNext)
	{
		if (pre == pxfollow)break;
	}
	if (pre == &(xList->EndItem))return 0;//������û�е�ַΪpxfollow��������

	ListItem* newItem = pvPortMalloc(sizeof(ListItem));
	if (newItem == NULL)
	{
		printf("fail to insert ListItem following item");
		return 1;
	}

	newItem->index = pre->index + 1;
	newItem->pxOwener = xList;
	newItem->pxElement = pxElement;
	newItem->value = xvalue;
	newItem->pxNext = pre->pxNext;
	newItem->pxPrevious = pre;
	pre->pxNext->pxPrevious = newItem;
	pre->pxNext = newItem;

	for (ListItem* cur = &(xList->EndItem); cur != newItem; cur = cur->pxPrevious)
	{
		cur->index++;
	}
	xList->size++;
	return 0;
}

ListItem* ListPop_Back(List xList)
{
	list_assert(xList->size > 2);//���ԣ�������û�б���������ˣ�������ȡ��������

	ListItem* temp = xList->EndItem.pxPrevious;

	temp->pxPrevious->pxNext = &(xList->EndItem);
	xList->EndItem.pxPrevious = temp->pxPrevious;

	xList->size--;
	xList->EndItem.index--;
	return temp;
	
}

ListItem* ListPop_Front(List xList)
{
	list_assert(xList->size > 2);//���ԣ�������û�б���������ˣ�������ȡ��������

	ListItem* temp = xList->HeadItem.pxNext;

	temp->pxNext->pxPrevious = &(xList->HeadItem);
	xList->HeadItem.pxNext = temp->pxNext;

	for (ListItem* cur = xList->HeadItem.pxNext; cur != NULL; cur = cur->pxNext)
	{
		cur->index--;
	}

	xList->size--;
	return temp;
}

ListItem* ListRemove_Index(List xList, uint32_t xindex)
{
	list_assert(xList->size > 2);//���ԣ�������û�б���������ˣ�������ȡ��������

	for (ListItem* cur = xList->EndItem.pxPrevious; cur != &(xList->HeadItem); cur = cur->pxPrevious)
	{
		if (cur->index == xindex)
		{
			cur->pxPrevious->pxNext = cur->pxNext;
			cur->pxNext->pxPrevious = cur->pxPrevious;
			xList->EndItem.index--;
			xList->size--;
			return cur;
		}
		cur->index--;
	}

	return NULL;
}

ListItem* ListRemove_Item(List xList, ListItem* pxItem)
{
	if (pxItem->pxOwener != xList)return NULL;
	for (ListItem* cur = xList->EndItem.pxPrevious; cur != &(xList->HeadItem); cur = cur->pxPrevious)
	{
		if (cur== pxItem)
		{
			cur->pxPrevious->pxNext = cur->pxNext;
			cur->pxNext->pxPrevious = cur->pxPrevious;
			xList->EndItem.index--;
			xList->size--;
			return cur;
		}
		cur->index--;
	}
    return NULL;
}

ListItem* ListGetItemAdd_ForIndex(List xList, uint32_t xindex)
{
	ListItem* tar;
	for (tar = &(xList->HeadItem); tar != NULL; tar = tar->pxNext)
	{
		if (tar->index == xindex)
			return tar;
	}
	return NULL;
}

void ListPrint(List const  xList)
{
	ListItem* cur = &(xList->HeadItem);
	printf("List's size = %d\r\n\r\n", xList->size);
	while (cur!= NULL)
	{
		
		if (cur == &(xList->HeadItem))
		{
			printf("-------- Head %d--------\r\n", cur->index);
			printf("pxItem     = %#x\r\n", (int)cur);
			printf("value      = %d\r\n", cur->value);
			printf("pxPrevious = None\r\n");
			printf("pxNext     = %#x\r\n", (int)cur->pxNext);
		}
		else if (cur == &(xList->EndItem))
		{
			printf("-------- End %d--------\r\n", cur->index);
			printf("pxItem     = %#x\r\n", (int)cur);
			printf("value      = %d\r\n", cur->value);
			printf("pxPrevious = %#x\r\n", (int)cur->pxPrevious);
			printf("pxNext     = NULL\r\n");
		}
		else
		{
			printf("---------Item %d---------\r\n", cur->index);
			printf("pxItem     = %#x\r\n", (int)cur);
			printf("value      = %d\r\n", cur->value);
			printf("pxElement  = %#x\r\n", (int)cur->pxElement);
			printf("pxPrevious = %#x\r\n", (int)cur->pxPrevious);
			printf("pxNext     = %#x\r\n", (int)cur->pxNext);
			printf("\r\n");
		}
		printf("------------------------\r\n");
		cur = cur->pxNext;
	}
	
}

void ItemPrint(ListItem* pxListItem)
{
	printf("--------ListItem--------\r\n");
	printf("index      = %d\r\n", pxListItem->index);
	printf("pxItem     = %#x\r\n", (int)pxListItem);
	printf("value      = %d\r\n", pxListItem->value);
	printf("pxElement  = %#x\r\n", (int)pxListItem->pxElement);
	printf("pxPrevious = %#x\r\n", (int)pxListItem->pxPrevious);
	printf("pxNext     = %#x\r\n", (int)pxListItem->pxNext);
	printf("------------------------\r\n");
	printf("\r\n");
}
