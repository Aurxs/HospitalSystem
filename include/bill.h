#ifndef BILL_H
#define BILL_H

#include "datastruct.h"

/**
 * 功能：构造一个费用记录
 * 参数：patientName - 患者姓名
 *       itemName - 收费项目
 *       amount - 金额
 * 返回值：构造好的费用节点（按值返回）
 */
BillNode make_bill(const char *patientName, const char *itemName, double amount);

/**
 * 功能：添加一个新费用记录
 * 参数：head - 链表头指针
 *       newInfo - 包含新费用信息的结构体
 * 返回值：添加的费用节点指针
 */
BillNode *add_bill(BillNode **head, BillNode newInfo);

/**
 * 功能：通过患者姓名查找费用记录
 * 参数：head - 链表头指针
 *       patientName - 要查找的患者姓名
 * 返回值：找到的费用节点指针，未找到返回 NULL
 */
BillNode *findBill_patient(BillNode *head, const char *patientName);

/**
 * 功能：通过收费项目查找费用记录
 * 参数：head - 链表头指针
 *       itemName - 要查找的收费项目
 * 返回值：找到的费用节点指针，未找到返回 NULL
 */
BillNode *findBill_item(BillNode *head, const char *itemName);

/**
 * 功能：计算患者的总费用
 * 参数：head - 链表头指针
 *       patientName - 患者姓名
 * 返回值：该患者的总费用
 */
double calculate_total_bill(BillNode *head, const char *patientName);

/**
 * 功能：修改费用记录
 * 参数：head - 链表头指针
 *       patientName - 患者姓名
 *       itemName - 收费项目
 *       newInfo - 包含更新后费用信息的结构体
 * 返回值：修改的费用节点指针，未找到返回 NULL
 */
BillNode *modify_bill(BillNode *head, const char *patientName, const char *itemName, BillNode newInfo);

/**
 * 功能：删除费用记录
 * 参数：head - 链表头指针
 *       patientName - 患者姓名
 *       itemName - 收费项目（唯一标识一条费用记录）
 * 返回值：删除成功后的头指针
 */
BillNode *delete_bill(BillNode *head, const char *patientName, const char *itemName);

/**
 * 功能：释放费用链表内存
 * 参数：head - 链表头指针
 */
void free_bill_list(BillNode *head);

#endif // BILL_H
