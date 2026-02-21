#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/datastruct.h"
#include "../include/patient.h"

static void copy_patient_fields(PatientNode *dst, const PatientNode *src) {
    if (dst == NULL || src == NULL) {
        return;
    }
    strncpy(dst->name, src->name, MAX_NAME - 1);
    dst->name[MAX_NAME - 1] = '\0';
    dst->age = src->age;
    strncpy(dst->gender, src->gender, MAX_GENDER - 1);
    dst->gender[MAX_GENDER - 1] = '\0';
    strncpy(dst->phone, src->phone, MAX_PHONE - 1);
    dst->phone[MAX_PHONE - 1] = '\0';
    strncpy(dst->diagnosis, src->diagnosis, MAX_DESC - 1);
    dst->diagnosis[MAX_DESC - 1] = '\0';
    strncpy(dst->treatment, src->treatment, MAX_DESC - 1);
    dst->treatment[MAX_DESC - 1] = '\0';
}

// 工具: 构造一个患者记录（按值返回，便于直接传给 addPatient/modifyPatient）
PatientNode make_patient(const char *name, int age, const char *gender,
                         const char *phone, const char *diagnosis, const char *treatment) {
    PatientNode p;
    memset(&p, 0, sizeof(p));
    snprintf(p.name, sizeof(p.name), "%s", name);
    p.age = age;
    snprintf(p.gender, sizeof(p.gender), "%s", gender);
    snprintf(p.phone, sizeof(p.phone), "%s", phone);
    snprintf(p.diagnosis, sizeof(p.diagnosis), "%s", diagnosis);
    snprintf(p.treatment, sizeof(p.treatment), "%s", treatment);
    p.next = NULL;
    return p;
}

// 添加病人（尾插法）
PatientNode *add_patient(PatientNode **head, const PatientNode newInfo) {
    PatientNode *newNode = (PatientNode *) malloc(sizeof(PatientNode));
    if (newNode == NULL) {
        printf("内存分配失败！\n");
        return NULL;
    }

    //将新数据填入临时结构体newNode
    copy_patient_fields(newNode, &newInfo);

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
    return newNode;
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
PatientNode *modify_patient(PatientNode *head, const char *phone, const PatientNode newInfo) {
    PatientNode *current = head;
    while (current != NULL) {
        if (strcmp(current->phone, phone) == 0) {
            // 更新各个字段
            copy_patient_fields(current, &newInfo);
            return current; // 返回修改后的节点指针
        }
        current = current->next; // 继续下一个节点
    }
    return NULL; // 没有找到，返回 NULL
}

//删除患者信息
PatientNode *delete_patient(PatientNode *head, const char *phone) {
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

//按姓名字典序排序患者链表（使用归并排序）
PatientNode *sort_patients_by_name(PatientNode *head) {
    // 基本情况：空链表或单节点链表
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // 使用快慢指针找到中点
    PatientNode *slow = head;
    PatientNode *fast = head->next;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 分割链表为两半
    PatientNode *mid = slow->next;
    slow->next = NULL;

    // 递归排序两半
    PatientNode *left = sort_patients_by_name(head);
    PatientNode *right = sort_patients_by_name(mid);

    // 合并排序后的两半
    PatientNode dummy;
    PatientNode *tail = &dummy;
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

//按电话号码字典序排序患者链表（使用归并排序）
PatientNode *sort_patients_by_phone(PatientNode *head) {
    // 基本情况：空链表或单节点链表
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // 使用快慢指针找到中点
    PatientNode *slow = head;
    PatientNode *fast = head->next;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 分割链表为两半
    PatientNode *mid = slow->next;
    slow->next = NULL;

    // 递归排序两半
    PatientNode *left = sort_patients_by_phone(head);
    PatientNode *right = sort_patients_by_phone(mid);

    // 合并排序后的两半
    PatientNode dummy;
    PatientNode *tail = &dummy;
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

//按年龄升序排序患者链表（使用归并排序）
PatientNode *sort_patients_by_age(PatientNode *head) {
    // 基本情况：空链表或单节点链表
    if (head == NULL || head->next == NULL) {
        return head;
    }

    // 使用快慢指针找到中点
    PatientNode *slow = head;
    PatientNode *fast = head->next;
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 分割链表为两半
    PatientNode *mid = slow->next;
    slow->next = NULL;

    // 递归排序两半
    PatientNode *left = sort_patients_by_age(head);
    PatientNode *right = sort_patients_by_age(mid);

    // 合并排序后的两半
    PatientNode dummy;
    PatientNode *tail = &dummy;
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