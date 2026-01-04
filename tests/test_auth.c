#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../include/auth.h"
#include "test.h"

int test_count_auth = 0;
int pass_count_auth = 0;

void test_make_user() {
    printf("测试 make_user 函数...\n");
    test_count_auth++;

    AuthNode user = make_user("U001", "admin", "123456", 0);
    (void) user;
    assert(strcmp(user.username, "admin") == 0);

    // 使用 cipher 函数验证加密后的密码
    char expected_password[MAX_NAME];
    cipher("123456", expected_password);
    assert(strcmp(user.password, expected_password) == 0);
    assert(user.role == 0);

    pass_count_auth++;
    printf("✓ make_user 测试通过\n\n");
}

void test_add_find_user() {
    printf("测试 add_user 和 find_user 函数...\n");
    test_count_auth++;

    AuthNode *head = NULL;
    AuthNode u1 = make_user("U001", "user1", "pass1", 1);
    AuthNode u2 = make_user("U001", "user2", "pass2", 2);

    add_user(&head, u1);
    add_user(&head, u2);


    AuthNode *found = find_user(head, "user1");
    assert(found != NULL);
    assert(strcmp(found->username, "user1") == 0);

    found = find_user(head, "user2");
    assert(found != NULL);
    assert(strcmp(found->username, "user2") == 0);

    found = find_user(head, "nonexistent");
    assert(found == NULL);

    // 测试重复添加
    AuthNode *result = add_user(&head, u1);
    assert(result == NULL); // 应该失败

    // 清理
    while (head != NULL) {
        AuthNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_auth++;
    printf("✓ add_user 和 find_user 测试通过\n\n");
}

void test_authenticate_user() {
    printf("测试 authenticate_user 函数...\n");
    test_count_auth++;

    AuthNode *head = NULL;
    AuthNode u1 = make_user("U001", "admin", "admin123", 0);
    add_user(&head, u1);

    assert(authenticate_user(head, "admin", "admin123") == 1);

    assert(authenticate_user(head, "admin", "wrongpass") == 0);
    assert(authenticate_user(head, "unknown", "admin123") == 0);

    // 清理
    while (head != NULL) {
        AuthNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_auth++;
    printf("✓ authenticate_user 测试通过\n\n");
}

void test_modify_user() {
    printf("测试 modify_user 函数...\n");
    test_count_auth++;

    AuthNode *head = NULL;
    AuthNode u1 = make_user("U001", "user1", "oldpass", 1);
    add_user(&head, u1);

    // 修改密码
    AuthNode newInfo = make_user("U001", "user1", "newpass", 1);
    AuthNode *modified = modify_user(head, "user1", newInfo);
    assert(modified != NULL);


    // 验证新密码
    assert(authenticate_user(head, "user1", "newpass") == 1);
    assert(authenticate_user(head, "user1", "oldpass") == 0);

    // 修改不存在的用户
    modified = modify_user(head, "nobody", newInfo);
    assert(modified == NULL);

    // 清理
    while (head != NULL) {
        AuthNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_auth++;
    printf("✓ modify_user 测试通过\n\n");
}

void test_delete_user() {
    printf("测试 delete_user 函数...\n");
    test_count_auth++;

    AuthNode *head = NULL;
    AuthNode u1 = make_user("U001", "user1", "pass1", 1);
    AuthNode u2 = make_user("U001", "user2", "pass2", 2);
    AuthNode u3 = make_user("U001", "user3", "pass3", 1);

    add_user(&head, u1);
    add_user(&head, u2);
    add_user(&head, u3);


    // 删除中间节点
    head = delete_user(head, "user2");
    assert(find_user(head, "user2") == NULL);
    assert(find_user(head, "user1") != NULL);
    assert(find_user(head, "user3") != NULL);

    // 删除头节点
    head = delete_user(head, "user3"); // user3 是最后添加的，所以在头部
    assert(find_user(head, "user3") == NULL);
    assert(find_user(head, "user1") != NULL);

    // 删除不存在的节点
    head = delete_user(head, "nobody");
    assert(find_user(head, "user1") != NULL);

    // 清理
    while (head != NULL) {
        AuthNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_auth++;
    printf("✓ delete_user 测试通过\n\n");
}

void run_auth_tests() {
    printf("============================================\n");
    printf("           开始 Auth 模块测试\n");
    printf("============================================\n");

    test_make_user();
    test_add_find_user();
    test_authenticate_user();
    test_modify_user();
    test_delete_user();

    printf("Auth 模块测试完成: %d/%d 通过\n\n", pass_count_auth, test_count_auth);
}

