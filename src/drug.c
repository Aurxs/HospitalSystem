//
// Created by 罗金源 on 2025/12/19.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/datastruct.h"
#include "../include/drug.h"

// 工具: 构造一个药品记录
DrugNode make_drug(const char *name, const char *spec, const char *factory, const double price, const int stock) {
    DrugNode p;
    memset(&p, 0, sizeof(p));
    snprintf(p.name, sizeof(p.name), "%s", name);
    snprintf(p.spec, sizeof(p.spec), "%s", spec);
    snprintf(p.factory, sizeof(p.factory), "%s", factory);
    p.price = price;
    p.stock = stock;
    p.next = NULL;
    return p;
}

// 添加药品（尾插法）
DrugNode *add_drug(DrugNode **head, const DrugNode newInfo) {
    DrugNode *newNode = (DrugNode *) malloc(sizeof(DrugNode));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }

    //将新数据填入临时结构体newNode
    strcpy(newNode->name, newInfo.name);
    strcpy(newNode->spec, newInfo.spec);
    strcpy(newNode->factory, newInfo.factory);
    newNode->price = newInfo.price;
    newNode->stock = newInfo.stock;

    newNode->next = NULL;

    if (*head == NULL) {
        // 头指针为空
        *head = newNode;
    } else {
        //找寻尾指针
        DrugNode *temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        //将数据插入到尾指针的next指针
        temp->next = newNode;
    }
    return newNode;
}

//通过名字查询药品信息
DrugNode *findDrug_name(DrugNode *head, const char *name) {
    DrugNode *current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current; // 找到匹配的药品，返回指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

//修改药品信息
DrugNode *modify_drug(DrugNode *head, const char *name, const DrugNode newInfo) {
    DrugNode *current = head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // 更新各个字段
            strcpy(current->name, newInfo.name);
            strcpy(current->spec, newInfo.spec);
            strcpy(current->factory, newInfo.factory);
            current->price = newInfo.price;
            current->stock = newInfo.stock;
            return current; // 返回修改后的节点指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

//删除药品信息
DrugNode *delete_drug(DrugNode *head, const char *name) {
    DrugNode *current = head;
    DrugNode *previous = NULL;

    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            // 找到匹配的药品，进行删除
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
