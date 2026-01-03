#ifndef DOCTOR_H
#define DOCTOR_H

#include "datastruct.h"

/**
 *功能：构建一个医生节点
 *参数：name - 医生姓名
 *      age - 医生年龄
 *      gender - 医生性别
 *      department - 医生科室
 *      phone - 医生电话号码
 *返回值：构造好的医生节点指针
 */
DoctorNode make_doctor(const char *name, int age, const char *gender, const char *department, const char *phone);

/**
 * 功能：添加一个新医生
 * 参数：head - 链表头指针
 *       newInfo - 包含新医生信息的结构体
 */
DoctorNode *add_doctor(DoctorNode **head, DoctorNode newInfo);

/**
 * 功能：通过姓名查找医生
 * 参数：head - 链表头指针
 *       name - 要查找的医生姓名
 * 返回值：找到的医生节点指针，未找到返回 NULL
 */
DoctorNode *findDoctor_name(DoctorNode *head, const char *name);

/**
 * 功能：通过电话号码查找医生
 * 参数：head - 链表头指针
 *       phone - 要查找的医生电话号码
 * 返回值：找到的医生节点指针，未找到返回 NULL
 */

DoctorNode *findDoctor_phone(DoctorNode *head, const char *phone);

/**
 * 功能：修改医生信息
 * 参数：head - 链表头指针
 *       phone - 医生电话号码（唯一标识）
 *       newInfo - 包含更新后医生信息的结构体
 * 返回值：修改的医生节点指针，未找到返回 NULL
 */
DoctorNode *modify_doctor(DoctorNode *head, const char *phone, DoctorNode newInfo);

/**
 * 功能：删除医生信息
 * 参数：head - 链表头指针
 *       phone - 医生电话号码（唯一标识）
 * 返回值：删除成功后的头指针，未找到返回 NULL
 */
DoctorNode *delete_doctor(DoctorNode *head, const char *phone);

/**
 * 功能：排序医生链表（按姓名字典序）
 * 参数：head - 链表头指针
 * 返回值：排序后的头指针
 */
DoctorNode *sort_doctors_by_name(DoctorNode *head);

/**
 * 功能：排序医生链表（按电话号码字典序）
 * 参数：head - 链表头指针
 * 返回值：排序后的头指针
 */
DoctorNode *sort_doctors_by_phone(DoctorNode *head);

/**
 * 功能：排序医生链表（按年龄升序）
 * 参数：head - 链表头指针
 * 返回值：排序后的头指针
 */
DoctorNode *sort_doctors_by_age(DoctorNode *head);


#endif //DOCTOR_H