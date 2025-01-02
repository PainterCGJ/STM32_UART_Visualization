/**
 * @brief  链表初始化函数
 * @param  xList 链表指针
 * @param  DesEnable 降序链表使能，设置为true时链表项根据value值降序排列
 * @return void
 * @exception NSException 可能抛出的异常.
 * @author Painter
 * @date 2023-5
 * @version 1.0 ：版本
 * @property ：属性介绍
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
	void *pxOwener;				  //用于指向该链表项所属的链表
	uint32_t index;				  //链表项索引值，所有链表项的索引值在链表中连续升序排列
	int value;					  //链表项的值，链表项根据此值在链表中降序
	void *pxElement;			  //链表项的内容指针
	struct xListItem *pxPrevious; //指向前一个链表项
	struct xListItem *pxNext;	  //指向后一个链表项

} ListItem;

typedef struct
{
	ListItem HeadItem; //链表的头链表项
	ListItem EndItem;  //链表的尾链表项
	bool Descend;	   //降序使能
	uint32_t size;	   //链表大小，描述链表中链表项个数
} __List;

typedef __List *List;

/**
 * List_Init
 * @brief     链表初始化函数
 * @param     xList 链表指针
 * @param     DesEnable 降序链表使能，设置为true时链表项根据value值降序排列
 * @return    List 创建成功
 * 			  NULL 创建失败
 * @exception none
 * @author    Painter
 * @date      2023-5
 **/
List List_Init(bool DesEnable);

/**
 * ListInsert_Des
 * @brief     在pxList链表末尾插入新项
 * @param     xList 链表指针
 * @param     xvalue 新链表项值，新项根据该值，降序插进pxList链表中
 * @param     pxElement 新链表项内容指针
 * @return    0 插入成功，1 插入失败
 * @exception 链表没使能降序，但是调用了该函数。该函数只能用于降序链表。
 * @author    Painter
 * @date      2023-5
 **/
uint8_t ListInsert_Des(List xList, int xvalue, void *pxElement);

/**
 * ListPush_Back
 * @brief     在链表末尾添加新项，降序链表不能使用该函数
 * @param     xList 链表指针
 * @param     xvalue 新链表项值
 * @param     pxElement 新链表项内容指针
 * @return    0 插入成功，1 插入失败
 * @exception 降序链表调用了该函数。降序链表不能调用该函数
 * @author    Painter
 * @date      2023-5
 **/
uint8_t ListPush_Back(List xList, int xvalue, void *pxElement);

/**
 * ListPush_Front
 * @brief     在链表头添加新项，降序链表不能使用该函数
 * @param     xList 链表指针
 * @param     xvalue 新链表项值
 * @param     pxElement 新链表项内容指针
 * @return    0 插入成功，1 插入失败
 * @exception 降序链表调用了该函数。降序链表不能调用该函数
 * @author    Painter
 * @date      2023-5
 **/
uint8_t ListPush_Front(List xList, int xvalue, void *pxElement);

/**
 * ListInsert_FollowIndex
 * @brief     在指定链表中，在指定索引值的链表项后面的位置插入新项
 * @param     xList 链表指针
 * @param     follow 索引值。在索引值为follow的链表项项的后面插入新项
 * @param     xvalue 新链表项值
 * @param     pxElement 新链表项内容指针
 * @return    1：插入失败，原链表中没有找到索引值follow的项； 0：插入成功
 * @exception 降序链表调用了该函数。降序链表不能调用该函数
 * @author    Painter
 * @date      2023-5
 **/
uint8_t ListInsert_FollowIndex(List xList, uint8_t follow, int xvalue, void *pxElement);

/**
 * ListPush_FollowItem
 * @brief     在指定链表中，在指定链表项后面的位置插入新项
 * @param     xList 链表指针
 * @param     pxfollow 链表项指针。在地址为pxfollow的链表项后插入新项
 * @param     xvalue 新链表项值
 * @param     pxElement 新链表项内容指针
 * @return    1：插入失败，原链表中没有地址为pxfollow的链表项； 0：插入成功
 * @exception 降序链表调用了该函数。降序链表不能调用该函数
 * @author    Painter
 * @date      2023-5
 **/
uint8_t ListInsert_FollowItem(List xList, ListItem *pxfollow, int xvalue, void *pxElement);

/**
 * ListPop_Back
 * @brief     取出在链表尾部的链表项
 * @param     xList 链表指针
 * @return    ListItem* 所取出的链表项的地址
 * @exception 链表中只有头尾链表项时调用了该函数
 * @author    Painter
 * @date      2023-5
 **/
ListItem *ListPop_Back(List xList);

/**
 * ListPop_Front
 * @brief     取出在链表头部的链表项
 * @param     xList 链表指针
 * @return    ListItem* 所取出的链表项的地址
 * @exception 链表中只有头尾链表项时调用了该函数
 * @author    Painter
 * @date      2023-5
 **/
ListItem *ListPop_Front(List xList);

/**
 * ListRemove_Index
 * @brief     根据索引值移除链表项
 * @param     xList 链表指针
 * @return    ListItem* ：所取出的链表项的地址；NULL：在链表中没有索引值为xIndex的项
 * @exception 链表中只有头尾链表项时调用了该函数
 * @author    Painter
 * @date      2023-5
 **/
ListItem *ListRemove_Index(List xList, uint32_t xIndex);

/**
 * ListRemove_Item
 * @brief     根据索引值移除链表项
 * @param     xList 链表指针
 * @param     pxItem 链表项指针
 * @return    ListItem* ：所取出的链表项的地址；NULL：链表中不存在pxItem链表项
 * @exception none
 * @author    Painter
 * @date      2023-5
 **/
ListItem *ListRemove_Item(List xList, ListItem *pxItem);

/**
 * ListGetItemAdd_ForIndex
 * @brief     根据给出的索引值，在链表中获取相应链表项的地址
 * @param     xList 链表指针
 * @param     xindex 目标索引值。根据xindex获取链表项地址
 * @return    ListItem* ：获取的链表项地址； NULL：在链表中没有索引值为xIndex的项
 * @exception none
 * @author    Painter
 * @date      2023-5
 **/
ListItem *ListGetItemAdd_ForIndex(List xList, uint32_t xIndex);

/**
 * ListPrint
 * @brief     打印链表
 * @param     xList 链表指针
 * @return    void
 * @exception none
 * @author    Painter
 * @date      2023-5
 **/
void ListPrint(List const xList);

/**
 * ItemPrint
 * @brief     打印链表项
 * @param     pxListItem 链表项地址
 * @return    void
 * @exception none
 * @author    Painter
 * @date      2023-5
 **/
void ItemPrint(ListItem *pxListItem);
