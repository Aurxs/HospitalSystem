//
// Bill Management Implementation
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/datastruct.h"
#include "../include/bill.h"

// 工具: 构造一个费用记录（按值返回，便于直接传给 add_bill）
BillNode make_bill(const char *patientName, const char *itemName, double amount) {
    BillNode b;
    memset(&b, 0, sizeof(b));
    snprintf(b.patientName, sizeof(b.patientName), "%s", patientName);
    snprintf(b.itemName, sizeof(b.itemName), "%s", itemName);
    b.amount = amount;
    b.next = NULL;
    return b;
}

// 添加费用记录（尾插法）
BillNode *add_bill(BillNode **head, const BillNode newInfo) {
    BillNode *newNode = (BillNode *) malloc(sizeof(BillNode));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }

    // 将新数据填入临时结构体newNode
    strcpy(newNode->patientName, newInfo.patientName);
    strcpy(newNode->itemName, newInfo.itemName);
    newNode->amount = newInfo.amount;

    newNode->next = NULL;

    if (*head == NULL) {
        // 头指针为空
        *head = newNode;
    } else {
        // 找寻尾指针
        BillNode *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        // 将数据插入到尾指针的next指针
        temp->next = newNode;
    }
    return newNode;
}

// 通过患者姓名查询费用记录
BillNode *findBill_patient(BillNode *head, const char *patientName) {
    BillNode *current = head;
    while (current != NULL) {
        if (strcmp(current->patientName, patientName) == 0) {
            return current; // 找到匹配的费用记录，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

// 通过收费项目查询费用记录
BillNode *findBill_item(BillNode *head, const char *itemName) {
    BillNode *current = head;
    while (current != NULL) {
        if (strcmp(current->itemName, itemName) == 0) {
            return current; // 找到匹配的费用记录，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

// 计算患者的总费用
double calculate_total_bill(BillNode *head, const char *patientName) {
    double total = 0.0;
    BillNode *current = head;
    while (current != NULL) {
        if (strcmp(current->patientName, patientName) == 0) {
            total += current->amount;
        }
        current = current->next;
    }
    return total;
}

// 修改费用记录
BillNode *modify_bill(BillNode *head, const char *patientName, const char *itemName, BillNode newInfo) {
    BillNode *current = head;
    while (current != NULL) {
        if (strcmp(current->patientName, patientName) == 0 &&
            strcmp(current->itemName, itemName) == 0) {
            // 更新各个字段
            strcpy(current->patientName, newInfo.patientName);
            strcpy(current->itemName, newInfo.itemName);
            current->amount = newInfo.amount;
            return current; // 返回修改后的节点指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

// 删除费用记录（通过患者姓名和收费项目唯一标识）
BillNode *delete_bill(BillNode *head, const char *patientName, const char *itemName) {
    BillNode *current = head;
    BillNode *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->patientName, patientName) == 0 &&
            strcmp(current->itemName, itemName) == 0) {
            // 找到匹配的费用记录，进行删除
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

// 释放费用链表内存
void free_bill_list(BillNode *head) {
    BillNode *temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}
