#ifndef AUTH_H
#define AUTH_H

#include "datastruct.h"

/**
 *功能：构建一个用户节点
 *参数：username - 用户名
 *      password - 密码
 *      role - 权限角色
 *返回值：构造好的用户节点指针
 */
AuthNode make_user(const char *username, const char *password, int role);

/**
 * 功能：添加一个新用户
 * 参数：head - 链表头指针
 *       newInfo - 包含新用户信息的结构体
 */
AuthNode *add_user(AuthNode **head, AuthNode newInfo);

/**
 *功能：修改用户信息
 *参数：head - 链表头指针
 *      username - 用户名（唯一标识）
 *      newInfo - 包含更新后用户信息的结构体
 *返回值：修改的用户节点指针，未找到返回 NULL
 */
AuthNode *modify_user(AuthNode *head, const char *username, AuthNode newInfo);

/**
 *功能：删除用户信息
 *参数：head - 链表头指针
 *      username - 要查找的用户名
 *返回值：删除成功后的头指针，未找到返回 NULL
 */
AuthNode *delete_user(AuthNode *head, const char *username);

/**
 *功能：判断用户是否存在
 *参数：head - 链表头指针
 *      username - 要查找的用户名
 *返回值：找到的用户节点指针，未找到返回 NULL
 */
AuthNode *find_user(AuthNode *head, const char *username);

/**
 *功能：验证用户名和密码
 *参数：head - 链表头指针
 *      username - 要验证的用户名
 *      password - 要验证的密码
 *返回值：验证成功返回 1，失败返回 0
 */
int authenticate_user(AuthNode *head, const char *username, const char *password);

/**
 * 功能：对密码做不可逆摘要（兼容旧数据时会在认证中处理）
 * 参数：input - 输入字符串
 *       output - 输出缓冲区（至少 MAX_NAME 大小）
 */
void cipher(const char *input, char *output);

#endif //AUTH_H
