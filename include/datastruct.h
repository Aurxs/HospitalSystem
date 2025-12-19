#ifndef _DATASTRUCT_H_
#define _DATASTRUCT_H_

#include <stdio.h>
#include <stdlib.h>

// 常量定义，统一管理字符串长度，方便修改
#define MAX_NAME 50
#define MAX_GENDER 10
#define MAX_PHONE 20
#define MAX_DESC 200     // 诊断详情、治疗方案等长文本
#define MAX_DEPT 50      // 科室名称长度


// 1. 患者信息节点 (Patient)
typedef struct PatientNode {
    char name[MAX_NAME];         // 姓名
    int age;                     // 年龄
    char gender[MAX_GENDER];     // 性别
    char phone[MAX_PHONE];       // 联系方式
    char diagnosis[MAX_DESC];    // 诊断结果
    char treatment[MAX_DESC];    // 治疗方案
    
    struct PatientNode* next;
} PatientNode;

// 2. 医生信息节点 (Doctor)
typedef struct DoctorNode {
    char name[MAX_NAME];
    int age;
    char gender[MAX_GENDER];
    char department[MAX_DEPT];   // 专业领域/科室
    char phone[MAX_PHONE];
    
    struct DoctorNode* next;
} DoctorNode;

// 3. 药品信息节点 (Drug)
typedef struct DrugNode {
    char name[MAX_NAME];
    char spec[MAX_DEPT];         // 规格 (如: 10mg/片)
    char factory[MAX_NAME];      // 生产厂家
    double price;                // 价格 (使用double存储小数)
    int stock;                   // 库存数量 (这是实际开发常加的，如果不加也可以)
    
    struct DrugNode* next;
} DrugNode;

// 4. 挂号记录节点 (Registration)
// 这个结构体用来连接患者和医生
typedef struct RegisterNode {
    char patientName[MAX_NAME];  // 挂号的患者
    char doctorName[MAX_NAME];   // 挂号的医生
    char department[MAX_DEPT];   // 挂号科室
    char date[MAX_NAME];         // 日期 (格式如 2023-10-01)
    
    struct RegisterNode* next;
} RegisterNode;

// 5. 费用记录节点 (Bill)
typedef struct BillNode {
    char patientName[MAX_NAME];  // 谁的费用
    char itemName[MAX_NAME];     // 收费项目 (如：阿莫西林、CT检查)
    double amount;               // 金额
    
    struct BillNode* next;
} BillNode;

#endif // _DATASTRUCT_H_ 结束