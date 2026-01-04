/**
 * ============================================================================
 * 文件名称: patient_portal.c
 * 创建日期: 2026/01/04
 * 描述: 患者门户界面实现文件
 *       本文件实现了患者专属界面的所有功能
 *       患者只能进行挂号和查询自己的费用记录
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
extern AuthNode *g_current_user;

/* 外部文件路径引用 - 通过函数获取 */
extern void save_registrations_data(void);

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
        "2. 费用查询",
        "3. 退出登录",
        "4. 退出系统"
    };

    int menu_count = 4;
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
 * 患者挂号表单
 * 功能: 让患者选择医生和科室进行挂号
 * ============================================================================
 */
int ui_patient_register_form(WINDOW *parent_win, const char *patient_name) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int form_height = 14;
    int form_width = 60;
    int start_y = (max_y - form_height) / 2;
    int start_x = (max_x - form_width) / 2;

    WINDOW *form_win = newwin(form_height, form_width, start_y, start_x);
    keypad(form_win, TRUE);

    char doctorName[MAX_NAME] = "";
    char department[MAX_DEPT] = "";
    char date[MAX_NAME] = "";

    /* 自动生成今天的日期 */
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    snprintf(date, sizeof(date), "%04d-%02d-%02d", 
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);

    int current_field = 0;
    int ch;
    int label_x = 3;
    int input_x = 14;

    while (1) {
        werase(form_win);
        ui_draw_box(form_win, "预约挂号");

        /* 显示患者姓名（自动填入，不可编辑） */
        mvwprintw(form_win, 2, label_x, "患者姓名: %s", patient_name);

        mvwprintw(form_win, 4, label_x, "医生姓名:");
        mvwprintw(form_win, 6, label_x, "科    室:");
        mvwprintw(form_win, 8, label_x, "挂号日期:");

        /* 绘制输入框 */
        int i;
        for (i = 0; i < 3; i++) {
            if (current_field == i)
                wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            switch (i) {
                case 0: mvwprintw(form_win, 4, input_x, "[%-30s]", doctorName);
                    break;
                case 1: mvwprintw(form_win, 6, input_x, "[%-30s]", department);
                    break;
                case 2: mvwprintw(form_win, 8, input_x, "[%-30s]", date);
                    break;
            }
            if (current_field == i)
                wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }

        /* 确认按钮 */
        if (current_field == 3)
            wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        mvwprintw(form_win, 11, 15, "  [ 确认挂号 ]  ");
        if (current_field == 3)
            wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);

        wrefresh(form_win);
        ch = wgetch(form_win);

        switch (ch) {
            case KEY_UP:
                if (current_field > 0) current_field--;
                break;
            case KEY_DOWN:
            case '\t':
                if (current_field < 3) current_field++;
                break;
            case '\n':
            case KEY_ENTER:
                if (current_field < 3) {
                    char *target = NULL;
                    int row_y = 4 + current_field * 2;
                    int max_len = 30;

                    switch (current_field) {
                        case 0: target = doctorName; max_len = MAX_NAME - 1; break;
                        case 1: target = department; max_len = MAX_DEPT - 1; break;
                        case 2: target = date; max_len = MAX_NAME - 1; break;
                    }

                    if (target) {
                        mvwprintw(form_win, row_y, input_x, "[%-30s]", "");
                        wrefresh(form_win);
                        ui_input_string(form_win, row_y, input_x + 1, target, max_len, 0);
                    }
                    current_field++;
                } else {
                    /* 验证必填项 */
                    if (strlen(doctorName) == 0) {
                        ui_show_message("错误", "医生姓名为必填项", 2);
                    } else if (strlen(date) == 0) {
                        ui_show_message("错误", "挂号日期为必填项", 2);
                    } else {
                        /* 添加挂号记录 */
                        RegisterNode r = make_registration(patient_name, doctorName, department, date);
                        add_registration(&g_registrations, r);
                        save_registrations_data();
                        ui_show_message("成功", "挂号成功！请按时就诊。", 1);
                        delwin(form_win);
                        touchwin(stdscr);
                        refresh();
                        return 0;
                    }
                }
                break;
            case 27: /* ESC */
                delwin(form_win);
                touchwin(stdscr);
                refresh();
                return -1;
        }
    }
}

/*
 * ============================================================================
 * 患者费用查询界面
 * 功能: 只显示当前患者自己的费用记录
 * ============================================================================
 */
void ui_patient_query_bills(WINDOW *content_win, const char *patient_name) {
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

        /* 计算该患者的记录数量 */
        BillNode *cur = g_bills;
        int total_count = 0;
        while (cur != NULL) {
            if (strcmp(cur->patientName, patient_name) == 0) {
                total_count++;
            }
            cur = cur->next;
        }

        /* 显示费用记录 */
        cur = g_bills;
        int index = 0;
        int display_row = 0;
        int row_y = 3;
        double total_amount = 0.0;

        /* 跳过不属于当前患者的记录和分页 */
        int patient_idx = 0;
        while (cur != NULL) {
            if (strcmp(cur->patientName, patient_name) == 0) {
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
 * 功能: 只显示当前患者自己的挂号记录
 * ============================================================================
 */
void ui_patient_view_registrations(WINDOW *content_win, const char *patient_name) {
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

        /* 计算该患者的记录数量 */
        RegisterNode *cur = g_registrations;
        int total_count = 0;
        while (cur != NULL) {
            if (strcmp(cur->patientName, patient_name) == 0) {
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
            if (strcmp(cur->patientName, patient_name) == 0) {
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
 * 患者门户主界面
 * 功能: 显示患者专属界面，只包含挂号和费用查询功能
 * ============================================================================
 */
void ui_patient_portal_main(const char *patient_name) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    WINDOW *header_win = newwin(1, max_x, 0, 0);
    WINDOW *sidebar_win = newwin(max_y - 2, SIDEBAR_WIDTH, 1, 0);
    WINDOW *content_win = newwin(max_y - 2, max_x - SIDEBAR_WIDTH, 1, SIDEBAR_WIDTH);
    WINDOW *status_win = newwin(1, max_x, max_y - 1, 0);

    keypad(sidebar_win, TRUE);
    keypad(content_win, TRUE);

    int selected = 0;
    int ch;
    int running = 1;
    int menu_count = 4;

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
        mvwprintw(content_win, 3, 3, "尊敬的 %s，欢迎您！", patient_name);
        mvwprintw(content_win, 5, 3, "您可以进行以下操作:");
        mvwprintw(content_win, 7, 5, "• 我要挂号 - 预约医生就诊");
        mvwprintw(content_win, 8, 5, "• 费用查询 - 查看您的费用记录");
        mvwprintw(content_win, 10, 3, "使用 ↑↓ 键选择菜单项，按 Enter 进入");

        wattron(content_win, COLOR_PAIR(COLOR_PAIR_WARNING));
        mvwprintw(content_win, 12, 3, "提示: 患者账户只能查看自己的记录");
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
                        ui_patient_register_form(content_win, patient_name);
                        break;
                    case PATIENT_MENU_QUERY_BILL:
                        ui_patient_query_bills(content_win, patient_name);
                        break;
                    case PATIENT_MENU_LOGOUT:
                        running = 0;
                        break;
                    case PATIENT_MENU_EXIT:
                        delwin(header_win);
                        delwin(sidebar_win);
                        delwin(content_win);
                        delwin(status_win);
                        /* 设置特殊返回标志以完全退出 */
                        g_current_user = NULL;
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
