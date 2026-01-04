//
// Created by 罗金源 on 2025/12/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/datastruct.h"
#include "../include/doctor.h"

// 工具: 构造一个医生记录
DoctorNode make_doctor(const char *name, int age, const char *gender, const char *department, const char *phone, const char *schedule) {
    DoctorNode p;
    memset(&p, 0, sizeof(p));
    snprintf(p.name, sizeof(p.name), "%s", name);
    p.age = age;
    snprintf(p.gender, sizeof(p.gender), "%s", gender);
    snprintf(p.phone, sizeof(p.phone), "%s", phone);
    snprintf(p.department, sizeof(p.department), "%s", department);
    snprintf(p.schedule, sizeof(p.schedule), "%s", schedule);
    p.next = NULL;
    return p;
}

// 添加医生（尾插法）
DoctorNode *add_doctor(DoctorNode **head, const DoctorNode newInfo) {
    DoctorNode *newNode = (DoctorNode *) malloc(sizeof(DoctorNode));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }

    //将新数据填入临时结构体newNode
    strcpy(newNode->name, newInfo.name);
    newNode->age = newInfo.age;
    strcpy(newNode->gender, newInfo.gender);
    strcpy(newNode->phone, newInfo.phone);
    strcpy(newNode->department, newInfo.department);
    strcpy(newNode->schedule, newInfo.schedule);

    newNode->next = NULL;

    if (*head == NULL) {
        // 头指针为空
        *head = newNode;
    } else {
        //找寻尾指针
        DoctorNode *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        //将数据插入到尾指针的next指针
        temp->next = newNode;
    }
    return newNode;
}

//通过名字查询医生信息
DoctorNode *findDoctor_name(DoctorNode *head, const char *name) {
    DoctorNode *current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current; // 找到匹配的医生，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

//通过联系方式查询医生信息
DoctorNode *findDoctor_phone(DoctorNode *head, const char *phone) {
    DoctorNode *current = head;
    while (current != NULL) {
        if (strcmp(current->phone, phone) == 0) {
            return current; // 找到匹配的医生，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

//修改医生信息
DoctorNode *modify_doctor(DoctorNode *head, const char *phone, const DoctorNode newInfo) {
    DoctorNode *current = head;
    while (current != NULL) {
        if (strcmp(current->phone, phone) == 0) {
            // 更新各个字段
            strcpy(current->name, newInfo.name);
            current->age = newInfo.age;
            strcpy(current->gender, newInfo.gender);
            strcpy(current->phone, newInfo.phone);
            strcpy(current->department, newInfo.department);
            strcpy(current->schedule, newInfo.schedule);
            return current; // 返回修改后的节点指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

//删除医生信息
DoctorNode *delete_doctor(DoctorNode *head, const char *phone) {
    DoctorNode *current = head;
    DoctorNode *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->phone, phone) == 0) {
            // 找到匹配的医生，进行删除
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

//按姓名字典序排序医生链表（使用归并排序）
DoctorNode *sort_doctors_by_name(DoctorNode *head) {
    // 基本情况：空链表或单节点链表
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // 使用快慢指针找到中点
    DoctorNode *slow = head;
    DoctorNode *fast = head->next;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 分割链表为两半
    DoctorNode *mid = slow->next;
    slow->next = NULL;

    // 递归排序两半
    DoctorNode *left = sort_doctors_by_name(head);
    DoctorNode *right = sort_doctors_by_name(mid);

    // 合并排序后的两半
    DoctorNode dummy;
    DoctorNode *tail = &dummy;
    dummy.next = NULL;

    while (left != NULL && right != NULL) {
        if (strcmp(left->name, right->name) <= 0) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }

    // 连接剩余节点
    if (left != NULL) {
        tail->next = left;
    } else {
        tail->next = right;
    }

    return dummy.next; // 返回排序后的头指针
}

//按电话号码字典序排序医生链表（使用归并排序）
DoctorNode *sort_doctors_by_phone(DoctorNode *head) {
    // 基本情况：空链表或单节点链表
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // 使用快慢指针找到中点
    DoctorNode *slow = head;
    DoctorNode *fast = head->next;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 分割链表为两半
    DoctorNode *mid = slow->next;
    slow->next = NULL;

    // 递归排序两半
    DoctorNode *left = sort_doctors_by_phone(head);
    DoctorNode *right = sort_doctors_by_phone(mid);

    // 合并排序后的两半
    DoctorNode dummy;
    DoctorNode *tail = &dummy;
    dummy.next = NULL;

    while (left != NULL && right != NULL) {
        if (strcmp(left->phone, right->phone) <= 0) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }

    // 连接剩余节点
    if (left != NULL) {
        tail->next = left;
    } else {
        tail->next = right;
    }

    return dummy.next; // 返回排序后的头指针
}

//按年龄升序排序医生链表（使用归并排序）
DoctorNode *sort_doctors_by_age(DoctorNode *head) {
    // 基本情况：空链表或单节点链表
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // 使用快慢指针找到中点
    DoctorNode *slow = head;
    DoctorNode *fast = head->next;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 分割链表为两半
    DoctorNode *mid = slow->next;
    slow->next = NULL;

    // 递归排序两半
    DoctorNode *left = sort_doctors_by_age(head);
    DoctorNode *right = sort_doctors_by_age(mid);

    // 合并排序后的两半
    DoctorNode dummy;
    DoctorNode *tail = &dummy;
    dummy.next = NULL;

    while (left != NULL && right != NULL) {
        if (left->age <= right->age) {
            tail->next = left;
            left = left->next;
        } else {
            tail->next = right;
            right = right->next;
        }
        tail = tail->next;
    }

    // 连接剩余节点
    if (left != NULL) {
        tail->next = left;
    } else {
        tail->next = right;
    }

    return dummy.next; // 返回排序后的头指针
}
