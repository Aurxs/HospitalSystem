//
// Registration Management Implementation
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/datastruct.h"
#include "../include/registration.h"

// 工具: 构造一个挂号记录（按值返回，便于直接传给 add_registration）
RegisterNode make_registration(const char *patientName, const char *doctorName,
                               const char *department, const char *date) {
    RegisterNode r;
    memset(&r, 0, sizeof(r));
    snprintf(r.patientName, sizeof(r.patientName), "%s", patientName);
    snprintf(r.doctorName, sizeof(r.doctorName), "%s", doctorName);
    snprintf(r.department, sizeof(r.department), "%s", department);
    snprintf(r.date, sizeof(r.date), "%s", date);
    r.next = NULL;
    return r;
}

// 添加挂号记录（尾插法）
RegisterNode *add_registration(RegisterNode **head, const RegisterNode newInfo) {
    RegisterNode *newNode = (RegisterNode *) malloc(sizeof(RegisterNode));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }

    // 将新数据填入临时结构体newNode
    strcpy(newNode->patientName, newInfo.patientName);
    strcpy(newNode->doctorName, newInfo.doctorName);
    strcpy(newNode->department, newInfo.department);
    strcpy(newNode->date, newInfo.date);

    newNode->next = NULL;

    if (*head == NULL) {
        // 头指针为空
        *head = newNode;
    } else {
        // 找寻尾指针
        RegisterNode *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        // 将数据插入到尾指针的next指针
        temp->next = newNode;
    }
    return newNode;
}

// 通过患者姓名查询挂号记录
RegisterNode *findRegistration_patient(RegisterNode *head, const char *patientName) {
    RegisterNode *current = head;
    while (current != NULL) {
        if (strcmp(current->patientName, patientName) == 0) {
            return current; // 找到匹配的挂号记录，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

// 通过医生姓名查询挂号记录
RegisterNode *findRegistration_doctor(RegisterNode *head, const char *doctorName) {
    RegisterNode *current = head;
    while (current != NULL) {
        if (strcmp(current->doctorName, doctorName) == 0) {
            return current; // 找到匹配的挂号记录，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

// 通过科室查询挂号记录
RegisterNode *findRegistration_department(RegisterNode *head, const char *department) {
    RegisterNode *current = head;
    while (current != NULL) {
        if (strcmp(current->department, department) == 0) {
            return current; // 找到匹配的挂号记录，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

// 通过日期查询挂号记录
RegisterNode *findRegistration_date(RegisterNode *head, const char *date) {
    RegisterNode *current = head;
    while (current != NULL) {
        if (strcmp(current->date, date) == 0) {
            return current; // 找到匹配的挂号记录，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

// 删除挂号记录（通过患者姓名和日期唯一标识）
RegisterNode *delete_registration(RegisterNode *head, const char *patientName, const char *date) {
    RegisterNode *current = head;
    RegisterNode *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->patientName, patientName) == 0 &&
            strcmp(current->date, date) == 0) {
            // 找到匹配的挂号记录，进行删除
            if (previous == NULL) {
                // 删除的是头节点
                head = current->next;
            } else {
                previous->next = current->next;
            }
            free(current); // 释放内存
            return head; // 返回新的头指针
        }
        previous = current;
        current = current->next; // 继续下一个节点
    }
    return head; // 没有找到，返回原头指针
}

// 释放挂号链表内存
void free_registration_list(RegisterNode *head) {
    RegisterNode *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}
