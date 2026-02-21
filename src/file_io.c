//
// File I/O Implementation - Save and Load Functions
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include "../include/datastruct.h"
#include "../include/file_io.h"

#include "../include/auth.h"
#include "../include/patient.h"
#include "../include/doctor.h"
#include "../include/drug.h"
#include "../include/registration.h"
#include "../include/bill.h"

#if !defined(_WIN32) && !defined(WIN32)
#include <sys/stat.h>
#endif

static void sanitize_field(const char *src, char *dst, size_t dst_size) {
    size_t i = 0;

    if (dst == NULL || dst_size == 0) {
        return;
    }
    if (src == NULL) {
        dst[0] = '\0';
        return;
    }

    while (src[i] != '\0' && i < dst_size - 1) {
        char c = src[i];
        if (c == '|' || c == '\r' || c == '\n') {
            c = ' ';
        }
        dst[i] = c;
        i++;
    }
    dst[i] = '\0';
}

static int parse_int_strict(const char *text, int *out_value) {
    char *endptr = NULL;
    long value;

    if (text == NULL || out_value == NULL || text[0] == '\0') {
        return 0;
    }

    errno = 0;
    value = strtol(text, &endptr, 10);
    if (errno != 0 || *endptr != '\0' || value < INT_MIN || value > INT_MAX) {
        return 0;
    }

    *out_value = (int) value;
    return 1;
}

static int parse_double_strict(const char *text, double *out_value) {
    char *endptr = NULL;
    double value;

    if (text == NULL || out_value == NULL || text[0] == '\0') {
        return 0;
    }

    errno = 0;
    value = strtod(text, &endptr);
    if (errno != 0 || *endptr != '\0' || !isfinite(value)) {
        return 0;
    }

    *out_value = value;
    return 1;
}

static int is_valid_role(int role) {
    return role >= 0 && role <= 2;
}

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
        char name[MAX_NAME], gender[MAX_GENDER], phone[MAX_PHONE];
        char diagnosis[MAX_DESC], treatment[MAX_DESC];
        sanitize_field(current->name, name, sizeof(name));
        sanitize_field(current->gender, gender, sizeof(gender));
        sanitize_field(current->phone, phone, sizeof(phone));
        sanitize_field(current->diagnosis, diagnosis, sizeof(diagnosis));
        sanitize_field(current->treatment, treatment, sizeof(treatment));

        fprintf(fp, "%s|%d|%s|%s|%s|%s\n",
                name,
                current->age,
                gender,
                phone,
                diagnosis,
                treatment);
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
        if (!parse_int_strict(token, &age) || age < 0 || age > 150) continue;

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
        char name[MAX_NAME], gender[MAX_GENDER], department[MAX_DEPT], phone[MAX_PHONE], schedule[MAX_DESC];
        sanitize_field(current->name, name, sizeof(name));
        sanitize_field(current->gender, gender, sizeof(gender));
        sanitize_field(current->department, department, sizeof(department));
        sanitize_field(current->phone, phone, sizeof(phone));
        sanitize_field(current->schedule, schedule, sizeof(schedule));

        fprintf(fp, "%s|%d|%s|%s|%s|%s\n",
                name,
                current->age,
                gender,
                department,
                phone,
                schedule);
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
    char line[1024];

    while (fgets(line, sizeof(line), fp) != NULL) {
        // 移除换行符
        line[strcspn(line, "\n")] = 0;

        char name[MAX_NAME], gender[MAX_GENDER], department[MAX_DEPT], phone[MAX_PHONE], schedule[MAX_DESC];
        int age;

        // 初始化schedule为空字符串（兼容旧数据）
        schedule[0] = '\0';

        // 解析数据
        char *token = strtok(line, "|");
        if (token == NULL) continue;
        strncpy(name, token, MAX_NAME - 1);
        name[MAX_NAME - 1] = '\0';

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        if (!parse_int_strict(token, &age) || age < 0 || age > 150) continue;

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

        // 读取排班记录（可选字段，兼容旧数据格式）
        token = strtok(NULL, "|");
        if (token != NULL) {
            strncpy(schedule, token, MAX_DESC - 1);
            schedule[MAX_DESC - 1] = '\0';
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
        char name[MAX_NAME], spec[MAX_DEPT], factory[MAX_NAME];
        sanitize_field(current->name, name, sizeof(name));
        sanitize_field(current->spec, spec, sizeof(spec));
        sanitize_field(current->factory, factory, sizeof(factory));

        fprintf(fp, "%s|%s|%s|%.2f|%d\n",
                name,
                spec,
                factory,
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
        if (!parse_double_strict(token, &price) || price < 0.0) continue;

        token = strtok(NULL, "|");
        if (token == NULL) continue;
        if (!parse_int_strict(token, &stock) || stock < 0) continue;

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
        char patientName[MAX_NAME], doctorName[MAX_NAME], department[MAX_DEPT], date[MAX_NAME];
        sanitize_field(current->patientName, patientName, sizeof(patientName));
        sanitize_field(current->doctorName, doctorName, sizeof(doctorName));
        sanitize_field(current->department, department, sizeof(department));
        sanitize_field(current->date, date, sizeof(date));

        fprintf(fp, "%s|%s|%s|%s\n",
                patientName,
                doctorName,
                department,
                date);
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
        char patientName[MAX_NAME], itemName[MAX_NAME];
        sanitize_field(current->patientName, patientName, sizeof(patientName));
        sanitize_field(current->itemName, itemName, sizeof(itemName));

        fprintf(fp, "%s|%s|%.2f\n",
                patientName,
                itemName,
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
        if (!parse_double_strict(token, &amount) || amount < 0.0) continue;

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
        char username[MAX_NAME], password[MAX_NAME];
        sanitize_field(current->username, username, sizeof(username));
        sanitize_field(current->password, password, sizeof(password));

        fprintf(fp, "%s|%s|%d\n",
                username,
                password,
                current->role);
        current = current->next;
    }

    if (fclose(fp) != 0) {
        return 0;
    }

#if !defined(_WIN32) && !defined(WIN32)
    (void) chmod(filename, S_IRUSR | S_IWUSR);
#endif

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
            int parsed_role = 2;
            if (parse_int_strict(token, &parsed_role) && is_valid_role(parsed_role)) {
                role = parsed_role;
            } else {
                role = 2;
            }
        }

        // 直接构造节点，避免 make_user 再次加密
        AuthNode a;
        char clean_username[MAX_NAME];
        char clean_password[MAX_NAME];
        memset(&a, 0, sizeof(AuthNode));
        sanitize_field(username, clean_username, sizeof(clean_username));
        sanitize_field(password, clean_password, sizeof(clean_password));
        strncpy(a.username, clean_username, MAX_NAME - 1);
        a.username[MAX_NAME - 1] = '\0';
        strncpy(a.password, clean_password, MAX_NAME - 1);
        a.password[MAX_NAME - 1] = '\0';
        a.role = is_valid_role(role) ? role : 2; // 设置角色
        a.next = NULL;

        add_user(&head, a);
    }
    fclose(fp);
    return head;
}
