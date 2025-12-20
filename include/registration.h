#ifndef REGISTRATION_H
#define REGISTRATION_H

#include "datastruct.h"

/**
 * 功能：构造一个挂号记录
 * 参数：patientName - 患者姓名
 *       doctorName - 医生姓名
 *       department - 科室名称
 *       date - 日期 (格式如 2023-10-01)
 * 返回值：构造好的挂号节点（按值返回）
 */
RegisterNode make_registration(const char *patientName, const char *doctorName,
                               const char *department, const char *date);

/**
 * 功能：添加一个新挂号记录
 * 参数：head - 链表头指针
 *       newInfo - 包含新挂号信息的结构体
 * 返回值：添加的挂号节点指针
 */
RegisterNode *add_registration(RegisterNode **head, RegisterNode newInfo);

/**
 * 功能：通过患者姓名查找挂号记录
 * 参数：head - 链表头指针
 *       patientName - 要查找的患者姓名
 * 返回值：找到的挂号节点指针，未找到返回 NULL
 */
RegisterNode *findRegistration_patient(RegisterNode *head, const char *patientName);

/**
 * 功能：通过医生姓名查找挂号记录
 * 参数：head - 链表头指针
 *       doctorName - 要查找的医生姓名
 * 返回值：找到的挂号节点指针，未找到返回 NULL
 */
RegisterNode *findRegistration_doctor(RegisterNode *head, const char *doctorName);

/**
 * 功能：通过科室查找挂号记录
 * 参数：head - 链表头指针
 *       department - 要查找的科室名称
 * 返回值：找到的挂号节点指针，未找到返回 NULL
 */
RegisterNode *findRegistration_department(RegisterNode *head, const char *department);

/**
 * 功能：通过日期查找挂号记录
 * 参数：head - 链表头指针
 *       date - 要查找的日期
 * 返回值：找到的挂号节点指针，未找到返回 NULL
 */
RegisterNode *findRegistration_date(RegisterNode *head, const char *date);

/**
 * 功能：删除挂号记录
 * 参数：head - 链表头指针
 *       patientName - 患者姓名
 *       date - 挂号日期（唯一标识一条挂号记录）
 * 返回值：删除成功后的头指针
 */
RegisterNode *delete_registration(RegisterNode *head, const char *patientName, const char *date);

/**
 * 功能：释放挂号链表内存
 * 参数：head - 链表头指针
 */
void free_registration_list(RegisterNode *head);

#endif // REGISTRATION_H
