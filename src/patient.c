#include <stdio.h>
#include <stdlib.h> // 使用 malloc 需要
#include <string.h> // 使用 strcpy 需要
#include "../include/datastruct.h"

// 添加病人（尾插法）
void addPatient(PatientNode **head, const PatientNode newInfo) {
    PatientNode *newNode = (PatientNode *) malloc(sizeof(PatientNode));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return;
    }

    //将新数据填入临时结构体newNode
    strcpy(newNode->name, newInfo.name);
    newNode->age = newInfo.age;
    strcpy(newNode->gender, newInfo.gender);
    strcpy(newNode->phone, newInfo.phone);
    strcpy(newNode->diagnosis, newInfo.diagnosis);
    strcpy(newNode->treatment, newInfo.treatment);

    newNode->next = NULL;

    if (*head == NULL) {
        // 头指针为空
        *head = newNode;
    } else {
        //找寻尾指针
        PatientNode *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        //将数据插入到尾指针的next指针
        temp->next = newNode;
    }

    printf("成功添加病人：%s\n", newNode->name);
}

//通过名字查询患者信息
PatientNode *findPatient_name(PatientNode *head, const char *name) {
    PatientNode *current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current; // 找到匹配的患者，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

//通过联系方式查询患者信息
PatientNode *findPatient_phone(PatientNode *head, const char *phone) {
    PatientNode *current = head;
    while (current != NULL) {
        if (strcmp(current->phone, phone) == 0) {
            return current; // 找到匹配的患者，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

//修改患者信息
PatientNode *modifyPatient(PatientNode *head, const char *phone, const PatientNode newInfo) {
    PatientNode *current = head;
    while (current != NULL) {
        if (strcmp(current->phone, phone) == 0) {
            // 更新各个字段
            strcpy(current->name, newInfo.name);
            current->age = newInfo.age;
            strcpy(current->gender, newInfo.gender);
            strcpy(current->phone, newInfo.phone);
            strcpy(current->diagnosis, newInfo.diagnosis);
            strcpy(current->treatment, newInfo.treatment);
            return current;// 返回修改后的节点指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL;// 没有找到，返回 NULL
}

//删除患者信息
PatientNode *deletePatient(PatientNode *head, const char *phone) {
    PatientNode *current = head;
    PatientNode *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->phone, phone) == 0) {
            // 找到匹配的患者，进行删除
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
