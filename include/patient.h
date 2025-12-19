#ifndef PATIENT_H_
#define PATIENT_H_

#include "datastruct.h"

/**
 * 功能：构造一个患者记录
 * 参数：name - 患者姓名
 *       age - 患者年龄
 *       gender - 患者性别
 *       phone - 患者电话号码
 *       diagnosis - 诊断结果
 *       treatment - 治疗方案
 * 返回值：构造好的患者节点（按值返回）
*/
PatientNode make_patient(const char *name, int age, const char *gender,
                         const char *phone, const char *diagnosis, const char *treatment);

/**
 * 功能：添加一个新患者
 * 参数：head - 链表头指针
         newInfo - 包含新患者信息的结构体
 */
PatientNode *add_patient(PatientNode **head, PatientNode newInfo);

/**
 * 功能：通过姓名查找患者
 * 参数：head - 链表头指针
 *       name - 要查找的患者姓名
 * 返回值：找到的患者节点指针，未找到返回 NULL
 */
PatientNode *findPatient_name(PatientNode *head, const char *name);

/**
 * 功能：通过电话号码查找患者
 * 参数：head - 链表头指针
 *       phone - 要查找的患者电话号码
 * 返回值：找到的患者节点指针，未找到返回 NULL
 */
PatientNode *findPatient_phone(PatientNode *head, const char *phone);

/**
 * 功能：修改患者信息
 * 参数：head - 链表头指针
 *       phone - 患者电话号码（唯一标识）
 *       newInfo - 包含更新后患者信息的结构体
 * 返回值：修改的患者节点指针，未找到返回 NULL
 */
PatientNode *modify_patient(PatientNode *head, const char *phone, PatientNode newInfo);

/**
 * 功能：删除患者信息
 * 参数：head - 链表头指针
 *       phone - 患者电话号码（唯一标识）
 * 返回值：删除成功后的头指针，未找到返回 NULL
 */
PatientNode *delete_patient(PatientNode *head, const char *phone);


#endif
