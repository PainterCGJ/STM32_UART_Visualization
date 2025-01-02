/**
 * @brief  �����ʼ������
 * @param  xList ����ָ��
 * @param  DesEnable ��������ʹ�ܣ�����Ϊtrueʱ���������valueֵ��������
 * @return void
 * @exception NSException �����׳����쳣.
 * @author Painter
 * @date 2023-5
 * @version 1.0 ���汾
 * @property �����Խ���
 **/

#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define list_assert(x)                                                                   \
	{                                                                                    \
		(x) ? (void)0 : printf("list assert:<file>%s,<line>%u\r\n", __FILE__, __LINE__); \
	}

typedef struct xListItem
{
	void *pxOwener;				  //����ָ�������������������
	uint32_t index;				  //����������ֵ�����������������ֵ��������������������
	int value;					  //�������ֵ����������ݴ�ֵ�������н���
	void *pxElement;			  //�����������ָ��
	struct xListItem *pxPrevious; //ָ��ǰһ��������
	struct xListItem *pxNext;	  //ָ���һ��������

} ListItem;

typedef struct
{
	ListItem HeadItem; //�����ͷ������
	ListItem EndItem;  //�����β������
	bool Descend;	   //����ʹ��
	uint32_t size;	   //�����С���������������������
} __List;

typedef __List *List;

/**
 * List_Init
 * @brief     �����ʼ������
 * @param     xList ����ָ��
 * @param     DesEnable ��������ʹ�ܣ�����Ϊtrueʱ���������valueֵ��������
 * @return    List �����ɹ�
 * 			  NULL ����ʧ��
 * @exception none
 * @author    Painter
 * @date      2023-5
 **/
List List_Init(bool DesEnable);

/**
 * ListInsert_Des
 * @brief     ��pxList����ĩβ��������
 * @param     xList ����ָ��
 * @param     xvalue ��������ֵ��������ݸ�ֵ��������pxList������
 * @param     pxElement ������������ָ��
 * @return    0 ����ɹ���1 ����ʧ��
 * @exception ����ûʹ�ܽ��򣬵��ǵ����˸ú������ú���ֻ�����ڽ�������
 * @author    Painter
 * @date      2023-5
 **/
uint8_t ListInsert_Des(List xList, int xvalue, void *pxElement);

/**
 * ListPush_Back
 * @brief     ������ĩβ����������������ʹ�øú���
 * @param     xList ����ָ��
 * @param     xvalue ��������ֵ
 * @param     pxElement ������������ָ��
 * @return    0 ����ɹ���1 ����ʧ��
 * @exception ������������˸ú��������������ܵ��øú���
 * @author    Painter
 * @date      2023-5
 **/
uint8_t ListPush_Back(List xList, int xvalue, void *pxElement);

/**
 * ListPush_Front
 * @brief     ������ͷ����������������ʹ�øú���
 * @param     xList ����ָ��
 * @param     xvalue ��������ֵ
 * @param     pxElement ������������ָ��
 * @return    0 ����ɹ���1 ����ʧ��
 * @exception ������������˸ú��������������ܵ��øú���
 * @author    Painter
 * @date      2023-5
 **/
uint8_t ListPush_Front(List xList, int xvalue, void *pxElement);

/**
 * ListInsert_FollowIndex
 * @brief     ��ָ�������У���ָ������ֵ������������λ�ò�������
 * @param     xList ����ָ��
 * @param     follow ����ֵ��������ֵΪfollow����������ĺ����������
 * @param     xvalue ��������ֵ
 * @param     pxElement ������������ָ��
 * @return    1������ʧ�ܣ�ԭ������û���ҵ�����ֵfollow��� 0������ɹ�
 * @exception ������������˸ú��������������ܵ��øú���
 * @author    Painter
 * @date      2023-5
 **/
uint8_t ListInsert_FollowIndex(List xList, uint8_t follow, int xvalue, void *pxElement);

/**
 * ListPush_FollowItem
 * @brief     ��ָ�������У���ָ������������λ�ò�������
 * @param     xList ����ָ��
 * @param     pxfollow ������ָ�롣�ڵ�ַΪpxfollow����������������
 * @param     xvalue ��������ֵ
 * @param     pxElement ������������ָ��
 * @return    1������ʧ�ܣ�ԭ������û�е�ַΪpxfollow������� 0������ɹ�
 * @exception ������������˸ú��������������ܵ��øú���
 * @author    Painter
 * @date      2023-5
 **/
uint8_t ListInsert_FollowItem(List xList, ListItem *pxfollow, int xvalue, void *pxElement);

/**
 * ListPop_Back
 * @brief     ȡ��������β����������
 * @param     xList ����ָ��
 * @return    ListItem* ��ȡ����������ĵ�ַ
 * @exception ������ֻ��ͷβ������ʱ�����˸ú���
 * @author    Painter
 * @date      2023-5
 **/
ListItem *ListPop_Back(List xList);

/**
 * ListPop_Front
 * @brief     ȡ��������ͷ����������
 * @param     xList ����ָ��
 * @return    ListItem* ��ȡ����������ĵ�ַ
 * @exception ������ֻ��ͷβ������ʱ�����˸ú���
 * @author    Painter
 * @date      2023-5
 **/
ListItem *ListPop_Front(List xList);

/**
 * ListRemove_Index
 * @brief     ��������ֵ�Ƴ�������
 * @param     xList ����ָ��
 * @return    ListItem* ����ȡ����������ĵ�ַ��NULL����������û������ֵΪxIndex����
 * @exception ������ֻ��ͷβ������ʱ�����˸ú���
 * @author    Painter
 * @date      2023-5
 **/
ListItem *ListRemove_Index(List xList, uint32_t xIndex);

/**
 * ListRemove_Item
 * @brief     ��������ֵ�Ƴ�������
 * @param     xList ����ָ��
 * @param     pxItem ������ָ��
 * @return    ListItem* ����ȡ����������ĵ�ַ��NULL�������в�����pxItem������
 * @exception none
 * @author    Painter
 * @date      2023-5
 **/
ListItem *ListRemove_Item(List xList, ListItem *pxItem);

/**
 * ListGetItemAdd_ForIndex
 * @brief     ���ݸ���������ֵ���������л�ȡ��Ӧ������ĵ�ַ
 * @param     xList ����ָ��
 * @param     xindex Ŀ������ֵ������xindex��ȡ�������ַ
 * @return    ListItem* ����ȡ���������ַ�� NULL����������û������ֵΪxIndex����
 * @exception none
 * @author    Painter
 * @date      2023-5
 **/
ListItem *ListGetItemAdd_ForIndex(List xList, uint32_t xIndex);

/**
 * ListPrint
 * @brief     ��ӡ����
 * @param     xList ����ָ��
 * @return    void
 * @exception none
 * @author    Painter
 * @date      2023-5
 **/
void ListPrint(List const xList);

/**
 * ItemPrint
 * @brief     ��ӡ������
 * @param     pxListItem �������ַ
 * @return    void
 * @exception none
 * @author    Painter
 * @date      2023-5
 **/
void ItemPrint(ListItem *pxListItem);
