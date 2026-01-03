#ifndef DRUG_H
#define DRUG_H

#include "datastruct.h"

/**
 *功能：构建一个药品节点
 *参数：name - 药品名称
 *      spec - 药品规格
 *      factory - 药品生产厂家
 *      price - 药品价格
 *      stock - 药品库存
 *返回值：构造好的药品节点指针
 */
DrugNode make_drug(const char *name, const char *spec, const char *factory, double price, int stock);

/**
 * 功能：添加一个新药品
 * 参数：head - 链表头指针
 *       newInfo - 包含新药品信息的结构体
 */
DrugNode *add_drug(DrugNode **head, DrugNode newInfo);

/**
 * 功能：通过名称查找药品
 * 参数：head - 链表头指针
 *       name - 要查找的药品名称
 * 返回值：找到的药品节点指针，未找到返回 NULL
 */
DrugNode *findDrug_name(DrugNode *head, const char *name);

/**
 * 功能：修改药品信息
 * 参数：head - 链表头指针
 *       name - 药品名称
 *       newInfo - 包含更新后药品信息的结构体
 * 返回值：修改的药品节点指针，未找到返回 NULL
 */
DrugNode *modify_drug(DrugNode *head, const char *name, DrugNode newInfo);

/**
 * 功能：删除药品信息
 * 参数：head - 链表头指针
 *       name - 药品名称
 * 返回值：删除成功后的头指针，未找到返回 NULL
 */
DrugNode *delete_drug(DrugNode *head, const char *name);

/**
 * 功能：排序药品链表（按价格升序）
 * 参数：head - 链表头指针
 * 返回值：排序后的头指针
 */
DrugNode *sort_drugs_by_price(DrugNode *head);

/**
 * 功能：排序药品链表（按库存数量升序）
 * 参数：head - 链表头指针
 * 返回值：排序后的头指针
 */
DrugNode *sort_drugs_by_stock(DrugNode *head);


#endif //DRUG_H