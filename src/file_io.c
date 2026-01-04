//
// File I/O Implementation - Save and Load Functions
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/datastruct.h"
#include "../include/file_io.h"

#include "../include/auth.h"
#include "../include/patient.h"
#include "../include/doctor.h"
#include "../include/drug.h"
#include "../include/registration.h"
#include "../include/bill.h"

// ================== 患者数据保存与读取 ==================

// 保存患者链表到文件
int save_patients(const char *filename, PatientNode *head) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法打开文件 %s 进行写入！\n", filename);
        return 0;
    }

    PatientNode *current = head;
    while (current != NULL) {
        fprintf(fp, "%s|%d|%s|%s|%s|%s\n",
                current->name,
                current->age,
                current->gender,
                current->phone,
                current->diagnosis,
                current->treatment);
        current = current->next;
    }

    fclose(fp);
    return 1;
}

// 从文件读取患者链表
PatientNode *load_patients(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }

    PatientNode *head = NULL;
    char line[1024];

    while (fgets(line, sizeof(line), fp) != NULL) {
        // 移除换行符
        line[strcspn(line, "\n")] = 0;

        char name[MAX_NAME], gender[MAX_GENDER], phone[MAX_PHONE];
        char diagnosis[MAX_DESC], treatment[MAX_DESC];
        int age;

        // 解析数据
        char *token = strtok(line, "|");
        if (token == NULL) continue;
        strncpy(name, token, MAX_NAME - 1);
        name[MAX_NAME - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        age = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(gender, token, MAX_GENDER - 1);
        gender[MAX_GENDER - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(phone, token, MAX_PHONE - 1);
        phone[MAX_PHONE - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(diagnosis, token, MAX_DESC - 1);
        diagnosis[MAX_DESC - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(treatment, token, MAX_DESC - 1);
        treatment[MAX_DESC - 1] = '\0';

        PatientNode p = make_patient(name, age, gender, phone, diagnosis, treatment);
        add_patient(&head, p);
    }

    fclose(fp);
    return head;
}

// ================== 医生数据保存与读取 ==================

// 保存医生链表到文件
int save_doctors(const char *filename, DoctorNode *head) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法打开文件 %s 进行写入！\n", filename);
        return 0;
    }

    DoctorNode *current = head;
    while (current != NULL) {
        fprintf(fp, "%s|%d|%s|%s|%s|%s\n",
                current->name,
                current->age,
                current->gender,
                current->department,
                current->phone,
                current->schedule);
        current = current->next;
    }

    fclose(fp);
    return 1;
}

// 从文件读取医生链表
DoctorNode *load_doctors(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }

    DoctorNode *head = NULL;
    char line[512];

    while (fgets(line, sizeof(line), fp) != NULL) {
        // 移除换行符
        line[strcspn(line, "\n")] = 0;

        char name[MAX_NAME], gender[MAX_GENDER], department[MAX_DEPT], phone[MAX_PHONE], schedule[MAX_DEPT];
        int age;
        
        // 初始化schedule为空（向后兼容）
        // 旧版本数据格式: name|age|gender|department|phone
        // 新版本数据格式: name|age|gender|department|phone|schedule
        // 如果文件中没有schedule字段，则使用空字符串
        memset(schedule, 0, sizeof(schedule));

        // 解析数据
        char *token = strtok(line, "|");
        if (token == NULL) continue;
        strncpy(name, token, MAX_NAME - 1);
        name[MAX_NAME - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        age = atoi(token);

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(gender, token, MAX_GENDER - 1);
        gender[MAX_GENDER - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(department, token, MAX_DEPT - 1);
        department[MAX_DEPT - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(phone, token, MAX_PHONE - 1);
        phone[MAX_PHONE - 1] = '\0';

        // 读取排班字段（可选，兼容旧数据）
        token = strtok(NULL, "|");
        if (token != NULL) {
            strncpy(schedule, token, MAX_DEPT - 1);
            schedule[MAX_DEPT - 1] = '\0';
        }

        DoctorNode d = make_doctor(name, age, gender, department, phone, schedule);
        add_doctor(&head, d);
    }

    fclose(fp);
    return head;
}

// ================== 药品数据保存与读取 ==================

// 保存药品链表到文件
int save_drugs(const char *filename, DrugNode *head) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法打开文件 %s 进行写入！\n", filename);
        return 0;
    }

    DrugNode *current = head;
    while (current != NULL) {
        fprintf(fp, "%s|%s|%s|%.2f|%d\n",
                current->name,
                current->spec,
                current->factory,
                current->price,
                current->stock);
        current = current->next;
    }

    fclose(fp);
    return 1;
}

// 从文件读取药品链表
DrugNode *load_drugs(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }

    DrugNode *head = NULL;
    char line[512];

    while (fgets(line, sizeof(line), fp) != NULL) {
        // 移除换行符
        line[strcspn(line, "\n")] = 0;

        char name[MAX_NAME], spec[MAX_DEPT], factory[MAX_NAME];
        double price;
        int stock;

        // 解析数据
        char *token = strtok(line, "|");
        if (token == NULL) continue;
        strncpy(name, token, MAX_NAME - 1);
        name[MAX_NAME - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(spec, token, MAX_DEPT - 1);
        spec[MAX_DEPT - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(factory, token, MAX_NAME - 1);
        factory[MAX_NAME - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        price = atof(token);

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        stock = atoi(token);

        DrugNode d = make_drug(name, spec, factory, price, stock);
        add_drug(&head, d);
    }

    fclose(fp);
    return head;
}

// ================== 挂号记录保存与读取 ==================

// 保存挂号记录链表到文件
int save_registrations(const char *filename, RegisterNode *head) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法打开文件 %s 进行写入！\n", filename);
        return 0;
    }

    RegisterNode *current = head;
    while (current != NULL) {
        fprintf(fp, "%s|%s|%s|%s\n",
                current->patientName,
                current->doctorName,
                current->department,
                current->date);
        current = current->next;
    }

    fclose(fp);
    return 1;
}

// 从文件读取挂号记录链表
RegisterNode *load_registrations(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }

    RegisterNode *head = NULL;
    char line[512];

    while (fgets(line, sizeof(line), fp) != NULL) {
        // 移除换行符
        line[strcspn(line, "\n")] = 0;

        char patientName[MAX_NAME], doctorName[MAX_NAME], department[MAX_DEPT], date[MAX_NAME];

        // 解析数据
        char *token = strtok(line, "|");
        if (token == NULL) continue;
        strncpy(patientName, token, MAX_NAME - 1);
        patientName[MAX_NAME - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(doctorName, token, MAX_NAME - 1);
        doctorName[MAX_NAME - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(department, token, MAX_DEPT - 1);
        department[MAX_DEPT - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(date, token, MAX_NAME - 1);
        date[MAX_NAME - 1] = '\0';

        RegisterNode r = make_registration(patientName, doctorName, department, date);
        add_registration(&head, r);
    }

    fclose(fp);
    return head;
}

// ================== 费用记录保存与读取 ==================

// 保存费用记录链表到文件
int save_bills(const char *filename, BillNode *head) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法打开文件 %s 进行写入！\n", filename);
        return 0;
    }

    BillNode *current = head;
    while (current != NULL) {
        fprintf(fp, "%s|%s|%.2f\n",
                current->patientName,
                current->itemName,
                current->amount);
        current = current->next;
    }

    fclose(fp);
    return 1;
}

// 从文件读取费用记录链表
BillNode *load_bills(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }

    BillNode *head = NULL;
    char line[256];

    while (fgets(line, sizeof(line), fp) != NULL) {
        // 移除换行符
        line[strcspn(line, "\n")] = 0;

        char patientName[MAX_NAME], itemName[MAX_NAME];
        double amount;

        // 解析数据
        char *token = strtok(line, "|");
        if (token == NULL) continue;
        strncpy(patientName, token, MAX_NAME - 1);
        patientName[MAX_NAME - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(itemName, token, MAX_NAME - 1);
        itemName[MAX_NAME - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        amount = atof(token);

        BillNode b = make_bill(patientName, itemName, amount);
        add_bill(&head, b);
    }

    fclose(fp);
    return head;
}

//================== 用户登录信息保存与读取 ==================

// 保存用户链表到文件
int save_users(const char *filename, AuthNode *head) {
    FILE *fp = fopen(filename, "w");
    if (fp == NULL) {
        printf("无法打开文件 %s 进行写入！\n", filename);
        return 0;
    }
    AuthNode *current = head;
    while (current != NULL) {
        fprintf(fp, "%s|%s|%d\n",
                current->username,
                current->password,
                current->role);
        current = current->next;
    }
    fclose(fp);
    return 1;
}

// 从文件读取用户链表
AuthNode *load_users(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        return NULL;
    }
    AuthNode *head = NULL;
    char line[256];
    while (fgets(line, sizeof(line), fp) != NULL) {
        // 移除换行符
        line[strcspn(line, "\n")] = 0;
        char username[MAX_NAME], password[MAX_NAME];
        int role = 2;  // 默认为患者权限（最低权限）
        
        // 解析数据
        char *token = strtok(line, "|");
        if (token == NULL) continue;
        strncpy(username, token, MAX_NAME - 1);
        username[MAX_NAME - 1] = '\0';
        
        token = strtok(NULL, "|");
        if (token == NULL) continue;
        strncpy(password, token, MAX_NAME - 1);
        password[MAX_NAME - 1] = '\0';
        
        // 读取角色字段（重要！）
        token = strtok(NULL, "|");
        if (token != NULL) {
            role = atoi(token);
        }

        // 直接构造节点，避免 make_user 再次加密
        AuthNode a;
        memset(&a, 0, sizeof(AuthNode));
        strncpy(a.username, username, MAX_NAME - 1);
        strncpy(a.password, password, MAX_NAME - 1);
        a.role = role;  // 设置角色
        a.next = NULL;

        add_user(&head, a);
    }
    fclose(fp);
    return head;
}