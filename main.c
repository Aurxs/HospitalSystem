/**
 * ============================================================================
 * 医院管理系统 - 主程序入口
 * 作者: 罗金源
 * 描述: 本程序是一个基于ncurses的医院管理系统
 *       支持患者、医生、药品、挂号、费用和用户管理
 * 
 * 代码架构说明:
 *   - main.c: 主程序入口，负责业务流程控制和调度
 *             包含：初始化 → 登录 → 角色分发 → 界面调用 → 清理
 *   
 *   - src/ui.c: UI基础组件和管理员/医生界面
 *               基础组件：ui_draw_box, ui_show_message, ui_confirm_dialog,
 *                        ui_input_string, ui_draw_table_header 等
 *               界面函数：ui_login_screen, ui_main_screen, 
 *                        ui_patient_management, ui_doctor_management 等
 *   
 *   - src/ui/patient_portal.c: 患者专属界面组件
 *               包含：ui_patient_portal_main (患者主界面)
 *                    ui_patient_register_form (挂号界面，带医生列表筛选)
 *                    ui_patient_view_registrations (我的挂号记录)
 *                    ui_patient_query_bills (费用查询)
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datastruct.h"
#include "file_io.h"
#include "ui.h"
#include "patient_portal.h"

/**
 * 主函数 - 程序入口
 * 功能: 控制整个程序的业务流程
 *   1. 调用 ui_init() 初始化UI系统和加载数据
 *   2. 循环显示登录界面，获取用户角色
 *   3. 根据用户角色分发到不同的界面：
 *      - ROLE_PATIENT: 调用 ui_patient_portal_main() 患者门户
 *      - ROLE_DOCTOR/ROLE_ADMIN: 调用 ui_main_screen() 管理界面
 *   4. 调用 ui_cleanup() 保存数据并清理资源
 */
int main() {
    /* 初始化UI系统和数据 */
    if (ui_init() != 0) {
        printf("系统初始化失败\n");
        return -1;
    }

    int running = 1;
    
    /* 主业务循环 */
    while (running) {
        /* 清屏并刷新 */
        clear();
        refresh();
        
        /* 显示登录界面，获取用户角色 */
        int role = ui_login_screen();
        
        if (role == -2) {
            /* 用户选择退出系统 */
            running = 0;
        } else if (role >= 0) {
            /* 根据角色分发到不同的界面 */
            if (role == ROLE_PATIENT) {
                /* 患者使用独立的患者门户界面 */
                ui_patient_portal_main(g_current_user->username);
            } else {
                /* 医生和管理员使用主界面 */
                ui_main_screen(role);
            }
            /* 清除当前用户信息，准备下一次登录 */
            g_current_user = NULL;
        }
    }

    /* 清理资源并退出 */
    ui_cleanup();
    return 0;
}