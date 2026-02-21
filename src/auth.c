#include "../include/auth.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#define LEGACY_XOR_KEY 0xAA
#define PASSWORD_HASH_PREFIX "h2$"

static size_t bounded_strlen(const char *s, size_t max_len) {
    size_t len = 0;
    if (s == NULL) {
        return 0;
    }
    while (len < max_len && s[len] != '\0') {
        len++;
    }
    return len;
}

static int is_valid_role(int role) {
    return role >= 0 && role <= 2;
}

static void legacy_cipher(const char *input, char *output) {
    int i;
    if (input == NULL || output == NULL) {
        if (output != NULL) {
            output[0] = '\0';
        }
        return;
    }
    for (i = 0; i < MAX_NAME - 1 && input[i] != '\0'; i++) {
        output[i] = (char) (input[i] ^ LEGACY_XOR_KEY);
    }
    output[i] = '\0';
}

static uint64_t fnv1a64_seeded(const unsigned char *data, size_t len, uint64_t seed) {
    uint64_t hash = 1469598103934665603ULL ^ seed;
    size_t i;
    for (i = 0; i < len; i++) {
        hash ^= (uint64_t) data[i];
        hash *= 1099511628211ULL;
        hash ^= (hash >> 32);
    }
    return hash;
}

void cipher(const char *input, char *output) {
    size_t input_len;
    uint64_t h1, h2;

    if (output == NULL) {
        return;
    }

    if (input == NULL) {
        output[0] = '\0';
        return;
    }

    input_len = bounded_strlen(input, MAX_NAME - 1);
    h1 = fnv1a64_seeded((const unsigned char *) input, input_len, 0x9e3779b185ebca87ULL);
    h2 = fnv1a64_seeded((const unsigned char *) input, input_len, 0xc2b2ae3d27d4eb4fULL);

    snprintf(output,
             MAX_NAME,
             PASSWORD_HASH_PREFIX "%016llx%016llx",
             (unsigned long long) h1,
             (unsigned long long) h2);
}

AuthNode make_user(const char *username, const char *password, int role) {
    AuthNode node;
    // 初始化内存
    memset(&node, 0, sizeof(AuthNode));

    // 复制用户名
    if (username != NULL) {
        strncpy(node.username, username, MAX_NAME - 1);
        node.username[MAX_NAME - 1] = '\0';
    }

    // 计算密码摘要（非可逆）
    cipher(password != NULL ? password : "", node.password);

    node.role = is_valid_role(role) ? role : 2;

    node.next = NULL;
    return node;
}

AuthNode *add_user(AuthNode **head, AuthNode newInfo) {
    if (head == NULL) {
        return NULL;
    }

    newInfo.username[MAX_NAME - 1] = '\0';
    newInfo.password[MAX_NAME - 1] = '\0';
    if (!is_valid_role(newInfo.role)) {
        newInfo.role = 2;
    }

    // 检查用户是否已存在
    if (find_user(*head, newInfo.username) != NULL) {
        return NULL;
    }

    AuthNode *newNode = (AuthNode *) malloc(sizeof(AuthNode));
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
        // 更新密码摘要
        strncpy(target->password, newInfo.password, MAX_NAME - 1);
        target->password[MAX_NAME - 1] = '\0';
        if (is_valid_role(newInfo.role)) {
            target->role = newInfo.role;
        }

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
    if (username == NULL) {
        return NULL;
    }
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
    char legacy_input[MAX_NAME];
    cipher(password != NULL ? password : "", encrypted_input);

    if (strcmp(user->password, encrypted_input) == 0) {
        return 1; // 验证成功
    }

    // 兼容旧版异或密码：首次成功登录后自动升级到新格式
    if (strncmp(user->password, PASSWORD_HASH_PREFIX, strlen(PASSWORD_HASH_PREFIX)) != 0) {
        legacy_cipher(password != NULL ? password : "", legacy_input);
        if (strcmp(user->password, legacy_input) == 0) {
            strncpy(user->password, encrypted_input, MAX_NAME - 1);
            user->password[MAX_NAME - 1] = '\0';
            return 1;
        }
    }
    return 0; // 密码错误
}
