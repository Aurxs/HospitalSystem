//
// Created by 罗金源 on 2025/12/19.
//

#include "../include/auth.h"
#include <string.h>
#include <stdlib.h>

// 使用 0xAA 作为异或密钥，避免生成 '\0' (针对 ASCII 可打印字符)
#define XOR_KEY 0xAA

/**
 * 内部辅助函数：异或加密/解密
 * 注意：output 必须有足够的空间 (至少 MAX_NAME)
 */
void cipher(const char *input, char *output) {
    int i;
    for (i = 0; i < MAX_NAME - 1 && input[i] != '\0'; i++) {
        output[i] = (char)(input[i] ^ XOR_KEY);
    }
    output[i] = '\0';
}

AuthNode make_user(const char *username, const char *password) {
    AuthNode node;
    // 初始化内存
    memset(&node, 0, sizeof(AuthNode));

    // 复制用户名
    strncpy(node.username, username, MAX_NAME - 1);

    // 加密密码
    cipher(password, node.password);

    node.next = NULL;
    return node;
}

AuthNode *add_user(AuthNode **head, AuthNode newInfo) {
    // 检查用户是否已存在
    if (find_user(*head, newInfo.username) != NULL) {
        return NULL;
    }

    AuthNode *newNode = (AuthNode *)malloc(sizeof(AuthNode));
    if (newNode == NULL) {
        return NULL;
    }

    *newNode = newInfo; // 结构体复制
    newNode->next = *head;
    *head = newNode;

    return newNode;
}

AuthNode *modify_user(AuthNode *head, const char *username, AuthNode newInfo) {
    AuthNode *target = find_user(head, username);
    if (target != NULL) {
        // 更新密码 (newInfo.password 应该是已经加密过的)
        strncpy(target->password, newInfo.password, MAX_NAME - 1);
        target->password[MAX_NAME - 1] = '\0';

        // 如果需要更新其他信息可以在这里添加
        // 注意：通常不修改用户名，因为它是标识符

        return target;
    }
    return NULL;
}

AuthNode *delete_user(AuthNode *head, const char *username) {
    AuthNode *current = head;
    AuthNode *prev = NULL;

    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            if (prev == NULL) {
                // 删除的是头节点
                head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return head;
        }
        prev = current;
        current = current->next;
    }
    return head;
}

AuthNode *find_user(AuthNode *head, const char *username) {
    AuthNode *current = head;
    while (current != NULL) {
        if (strcmp(current->username, username) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

int authenticate_user(AuthNode *head, const char *username, const char *password) {
    AuthNode *user = find_user(head, username);
    if (user == NULL) {
        return 0; // 用户不存在
    }

    char encrypted_input[MAX_NAME];
    cipher(password, encrypted_input);

    if (strcmp(user->password, encrypted_input) == 0) {
        return 1; // 验证成功
    }
    return 0; // 密码错误
}
