/**
 * ============================================================================
 * 文件名称: patient_portal.c
 * 创建日期: 2026/01/04
 * 描述: 患者门户界面实现文件
 *       本文件实现了患者专属界面的所有功能
 *       患者只能进行挂号和查询自己的费用记录
 *       使用 userId 进行数据匹配
 * ============================================================================
 */

#include "../include/patient_portal.h"
#include "../include/ui.h"
#include "../include/datastruct.h"
#include "../include/registration.h"
#include "../include/bill.h"
#include "../include/file_io.h"

#if defined(_WIN32) || defined(WIN32)
#include <curses.h>
#else
#include <ncurses.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <time.h>

/* 外部全局变量引用 */
extern RegisterNode *g_registrations;
extern BillNode *g_bills;
extern DoctorNode *g_doctors;
extern PatientNode *g_patients;
extern AuthNode *g_current_user;

/* 外部文件路径引用 - 通过函数获取 */
extern void save_registrations_data(void);

/* 生成唯一订单ID的辅助函数 */
static void generate_order_id(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    snprintf(buffer, size, "R%04d%02d%02d%02d%02d%02d",
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
}

/*
 * ============================================================================
 * 绘制患者门户侧边栏
 * ============================================================================
 */
void ui_patient_portal_draw_sidebar(WINDOW *win, int selected) {
    int max_y = getmaxy(win);

    werase(win);
    ui_draw_box(win, "患者服务");

    const char *menu_items[] = {
        "1. 我要挂号",
        "2. 我的挂号",
        "3. 费用查询",
        "4. 退出登录",
        "5. 退出系统"
    };

    int menu_count = 5;
    int start_y = 2;
    int i;

    for (i = 0; i < menu_count && start_y + i < max_y - 1; i++) {
        if (i == selected) {
            wattron(win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
            mvwprintw(win, start_y + i, 1, " %-17s", menu_items[i]);
            wattroff(win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        } else {
            mvwprintw(win, start_y + i, 2, "%-17s", menu_items[i]);
        }
    }

    wrefresh(win);
}

/*
 * ============================================================================
 * 患者挂号界面（带医生列表和筛选）
 * 功能: 显示医生列表，支持按科室筛选，选择医生进行挂号
 * ============================================================================
 */
int ui_patient_register_form(WINDOW *parent_win, const char *userId, const char *patient_name) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *content_win = parent_win;
    keypad(content_win, TRUE);

    int selected_row = 0;
    int start_index = 0;
    char dept_filter[MAX_DEPT] = "";
    int ch;

    while (1) {
        werase(content_win);
        ui_draw_box(content_win, "选择医生挂号");

        int win_max_y, win_max_x;
        getmaxyx(content_win, win_max_y, win_max_x);

        /* 显示筛选区域 */
        mvwprintw(content_win, 1, 2, "科室筛选: [%-15s] 按F键输入筛选, C键清除筛选", dept_filter);

        /* 表头 */
        const char *headers[] = {"序号", "姓名", "科室", "排班", "电话"};
        int col_widths[] = {6, 12, 15, 20, 15};
        int col_count = 5;
        int x = 1;
        int i;

        wattron(content_win, COLOR_PAIR(COLOR_PAIR_HEADER) | A_BOLD | A_UNDERLINE);
        for (i = 0; i < col_count; i++) {
            mvwprintw(content_win, 3, x, "%-*s", col_widths[i], headers[i]);
            x += col_widths[i] + 1;
        }
        wattroff(content_win, COLOR_PAIR(COLOR_PAIR_HEADER) | A_BOLD | A_UNDERLINE);

        /* 统计符合条件的医生数量 */
        DoctorNode *cur = g_doctors;
        int total_count = 0;
        while (cur != NULL) {
            if (strlen(dept_filter) == 0 || strstr(cur->department, dept_filter) != NULL) {
                total_count++;
            }
            cur = cur->next;
        }

        /* 显示医生列表 */
        cur = g_doctors;
        int display_row = 0;
        int row_y = 5;
        int filtered_idx = 0;

        while (cur != NULL && row_y < win_max_y - 4) {
            /* 应用科室筛选 */
            if (strlen(dept_filter) == 0 || strstr(cur->department, dept_filter) != NULL) {
                if (filtered_idx >= start_index) {
                    x = 1;
                    if (display_row == selected_row)
                        wattron(content_win, COLOR_PAIR(COLOR_PAIR_SELECT));

                    mvwprintw(content_win, row_y, x, "%-*d", col_widths[0], filtered_idx + 1);
                    x += col_widths[0] + 1;
                    mvwprintw(content_win, row_y, x, "%-*.*s", col_widths[1], col_widths[1], cur->name);
                    x += col_widths[1] + 1;
                    mvwprintw(content_win, row_y, x, "%-*.*s", col_widths[2], col_widths[2], cur->department);
                    x += col_widths[2] + 1;
                    mvwprintw(content_win, row_y, x, "%-*.*s", col_widths[3], col_widths[3], cur->schedule);
                    x += col_widths[3] + 1;
                    mvwprintw(content_win, row_y, x, "%-*.*s", col_widths[4], col_widths[4], cur->phone);

                    if (display_row == selected_row)
                        wattroff(content_win, COLOR_PAIR(COLOR_PAIR_SELECT));

                    row_y++;
                    display_row++;
                }
                filtered_idx++;
            }
            cur = cur->next;
        }

        if (total_count == 0) {
            mvwprintw(content_win, 6, 3, "暂无医生信息，请联系管理员添加");
        }

        /* 显示操作提示 */
        mvwprintw(content_win, win_max_y - 3, 2, "共 %d 位医生", total_count);
        mvwprintw(content_win, win_max_y - 2, 2, "[↑↓]选择 [Enter]挂号 [F]筛选 [C]清除 [ESC]返回");

        wrefresh(content_win);
        ch = wgetch(content_win);

        switch (ch) {
            case KEY_UP:
            case 'k':
                if (selected_row > 0) selected_row--;
                else if (start_index > 0) { start_index--; }
                break;
            case KEY_DOWN:
            case 'j':
                if (selected_row < display_row - 1) selected_row++;
                else if (start_index + display_row < total_count) start_index++;
                break;
            case 'f':
            case 'F': {
                /* 输入科室筛选 */
                mvwprintw(content_win, 1, 14, "[%-15s]", "");
                wrefresh(content_win);
                ui_input_string(content_win, 1, 15, dept_filter, MAX_DEPT - 1, 0);
                selected_row = 0;
                start_index = 0;
                break;
            }
            case 'c':
            case 'C':
                /* 清除筛选 */
                memset(dept_filter, 0, sizeof(dept_filter));
                selected_row = 0;
                start_index = 0;
                break;
            case '\n':
            case KEY_ENTER: {
                if (total_count == 0) break;

                /* 找到选中的医生 */
                DoctorNode *selected_doctor = NULL;
                cur = g_doctors;
                int idx = 0;
                int target_idx = start_index + selected_row;

                while (cur != NULL) {
                    if (strlen(dept_filter) == 0 || strstr(cur->department, dept_filter) != NULL) {
                        if (idx == target_idx) {
                            selected_doctor = cur;
                            break;
                        }
                        idx++;
                    }
                    cur = cur->next;
                }

                if (selected_doctor != NULL) {
                    /* 确认挂号对话框 */
                    char confirm_msg[256];
                    snprintf(confirm_msg, sizeof(confirm_msg), 
                             "确定预约 %s (%s) 医生吗？", 
                             selected_doctor->name, selected_doctor->department);
                    
                    if (ui_confirm_dialog("确认挂号", confirm_msg)) {
                        /* 生成今天的日期和订单ID */
                        time_t now = time(NULL);
                        struct tm *tm_info = localtime(&now);
                        char date[MAX_NAME];
                        char orderId[MAX_ID];
                        snprintf(date, sizeof(date), "%04d-%02d-%02d", 
                                 tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
                        generate_order_id(orderId, sizeof(orderId));

                        /* 添加挂号记录 */
                        RegisterNode r = make_registration(orderId, userId, patient_name,
                                                           selected_doctor->name, 
                                                           selected_doctor->department, date);
                        add_registration(&g_registrations, r);
                        save_registrations_data();
                        ui_show_message("成功", "挂号成功！请按时就诊。", 1);
                        touchwin(stdscr);
                        refresh();
                        return 0;
                    }
                }
                break;
            }
            case 27: /* ESC */
            case 'b':
            case 'B':
                touchwin(stdscr);
                refresh();
                return -1;
        }
    }
}

/*
 * ============================================================================
 * 患者费用查询界面
 * 功能: 只显示当前患者自己的费用记录（按patientId匹配）
 * ============================================================================
 */
void ui_patient_query_bills(WINDOW *content_win, const char *userId) {
    int max_y, max_x;
    getmaxyx(content_win, max_y, max_x);

    int selected_row = 0;
    int start_index = 0;
    int ch;

    while (1) {
        werase(content_win);
        ui_draw_box(content_win, "我的费用记录");

        /* 表头 */
        const char *headers[] = {"序号", "收费项目", "金额"};
        int col_widths[] = {6, 25, 12};
        int col_count = 3;
        int x = 1;
        int i;

        wattron(content_win, COLOR_PAIR(COLOR_PAIR_HEADER) | A_BOLD | A_UNDERLINE);
        for (i = 0; i < col_count; i++) {
            mvwprintw(content_win, 1, x, "%-*s", col_widths[i], headers[i]);
            x += col_widths[i] + 1;
        }
        wattroff(content_win, COLOR_PAIR(COLOR_PAIR_HEADER) | A_BOLD | A_UNDERLINE);

        /* 计算该患者的记录数量（按patientId匹配） */
        BillNode *cur = g_bills;
        int total_count = 0;
        while (cur != NULL) {
            if (strcmp(cur->patientId, userId) == 0) {
                total_count++;
            }
            cur = cur->next;
        }

        /* 显示费用记录 */
        cur = g_bills;
        int display_row = 0;
        int row_y = 3;
        double total_amount = 0.0;

        /* 跳过不属于当前患者的记录和分页 */
        int patient_idx = 0;
        while (cur != NULL) {
            if (strcmp(cur->patientId, userId) == 0) {
                if (patient_idx >= start_index && row_y < max_y - 4) {
                    x = 1;
                    if (display_row == selected_row)
                        wattron(content_win, COLOR_PAIR(COLOR_PAIR_SELECT));

                    mvwprintw(content_win, row_y, x, "%-*d", col_widths[0], patient_idx + 1);
                    x += col_widths[0] + 1;
                    mvwprintw(content_win, row_y, x, "%-*.*s", col_widths[1], col_widths[1], cur->itemName);
                    x += col_widths[1] + 1;
                    mvwprintw(content_win, row_y, x, "%-*.2f", col_widths[2], cur->amount);

                    if (display_row == selected_row)
                        wattroff(content_win, COLOR_PAIR(COLOR_PAIR_SELECT));

                    row_y++;
                    display_row++;
                }
                total_amount += cur->amount;
                patient_idx++;
            }
            cur = cur->next;
        }

        /* 显示统计信息 */
        mvwprintw(content_win, max_y - 3, 2, "共 %d 条记录", total_count);
        wattron(content_win, COLOR_PAIR(COLOR_PAIR_SUCCESS) | A_BOLD);
        mvwprintw(content_win, max_y - 3, 20, "总费用: %.2f 元", total_amount);
        wattroff(content_win, COLOR_PAIR(COLOR_PAIR_SUCCESS) | A_BOLD);
        mvwprintw(content_win, max_y - 2, 2, "[↑↓]选择  [ESC/B]返回");

        wrefresh(content_win);
        ch = wgetch(content_win);

        switch (ch) {
            case KEY_UP:
            case 'k':
                if (selected_row > 0) selected_row--;
                else if (start_index > 0) start_index--;
                break;
            case KEY_DOWN:
            case 'j':
                if (selected_row < display_row - 1) selected_row++;
                else if (start_index + display_row < total_count) start_index++;
                break;
            case 'b':
            case 'B':
            case 27: /* ESC */
                return;
        }
    }
}

/*
 * ============================================================================
 * 患者查看挂号记录
 * 功能: 只显示当前患者自己的挂号记录（按patientId匹配）
 * ============================================================================
 */
void ui_patient_view_registrations(WINDOW *content_win, const char *userId) {
    int max_y, max_x;
    getmaxyx(content_win, max_y, max_x);

    int selected_row = 0;
    int start_index = 0;
    int ch;

    while (1) {
        werase(content_win);
        ui_draw_box(content_win, "我的挂号记录");

        /* 表头 */
        const char *headers[] = {"序号", "医生", "科室", "日期"};
        int col_widths[] = {6, 15, 20, 15};
        int col_count = 4;
        int x = 1;
        int i;

        wattron(content_win, COLOR_PAIR(COLOR_PAIR_HEADER) | A_BOLD | A_UNDERLINE);
        for (i = 0; i < col_count; i++) {
            mvwprintw(content_win, 1, x, "%-*s", col_widths[i], headers[i]);
            x += col_widths[i] + 1;
        }
        wattroff(content_win, COLOR_PAIR(COLOR_PAIR_HEADER) | A_BOLD | A_UNDERLINE);

        /* 计算该患者的记录数量（按patientId匹配） */
        RegisterNode *cur = g_registrations;
        int total_count = 0;
        while (cur != NULL) {
            if (strcmp(cur->patientId, userId) == 0) {
                total_count++;
            }
            cur = cur->next;
        }

        /* 显示挂号记录 */
        cur = g_registrations;
        int display_row = 0;
        int row_y = 3;
        int patient_idx = 0;

        while (cur != NULL) {
            if (strcmp(cur->patientId, userId) == 0) {
                if (patient_idx >= start_index && row_y < max_y - 3) {
                    x = 1;
                    if (display_row == selected_row)
                        wattron(content_win, COLOR_PAIR(COLOR_PAIR_SELECT));

                    mvwprintw(content_win, row_y, x, "%-*d", col_widths[0], patient_idx + 1);
                    x += col_widths[0] + 1;
                    mvwprintw(content_win, row_y, x, "%-*.*s", col_widths[1], col_widths[1], cur->doctorName);
                    x += col_widths[1] + 1;
                    mvwprintw(content_win, row_y, x, "%-*.*s", col_widths[2], col_widths[2], cur->department);
                    x += col_widths[2] + 1;
                    mvwprintw(content_win, row_y, x, "%-*.*s", col_widths[3], col_widths[3], cur->date);

                    if (display_row == selected_row)
                        wattroff(content_win, COLOR_PAIR(COLOR_PAIR_SELECT));

                    row_y++;
                    display_row++;
                }
                patient_idx++;
            }
            cur = cur->next;
        }

        if (total_count == 0) {
            mvwprintw(content_win, 4, 3, "暂无挂号记录");
        }

        mvwprintw(content_win, max_y - 2, 2, "共 %d 条记录  [↑↓]选择  [ESC/B]返回", total_count);

        wrefresh(content_win);
        ch = wgetch(content_win);

        switch (ch) {
            case KEY_UP:
            case 'k':
                if (selected_row > 0) selected_row--;
                else if (start_index > 0) start_index--;
                break;
            case KEY_DOWN:
            case 'j':
                if (selected_row < display_row - 1) selected_row++;
                else if (start_index + display_row < total_count) start_index++;
                break;
            case 'b':
            case 'B':
            case 27:
                return;
        }
    }
}

/*
 * ============================================================================
 * 查找患者真实姓名（根据userId）
 * ============================================================================
 */
static const char* find_patient_real_name(const char *userId) {
    PatientNode *cur = g_patients;
    while (cur != NULL) {
        if (strcmp(cur->userId, userId) == 0) {
            return cur->name;
        }
        cur = cur->next;
    }
    return userId;  /* 如果找不到，返回userId */
}

/*
 * ============================================================================
 * 患者门户主界面
 * 功能: 显示患者专属界面，只包含挂号和费用查询功能
 * ============================================================================
 */
void ui_patient_portal_main(const char *userId, const char *patient_name) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *header_win = newwin(1, max_x, 0, 0);
    WINDOW *sidebar_win = newwin(max_y - 2, SIDEBAR_WIDTH, 1, 0);
    WINDOW *content_win = newwin(max_y - 2, max_x - SIDEBAR_WIDTH, 1, SIDEBAR_WIDTH);
    WINDOW *status_win = newwin(1, max_x, max_y - 1, 0);

    keypad(sidebar_win, TRUE);
    keypad(content_win, TRUE);

    /* 获取患者的真实姓名用于显示 */
    const char *real_name = find_patient_real_name(userId);

    int selected = 0;
    int ch;
    int running = 1;
    int menu_count = 5;

    while (running) {
        touchwin(stdscr);
        refresh();

        /* 绘制标题栏 */
        ui_draw_header(header_win);

        /* 绘制患者门户侧边栏 */
        ui_patient_portal_draw_sidebar(sidebar_win, selected);

        /* 绘制状态栏 */
        ui_draw_status_bar(status_win, NULL);

        /* 绘制内容区域 */
        werase(content_win);
        ui_draw_box(content_win, "欢迎使用患者服务平台");
        mvwprintw(content_win, 3, 3, "尊敬的 %s，欢迎您！", real_name);
        mvwprintw(content_win, 5, 3, "您可以进行以下操作:");
        mvwprintw(content_win, 7, 5, "• 我要挂号 - 查看医生列表，选择医生预约挂号");
        mvwprintw(content_win, 8, 5, "• 我的挂号 - 查看您已预约的挂号记录");
        mvwprintw(content_win, 9, 5, "• 费用查询 - 查看您的费用记录");
        mvwprintw(content_win, 11, 3, "使用 ↑↓ 键选择菜单项，按 Enter 进入");

        wattron(content_win, COLOR_PAIR(COLOR_PAIR_WARNING));
        mvwprintw(content_win, 13, 3, "提示: 患者账户只能查看自己的记录");
        wattroff(content_win, COLOR_PAIR(COLOR_PAIR_WARNING));

        wrefresh(content_win);

        ch = wgetch(sidebar_win);

        switch (ch) {
            case KEY_UP:
            case 'k':
                if (selected > 0) selected--;
                break;
            case KEY_DOWN:
            case 'j':
                if (selected < menu_count - 1) selected++;
                break;
            case '\n':
            case KEY_ENTER:
                switch (selected) {
                    case PATIENT_MENU_REGISTER:
                        ui_patient_register_form(content_win, userId, real_name);
                        break;
                    case PATIENT_MENU_MY_REGISTER:
                        ui_patient_view_registrations(content_win, userId);
                        break;
                    case PATIENT_MENU_QUERY_BILL:
                        ui_patient_query_bills(content_win, userId);
                        break;
                    case PATIENT_MENU_LOGOUT:
                        running = 0;
                        break;
                    case PATIENT_MENU_EXIT:
                        delwin(header_win);
                        delwin(sidebar_win);
                        delwin(content_win);
                        delwin(status_win);
                        return;
                }
                touchwin(stdscr);
                refresh();
                break;
            case 'q':
            case 'Q':
                running = 0;
                break;
        }
    }

    delwin(header_win);
    delwin(sidebar_win);
    delwin(content_win);
    delwin(status_win);
}
