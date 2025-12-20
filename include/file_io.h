#ifndef FILE_IO_H
#define FILE_IO_H

#include "datastruct.h"

/**
 * 功能：保存患者链表到文件
 * 参数：filename - 文件名
 *       head - 患者链表头指针
 * 返回值：成功返回1，失败返回0
 */
int save_patients(const char *filename, PatientNode *head);

/**
 * 功能：从文件读取患者链表
 * 参数：filename - 文件名
 * 返回值：读取的患者链表头指针，失败返回NULL
 */
PatientNode *load_patients(const char *filename);

/**
 * 功能：保存医生链表到文件
 * 参数：filename - 文件名
 *       head - 医生链表头指针
 * 返回值：成功返回1，失败返回0
 */
int save_doctors(const char *filename, DoctorNode *head);

/**
 * 功能：从文件读取医生链表
 * 参数：filename - 文件名
 * 返回值：读取的医生链表头指针，失败返回NULL
 */
DoctorNode *load_doctors(const char *filename);

/**
 * 功能：保存药品链表到文件
 * 参数：filename - 文件名
 *       head - 药品链表头指针
 * 返回值：成功返回1，失败返回0
 */
int save_drugs(const char *filename, DrugNode *head);

/**
 * 功能：从文件读取药品链表
 * 参数：filename - 文件名
 * 返回值：读取的药品链表头指针，失败返回NULL
 */
DrugNode *load_drugs(const char *filename);

/**
 * 功能：保存挂号记录链表到文件
 * 参数：filename - 文件名
 *       head - 挂号记录链表头指针
 * 返回值：成功返回1，失败返回0
 */
int save_registrations(const char *filename, RegisterNode *head);

/**
 * 功能：从文件读取挂号记录链表
 * 参数：filename - 文件名
 * 返回值：读取的挂号记录链表头指针，失败返回NULL
 */
RegisterNode *load_registrations(const char *filename);

/**
 * 功能：保存费用记录链表到文件
 * 参数：filename - 文件名
 *       head - 费用记录链表头指针
 * 返回值：成功返回1，失败返回0
 */
int save_bills(const char *filename, BillNode *head);

/**
 * 功能：从文件读取费用记录链表
 * 参数：filename - 文件名
 * 返回值：读取的费用记录链表头指针，失败返回NULL
 */
BillNode *load_bills(const char *filename);

#endif // FILE_IO_H
