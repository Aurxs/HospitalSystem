/**
 * ============================================================================
 * 文件名称: patient_portal.h
 * 创建日期: 2026/01/04
 * 描述: 患者门户界面头文件
 *       本文件定义了患者专属界面的所有函数
 *       患者只能进行挂号和查询自己的费用记录
 * ============================================================================
 */

#ifndef PATIENT_PORTAL_H
#define PATIENT_PORTAL_H

#include "datastruct.h"
#if defined(_WIN32) || defined(WIN32)
#include <curses.h>
#else
#include <ncurses.h>
#endif

/*
 * ============================================================================
 * 患者门户菜单选项定义
 * ============================================================================
 */
#define PATIENT_MENU_REGISTER    0    /* 挂号 */
#define PATIENT_MENU_QUERY_BILL  1    /* 费用查询 */
#define PATIENT_MENU_LOGOUT      2    /* 退出登录 */
#define PATIENT_MENU_EXIT        3    /* 退出系统 */

/*
 * ============================================================================
 * 患者门户界面函数声明
 * ============================================================================
 */

/**
 * 函数名: ui_patient_portal_main
 * 功能: 患者门户主界面
 * 说明: 显示患者专属界面，只包含挂号和费用查询功能
 * 参数:
 *   - patient_name: 当前登录患者的用户名
 * 返回值: 无
 */
void ui_patient_portal_main(const char *patient_name);

/**
 * 函数名: ui_patient_portal_draw_sidebar
 * 功能: 绘制患者门户侧边栏
 * 参数:
 *   - win: 侧边栏窗口
 *   - selected: 当前选中项
 */
void ui_patient_portal_draw_sidebar(WINDOW *win, int selected);

/**
 * 函数名: ui_patient_register_form
 * 功能: 患者挂号表单
 * 说明: 让患者选择医生和科室进行挂号，患者姓名自动填入
 * 参数:
 *   - parent_win: 父窗口
 *   - patient_name: 当前登录的患者姓名
 * 返回值: 0-成功, -1-取消或失败
 */
int ui_patient_register_form(WINDOW *parent_win, const char *patient_name);

/**
 * 函数名: ui_patient_query_bills
 * 功能: 患者费用查询界面
 * 说明: 只显示当前患者自己的费用记录
 * 参数:
 *   - content_win: 内容区域窗口
 *   - patient_name: 当前登录的患者姓名
 */
void ui_patient_query_bills(WINDOW *content_win, const char *patient_name);

/**
 * 函数名: ui_patient_view_registrations
 * 功能: 患者查看挂号记录
 * 说明: 只显示当前患者自己的挂号记录
 * 参数:
 *   - content_win: 内容区域窗口
 *   - patient_name: 当前登录的患者姓名
 */
void ui_patient_view_registrations(WINDOW *content_win, const char *patient_name);

#endif // PATIENT_PORTAL_H
