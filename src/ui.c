/**
 * ============================================================================
 * 文件名称: ui.c
 * 作者: 罗金源
 * 创建日期: 2025/12/19
 * 修改日期: 2026/01/03
 * 描述: 医院管理系统用户界面实现文件
 *       本文件实现了系统的完整用户界面功能
 *       使用 ncurses 库实现终端图形界面
 *       
 * 功能概述:
 *   1. 登录界面 - 支持管理员/医生/患者三种角色登录
 *   2. 主界面 - 左侧菜单栏 + 右侧内容区域的经典布局
 *   3. 数据表格 - 类似Excel的表格显示，支持选择、分页
 *   4. 增删改查 - 完整的CRUD操作界面
 *   5. 排序功能 - 支持多种排序方式
 *   
 * 教学说明:
 *   本代码包含详细的中文注释，适合C语言初学者学习
 *   ncurses是Unix/Linux系统下常用的终端图形库
 *   通过本代码可以学习:
 *   - ncurses基本使用方法
 *   - 终端界面布局设计
 *   - 用户交互处理
 *   - 链表数据的界面展示
 * ============================================================================
 */

/*
 * ============================================================================
 * 头文件引入
 * ============================================================================
 */
#include "../include/ui.h"          /* UI头文件，包含函数声明 */
#include "../include/datastruct.h"   /* 数据结构定义 */
#include "../include/patient.h"      /* 患者管理函数 */
#include "../include/doctor.h"       /* 医生管理函数 */
#include "../include/drug.h"         /* 药品管理函数 */
#include "../include/registration.h" /* 挂号管理函数 */
#include "../include/bill.h"         /* 费用管理函数 */
#include "../include/auth.h"         /* 用户认证函数 */
#include "../include/file_io.h"      /* 文件读写函数 */

#include <ncurses.h>    /* ncurses库 - 终端图形界面 */
#include <string.h>     /* 字符串处理函数 */
#include <stdlib.h>     /* 标准库函数 */
#include <locale.h>     /* 本地化设置（支持中文显示） */
#include <ctype.h>      /* 字符处理函数 */

/*
 * ============================================================================
 * 全局变量定义
 * 说明: 这些变量在整个UI模块中共享使用
 * ============================================================================
 */

/* 数据链表头指针 - 指向各类数据的链表头 */
PatientNode   *g_patients = NULL;       /* 患者数据链表头 */
DoctorNode    *g_doctors = NULL;        /* 医生数据链表头 */
DrugNode      *g_drugs = NULL;          /* 药品数据链表头 */
RegisterNode  *g_registrations = NULL;  /* 挂号记录链表头 */
BillNode      *g_bills = NULL;          /* 费用记录链表头 */
AuthNode      *g_users = NULL;          /* 用户账户链表头 */

/* 当前登录用户信息 */
AuthNode      *g_current_user = NULL;   /* 当前登录的用户节点 */

/* 数据文件路径 - 相对于程序运行目录 */
static const char *DATA_PATH_PATIENTS = "data/patients.txt";
static const char *DATA_PATH_DOCTORS = "data/doctors.txt";
static const char *DATA_PATH_DRUGS = "data/drugs.txt";
static const char *DATA_PATH_REGISTRATIONS = "data/registrations.txt";
static const char *DATA_PATH_BILLS = "data/bills.txt";
static const char *DATA_PATH_USERS = "data/auth.txt";

/*
 * ============================================================================
 * 辅助函数：获取角色名称字符串
 * 功能: 将角色编号转换为可读的中文名称
 * 参数: role - 角色编号 (0=管理员, 1=医生, 2=患者)
 * 返回: 角色名称字符串
 * ============================================================================
 */
const char *get_role_string(int role) {
    switch (role) {
        case ROLE_ADMIN:   return "管理员";
        case ROLE_DOCTOR:  return "医生";
        case ROLE_PATIENT: return "患者";
        default:           return "未知";
    }
}

/*
 * ============================================================================
 * 辅助函数：统计链表节点数量
 * 功能: 遍历链表并返回节点总数
 * 参数: 
 *   - head: 链表头指针（通用指针类型）
 *   - type: 链表类型 (0=患者, 1=医生, 2=药品, 3=挂号, 4=费用, 5=用户)
 * 返回: 节点数量
 * 说明: 由于C语言没有泛型，这里使用void*和类型标识来实现通用功能
 * ============================================================================
 */
int count_list_nodes(void *head, int type) {
    int count = 0;  /* 计数器 */
    
    /* 根据不同类型遍历相应的链表 */
    switch (type) {
        case 0: { /* 患者链表 */
            PatientNode *p = (PatientNode *)head;
            while (p != NULL) {
                count++;
                p = p->next;
            }
            break;
        }
        case 1: { /* 医生链表 */
            DoctorNode *d = (DoctorNode *)head;
            while (d != NULL) {
                count++;
                d = d->next;
            }
            break;
        }
        case 2: { /* 药品链表 */
            DrugNode *dr = (DrugNode *)head;
            while (dr != NULL) {
                count++;
                dr = dr->next;
            }
            break;
        }
        case 3: { /* 挂号链表 */
            RegisterNode *r = (RegisterNode *)head;
            while (r != NULL) {
                count++;
                r = r->next;
            }
            break;
        }
        case 4: { /* 费用链表 */
            BillNode *b = (BillNode *)head;
            while (b != NULL) {
                count++;
                b = b->next;
            }
            break;
        }
        case 5: { /* 用户链表 */
            AuthNode *a = (AuthNode *)head;
            while (a != NULL) {
                count++;
                a = a->next;
            }
            break;
        }
    }
    return count;
}

/*
 * ============================================================================
 * 辅助函数：根据索引获取链表节点
 * 功能: 遍历链表获取指定索引位置的节点
 * 参数:
 *   - head: 链表头指针
 *   - index: 节点索引（从0开始）
 *   - type: 链表类型
 * 返回: 节点指针，未找到返回NULL
 * ============================================================================
 */
void *get_node_by_index(void *head, int index, int type) {
    int i = 0;
    
    switch (type) {
        case 0: { /* 患者链表 */
            PatientNode *p = (PatientNode *)head;
            while (p != NULL && i < index) {
                p = p->next;
                i++;
            }
            return p;
        }
        case 1: { /* 医生链表 */
            DoctorNode *d = (DoctorNode *)head;
            while (d != NULL && i < index) {
                d = d->next;
                i++;
            }
            return d;
        }
        case 2: { /* 药品链表 */
            DrugNode *dr = (DrugNode *)head;
            while (dr != NULL && i < index) {
                dr = dr->next;
                i++;
            }
            return dr;
        }
        case 3: { /* 挂号链表 */
            RegisterNode *r = (RegisterNode *)head;
            while (r != NULL && i < index) {
                r = r->next;
                i++;
            }
            return r;
        }
        case 4: { /* 费用链表 */
            BillNode *b = (BillNode *)head;
            while (b != NULL && i < index) {
                b = b->next;
                i++;
            }
            return b;
        }
        case 5: { /* 用户链表 */
            AuthNode *a = (AuthNode *)head;
            while (a != NULL && i < index) {
                a = a->next;
                i++;
            }
            return a;
        }
    }
    return NULL;
}

/*
 * ============================================================================
 * UI初始化函数
 * 功能: 初始化ncurses环境和加载数据
 * 返回: 0-成功, -1-失败
 * 
 * ncurses初始化说明:
 *   - initscr(): 初始化屏幕，必须首先调用
 *   - cbreak(): 禁用行缓冲，输入立即传递给程序
 *   - noecho(): 禁止自动回显输入字符
 *   - keypad(): 启用功能键（如方向键、F1-F12等）
 *   - curs_set(): 设置光标可见性
 *   - start_color(): 启用颜色支持
 * ============================================================================
 */
int ui_init(void) {
    /* 设置本地化，支持中文显示 */
    setlocale(LC_ALL, "");
    
    /* 初始化ncurses屏幕 */
    initscr();
    
    /* 检查终端是否支持颜色 */
    if (has_colors() == FALSE) {
        endwin();
        printf("您的终端不支持彩色显示\n");
        return -1;
    }
    
    /* 启用颜色功能 */
    start_color();
    
    /* 定义颜色对 (前景色, 背景色) */
    /* COLOR_PAIR_TITLE: 标题使用白色前景，蓝色背景 */
    init_pair(COLOR_PAIR_TITLE, COLOR_WHITE, COLOR_BLUE);
    /* COLOR_PAIR_MENU: 菜单使用白色前景，默认背景 */
    init_pair(COLOR_PAIR_MENU, COLOR_WHITE, COLOR_BLACK);
    /* COLOR_PAIR_SELECT: 选中项使用黑色前景，青色背景 */
    init_pair(COLOR_PAIR_SELECT, COLOR_BLACK, COLOR_CYAN);
    /* COLOR_PAIR_BORDER: 边框使用青色 */
    init_pair(COLOR_PAIR_BORDER, COLOR_CYAN, COLOR_BLACK);
    /* COLOR_PAIR_ERROR: 错误信息使用红色 */
    init_pair(COLOR_PAIR_ERROR, COLOR_RED, COLOR_BLACK);
    /* COLOR_PAIR_SUCCESS: 成功信息使用绿色 */
    init_pair(COLOR_PAIR_SUCCESS, COLOR_GREEN, COLOR_BLACK);
    /* COLOR_PAIR_WARNING: 警告信息使用黄色 */
    init_pair(COLOR_PAIR_WARNING, COLOR_YELLOW, COLOR_BLACK);
    /* COLOR_PAIR_HEADER: 表头使用黄色前景 */
    init_pair(COLOR_PAIR_HEADER, COLOR_YELLOW, COLOR_BLACK);
    
    /* 禁用行缓冲 */
    cbreak();
    /* 禁止回显 */
    noecho();
    /* 启用功能键 */
    keypad(stdscr, TRUE);
    /* 隐藏光标 */
    curs_set(0);
    
    /* 加载所有数据文件 */
    g_patients = load_patients(DATA_PATH_PATIENTS);
    g_doctors = load_doctors(DATA_PATH_DOCTORS);
    g_drugs = load_drugs(DATA_PATH_DRUGS);
    g_registrations = load_registrations(DATA_PATH_REGISTRATIONS);
    g_bills = load_bills(DATA_PATH_BILLS);
    g_users = load_users(DATA_PATH_USERS);
    
    /* 如果没有任何用户，创建默认管理员账户 */
    if (g_users == NULL) {
        AuthNode admin = make_user("admin", "admin123", ROLE_ADMIN);
        add_user(&g_users, admin);
        save_users(DATA_PATH_USERS, g_users);
    }
    
    return 0;
}

/*
 * ============================================================================
 * UI清理函数
 * 功能: 保存数据并清理ncurses环境
 * ============================================================================
 */
void ui_cleanup(void) {
    /* 保存所有数据到文件 */
    save_patients(DATA_PATH_PATIENTS, g_patients);
    save_doctors(DATA_PATH_DOCTORS, g_doctors);
    save_drugs(DATA_PATH_DRUGS, g_drugs);
    save_registrations(DATA_PATH_REGISTRATIONS, g_registrations);
    save_bills(DATA_PATH_BILLS, g_bills);
    save_users(DATA_PATH_USERS, g_users);
    
    /* 结束ncurses模式，恢复终端设置 */
    endwin();
}

/*
 * ============================================================================
 * 居中显示字符串
 * 功能: 在窗口的指定行居中显示字符串
 * 参数:
 *   - win: 目标窗口
 *   - y: 行号
 *   - str: 要显示的字符串
 * ============================================================================
 */
void ui_center_string(WINDOW *win, int y, const char *str) {
    int max_x = getmaxx(win);           /* 获取窗口宽度 */
    int len = strlen(str);               /* 计算字符串长度 */
    int x = (max_x - len) / 2;           /* 计算起始X坐标 */
    if (x < 0) x = 0;
    mvwprintw(win, y, x, "%s", str);     /* 在计算出的位置显示字符串 */
}

/*
 * ============================================================================
 * 绘制带标题的边框
 * 功能: 在窗口周围绘制边框，并在顶部中央显示标题
 * 参数:
 *   - win: 目标窗口
 *   - title: 标题字符串（可为NULL）
 * ============================================================================
 */
void ui_draw_box(WINDOW *win, const char *title) {
    /* 使用COLOR_PAIR_BORDER颜色绘制边框 */
    wattron(win, COLOR_PAIR(COLOR_PAIR_BORDER));
    box(win, 0, 0);  /* 使用默认字符绘制边框 */
    wattroff(win, COLOR_PAIR(COLOR_PAIR_BORDER));
    
    /* 如果有标题，在顶部边框中央显示 */
    if (title != NULL && strlen(title) > 0) {
        int max_x = getmaxx(win);
        int title_len = strlen(title) + 4;  /* 标题加上两边空格 */
        int title_x = (max_x - title_len) / 2;
        if (title_x < 1) title_x = 1;
        
        wattron(win, COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
        mvwprintw(win, 0, title_x, "[ %s ]", title);
        wattroff(win, COLOR_PAIR(COLOR_PAIR_TITLE) | A_BOLD);
    }
}

/*
 * ============================================================================
 * 显示消息框
 * 功能: 在屏幕中央弹出消息框，等待用户按键确认
 * 参数:
 *   - title: 消息框标题
 *   - message: 消息内容
 *   - type: 消息类型 (0=普通, 1=成功, 2=错误, 3=警告)
 * ============================================================================
 */
void ui_show_message(const char *title, const char *message, int type) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);  /* 获取屏幕尺寸 */
    
    /* 计算消息框尺寸 */
    int msg_len = strlen(message);
    int box_width = msg_len + 6;
    if (box_width < 30) box_width = 30;
    if (box_width > max_x - 4) box_width = max_x - 4;
    int box_height = 7;
    
    /* 计算消息框位置（屏幕中央） */
    int start_y = (max_y - box_height) / 2;
    int start_x = (max_x - box_width) / 2;
    
    /* 创建消息框窗口 */
    WINDOW *msg_win = newwin(box_height, box_width, start_y, start_x);
    
    /* 根据消息类型选择颜色 */
    int color_pair;
    switch (type) {
        case 1:  color_pair = COLOR_PAIR_SUCCESS; break;  /* 成功 - 绿色 */
        case 2:  color_pair = COLOR_PAIR_ERROR;   break;  /* 错误 - 红色 */
        case 3:  color_pair = COLOR_PAIR_WARNING; break;  /* 警告 - 黄色 */
        default: color_pair = COLOR_PAIR_MENU;    break;  /* 普通 - 白色 */
    }
    
    /* 绘制边框和标题 */
    ui_draw_box(msg_win, title);
    
    /* 显示消息内容 */
    wattron(msg_win, COLOR_PAIR(color_pair));
    ui_center_string(msg_win, 2, message);
    wattroff(msg_win, COLOR_PAIR(color_pair));
    
    /* 显示提示信息 */
    ui_center_string(msg_win, 4, "按任意键继续...");
    
    wrefresh(msg_win);
    
    /* 等待用户按键 */
    wgetch(msg_win);
    
    /* 销毁窗口 */
    delwin(msg_win);
    
    /* 刷新主屏幕 */
    touchwin(stdscr);
    refresh();
}

/*
 * ============================================================================
 * 确认对话框
 * 功能: 显示确认对话框，让用户选择是或否
 * 参数:
 *   - title: 对话框标题
 *   - message: 确认消息
 * 返回: 1-用户选择是, 0-用户选择否
 * ============================================================================
 */
int ui_confirm_dialog(const char *title, const char *message) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    int msg_len = strlen(message);
    int box_width = msg_len + 6;
    if (box_width < 40) box_width = 40;
    if (box_width > max_x - 4) box_width = max_x - 4;
    int box_height = 8;
    
    int start_y = (max_y - box_height) / 2;
    int start_x = (max_x - box_width) / 2;
    
    WINDOW *dialog_win = newwin(box_height, box_width, start_y, start_x);
    keypad(dialog_win, TRUE);
    
    int selected = 0;  /* 0=是, 1=否 */
    int ch;
    
    while (1) {
        /* 清空窗口 */
        werase(dialog_win);
        
        /* 绘制边框 */
        ui_draw_box(dialog_win, title);
        
        /* 显示消息 */
        wattron(dialog_win, COLOR_PAIR(COLOR_PAIR_WARNING));
        ui_center_string(dialog_win, 2, message);
        wattroff(dialog_win, COLOR_PAIR(COLOR_PAIR_WARNING));
        
        /* 显示选项按钮 */
        int btn_y = 5;
        int btn_width = 10;
        int space = 4;
        int total_width = btn_width * 2 + space;
        int start_btn_x = (box_width - total_width) / 2;
        
        /* "是" 按钮 */
        if (selected == 0) {
            wattron(dialog_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        mvwprintw(dialog_win, btn_y, start_btn_x, "  [ 是 ]  ");
        if (selected == 0) {
            wattroff(dialog_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        
        /* "否" 按钮 */
        if (selected == 1) {
            wattron(dialog_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        mvwprintw(dialog_win, btn_y, start_btn_x + btn_width + space, "  [ 否 ]  ");
        if (selected == 1) {
            wattroff(dialog_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        
        wrefresh(dialog_win);
        
        /* 处理用户输入 */
        ch = wgetch(dialog_win);
        switch (ch) {
            case KEY_LEFT:
            case 'h':
                selected = 0;
                break;
            case KEY_RIGHT:
            case 'l':
                selected = 1;
                break;
            case '\n':
            case KEY_ENTER:
                delwin(dialog_win);
                touchwin(stdscr);
                refresh();
                return (selected == 0) ? 1 : 0;
            case 27:  /* ESC键 */
                delwin(dialog_win);
                touchwin(stdscr);
                refresh();
                return 0;
        }
    }
}

/*
 * ============================================================================
 * 字符串输入函数
 * 功能: 在指定位置获取用户输入的字符串
 * 参数:
 *   - win: 目标窗口
 *   - y, x: 输入框位置
 *   - buffer: 接收输入的缓冲区
 *   - max_len: 最大输入长度
 *   - hidden: 是否隐藏输入（用于密码）
 * 返回: 输入的字符数，-1表示取消（按ESC）
 * ============================================================================
 */
int ui_input_string(WINDOW *win, int y, int x, char *buffer, int max_len, int hidden) {
    int pos = 0;           /* 当前字节位置 */
    int ch;
    
    /* 显示光标 */
    curs_set(1);
    
    /* 清空缓冲区 */
    memset(buffer, 0, max_len);
    
    /* 移动光标到输入位置 */
    wmove(win, y, x);
    wrefresh(win);
    
    while (1) {
        ch = wgetch(win);
        
        if (ch == '\n' || ch == KEY_ENTER) {
            /* 回车键确认输入 */
            break;
        } else if (ch == 27) {
            /* ESC键取消输入 */
            curs_set(0);
            return -1;
        } else if (ch == KEY_BACKSPACE || ch == 127 || ch == 8) {
            /* 退格键删除字符（支持UTF-8多字节字符） */
            if (pos > 0) {
                /* 
                 * UTF-8编码规则:
                 * - 单字节: 0xxxxxxx (ASCII)
                 * - 多字节: 首字节 11xxxxxx, 后续字节 10xxxxxx
                 * 删除时需要找到上一个字符的起始位置
                 */
                pos--;
                /* 跳过UTF-8后续字节 (10xxxxxx) */
                while (pos > 0 && (buffer[pos] & 0xC0) == 0x80) {
                    pos--;
                }
                buffer[pos] = '\0';
                
                /* 重新显示输入内容 */
                wmove(win, y, x);
                int i;
                for (i = 0; i < max_len - 1; i++) {
                    waddch(win, ' ');
                }
                wmove(win, y, x);
                if (hidden) {
                    /* 密码模式：计算实际字符数显示星号 */
                    int char_count = 0;
                    for (i = 0; i < pos; ) {
                        if ((buffer[i] & 0x80) == 0) i += 1;         /* ASCII */
                        else if ((buffer[i] & 0xE0) == 0xC0) i += 2; /* 2字节UTF-8 */
                        else if ((buffer[i] & 0xF0) == 0xE0) i += 3; /* 3字节UTF-8 (中文) */
                        else if ((buffer[i] & 0xF8) == 0xF0) i += 4; /* 4字节UTF-8 */
                        else i += 1;
                        char_count++;
                    }
                    for (i = 0; i < char_count; i++) {
                        waddch(win, '*');
                    }
                } else {
                    wprintw(win, "%s", buffer);
                }
            }
        } else if (ch >= 32 && ch <= 126) {
            /* ASCII可打印字符 */
            if (pos < max_len - 1) {
                buffer[pos] = (char)ch;
                pos++;
                buffer[pos] = '\0';
                if (hidden) {
                    waddch(win, '*');
                } else {
                    waddch(win, ch);
                }
            }
        } else if ((ch & 0x80) != 0 && pos < max_len - 4) {
            /* 
             * UTF-8多字节字符输入（支持中文）
             * UTF-8中文字符通常是3个字节: 1110xxxx 10xxxxxx 10xxxxxx
             * ncurses在某些配置下会将UTF-8字符作为多个字节返回
             */
            unsigned char first_byte = (unsigned char)ch;
            int bytes_needed = 0;
            
            /* 根据首字节确定UTF-8字符的字节数 */
            if ((first_byte & 0xE0) == 0xC0) bytes_needed = 2;      /* 110xxxxx: 2字节 */
            else if ((first_byte & 0xF0) == 0xE0) bytes_needed = 3; /* 1110xxxx: 3字节 (中文) */
            else if ((first_byte & 0xF8) == 0xF0) bytes_needed = 4; /* 11110xxx: 4字节 */
            else bytes_needed = 1;  /* 单字节或无效 */
            
            if (pos + bytes_needed < max_len) {
                buffer[pos++] = (char)ch;
                
                /* 读取后续字节 */
                int i;
                for (i = 1; i < bytes_needed; i++) {
                    int next_byte = wgetch(win);
                    if ((next_byte & 0xC0) == 0x80) {  /* 验证是后续字节 10xxxxxx */
                        buffer[pos++] = (char)next_byte;
                    } else {
                        /* 无效的UTF-8序列，回退 */
                        ungetch(next_byte);
                        break;
                    }
                }
                buffer[pos] = '\0';
                
                if (hidden) {
                    waddch(win, '*');
                } else {
                    /* 重新显示整个字符串以正确渲染UTF-8 */
                    wmove(win, y, x);
                    wprintw(win, "%s", buffer);
                }
            }
        }
        wrefresh(win);
    }
    
    /* 隐藏光标 */
    curs_set(0);
    return pos;
}

/*
 * ============================================================================
 * 整数输入函数
 * 功能: 在指定位置获取用户输入的整数
 * 参数:
 *   - win: 目标窗口
 *   - y, x: 输入框位置
 *   - value: 接收输入值的指针
 * 返回: 0-成功, -1-取消或无效输入
 * ============================================================================
 */
int ui_input_int(WINDOW *win, int y, int x, int *value) {
    char buffer[32];
    int result = ui_input_string(win, y, x, buffer, sizeof(buffer), 0);
    
    if (result < 0) {
        return -1;
    }
    
    /* 验证输入是否为有效整数 */
    char *endptr;
    long val = strtol(buffer, &endptr, 10);
    
    if (*endptr != '\0' || buffer[0] == '\0') {
        return -1;  /* 无效输入 */
    }
    
    *value = (int)val;
    return 0;
}

/*
 * ============================================================================
 * 浮点数输入函数
 * 功能: 在指定位置获取用户输入的浮点数
 * 参数:
 *   - win: 目标窗口
 *   - y, x: 输入框位置
 *   - value: 接收输入值的指针
 * 返回: 0-成功, -1-取消或无效输入
 * ============================================================================
 */
int ui_input_double(WINDOW *win, int y, int x, double *value) {
    char buffer[32];
    int result = ui_input_string(win, y, x, buffer, sizeof(buffer), 0);
    
    if (result < 0) {
        return -1;
    }
    
    /* 验证输入是否为有效浮点数 */
    char *endptr;
    double val = strtod(buffer, &endptr);
    
    if (*endptr != '\0' || buffer[0] == '\0') {
        return -1;  /* 无效输入 */
    }
    
    *value = val;
    return 0;
}

/*
 * ============================================================================
 * 登录界面
 * 功能: 显示系统登录界面，验证用户身份
 * 返回: 成功返回用户角色(0/1/2)，失败返回-1，退出返回-2
 * 
 * 界面布局:
 *   +----------------------------------+
 *   |        医院管理系统              |
 *   |                                  |
 *   |    用户名: [_______________]     |
 *   |    密  码: [_______________]     |
 *   |                                  |
 *   |    [登录]      [退出]            |
 *   +----------------------------------+
 * ============================================================================
 */
int ui_login_screen(void) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    /* 登录框尺寸 */
    int box_height = 14;
    int box_width = 50;
    int start_y = (max_y - box_height) / 2;
    int start_x = (max_x - box_width) / 2;
    
    /* 创建登录窗口 */
    WINDOW *login_win = newwin(box_height, box_width, start_y, start_x);
    keypad(login_win, TRUE);
    
    char username[MAX_NAME] = "";
    char password[MAX_NAME] = "";
    int current_field = 0;  /* 0=用户名, 1=密码, 2=登录按钮, 3=退出按钮 */
    int ch;
    int login_attempts = 0;
    
    while (1) {
        /* 清空并重绘窗口 */
        werase(login_win);
        
        /* 绘制边框和标题 */
        ui_draw_box(login_win, "医院管理系统");
        
        /* 显示欢迎信息 */
        wattron(login_win, COLOR_PAIR(COLOR_PAIR_SUCCESS) | A_BOLD);
        ui_center_string(login_win, 2, "欢迎使用医院管理系统");
        wattroff(login_win, COLOR_PAIR(COLOR_PAIR_SUCCESS) | A_BOLD);
        
        ui_center_string(login_win, 3, "请输入您的登录信息");
        
        /* 用户名标签和输入框 */
        mvwprintw(login_win, 5, 5, "用户名:");
        if (current_field == 0) {
            wattron(login_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        mvwprintw(login_win, 5, 14, "[%-20s]", username);
        if (current_field == 0) {
            wattroff(login_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        
        /* 密码标签和输入框 */
        mvwprintw(login_win, 7, 5, "密  码:");
        if (current_field == 1) {
            wattron(login_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        /* 密码显示为星号 */
        char pwd_display[22] = "";
        int i;
        for (i = 0; i < (int)strlen(password) && i < 20; i++) {
            pwd_display[i] = '*';
        }
        pwd_display[i] = '\0';
        mvwprintw(login_win, 7, 14, "[%-20s]", pwd_display);
        if (current_field == 1) {
            wattroff(login_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        
        /* 登录按钮 */
        if (current_field == 2) {
            wattron(login_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        mvwprintw(login_win, 10, 10, "  [ 登 录 ]  ");
        if (current_field == 2) {
            wattroff(login_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        
        /* 退出按钮 */
        if (current_field == 3) {
            wattron(login_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        mvwprintw(login_win, 10, 28, "  [ 退 出 ]  ");
        if (current_field == 3) {
            wattroff(login_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        
        /* 提示信息 */
        wattron(login_win, COLOR_PAIR(COLOR_PAIR_WARNING));
        ui_center_string(login_win, 12, "使用方向键移动, Enter键确认");
        wattroff(login_win, COLOR_PAIR(COLOR_PAIR_WARNING));
        
        wrefresh(login_win);
        
        ch = wgetch(login_win);
        
        switch (ch) {
            case KEY_UP:
                if (current_field > 0) current_field--;
                break;
                
            case KEY_DOWN:
                if (current_field < 3) current_field++;
                break;
                
            case KEY_LEFT:
                if (current_field >= 2) current_field = 2;
                break;
                
            case KEY_RIGHT:
                if (current_field >= 2) current_field = 3;
                break;
                
            case '\t':  /* Tab键切换字段 */
                current_field = (current_field + 1) % 4;
                break;
                
            case '\n':
            case KEY_ENTER:
                if (current_field == 0) {
                    /* 输入用户名 */
                    mvwprintw(login_win, 5, 14, "[                    ]");
                    wmove(login_win, 5, 15);
                    wrefresh(login_win);
                    ui_input_string(login_win, 5, 15, username, 20, 0);
                    current_field = 1;  /* 自动跳到密码字段 */
                } else if (current_field == 1) {
                    /* 输入密码 */
                    mvwprintw(login_win, 7, 14, "[                    ]");
                    wmove(login_win, 7, 15);
                    wrefresh(login_win);
                    ui_input_string(login_win, 7, 15, password, 20, 1);
                    current_field = 2;  /* 自动跳到登录按钮 */
                } else if (current_field == 2) {
                    /* 点击登录按钮 */
                    if (strlen(username) == 0 || strlen(password) == 0) {
                        ui_show_message("错误", "请输入用户名和密码", 2);
                    } else {
                        /* 验证用户 */
                        if (authenticate_user(g_users, username, password)) {
                            /* 登录成功 */
                            g_current_user = find_user(g_users, username);
                            delwin(login_win);
                            return g_current_user->role;
                        } else {
                            /* 登录失败 */
                            login_attempts++;
                            if (login_attempts >= 3) {
                                ui_show_message("警告", "登录失败次数过多，请稍后再试", 3);
                                login_attempts = 0;
                            } else {
                                ui_show_message("错误", "用户名或密码错误", 2);
                            }
                            /* 清空密码 */
                            memset(password, 0, sizeof(password));
                            current_field = 1;
                        }
                    }
                } else if (current_field == 3) {
                    /* 点击退出按钮 */
                    delwin(login_win);
                    return -2;
                }
                break;
                
            case 27:  /* ESC键退出 */
                delwin(login_win);
                return -2;
        }
    }
}

/*
 * ============================================================================
 * 绘制顶部标题栏
 * 功能: 在屏幕顶部显示系统名称和当前用户信息
 * ============================================================================
 */
void ui_draw_header(WINDOW *win) {
    int max_x = getmaxx(win);
    
    /* 设置标题栏背景色 */
    wbkgd(win, COLOR_PAIR(COLOR_PAIR_TITLE));
    werase(win);
    
    /* 显示系统名称（左侧） */
    wattron(win, A_BOLD);
    mvwprintw(win, 0, 2, "医院管理系统 v1.0");
    wattroff(win, A_BOLD);
    
    /* 显示当前用户信息（右侧） */
    if (g_current_user != NULL) {
        char user_info[80];
        snprintf(user_info, sizeof(user_info), "当前用户: %s (%s)", 
                 g_current_user->username, 
                 get_role_string(g_current_user->role));
        mvwprintw(win, 0, max_x - strlen(user_info) - 2, "%s", user_info);
    }
    
    wrefresh(win);
}

/*
 * ============================================================================
 * 绘制底部状态栏
 * 功能: 显示操作提示和快捷键说明
 * ============================================================================
 */
void ui_draw_status_bar(WINDOW *win, const char *message) {
    int max_x = getmaxx(win);
    
    /* 设置状态栏背景色 */
    wbkgd(win, COLOR_PAIR(COLOR_PAIR_TITLE));
    werase(win);
    
    /* 显示自定义消息或默认提示 */
    if (message != NULL && strlen(message) > 0) {
        mvwprintw(win, 0, 2, "%s", message);
    } else {
        mvwprintw(win, 0, 2, "↑↓:选择  Enter:确认  ESC:返回  Q:退出");
    }
    
    /* 显示当前时间（可选功能） */
    mvwprintw(win, 0, max_x - 20, "F1:帮助");
    
    wrefresh(win);
}

/*
 * ============================================================================
 * 绘制左侧菜单栏
 * 功能: 显示功能菜单，根据用户角色调整可用选项
 * ============================================================================
 */
void ui_draw_sidebar(WINDOW *win, int role, int selected) {
    int max_y = getmaxy(win);
    
    /* 清空并绘制边框 */
    werase(win);
    ui_draw_box(win, "功能菜单");
    
    /* 定义菜单项 */
    const char *menu_items[] = {
        "1. 患者管理",
        "2. 医生管理",
        "3. 药品管理",
        "4. 挂号管理",
        "5. 费用管理",
        "6. 用户管理",
        "7. 退出登录",
        "8. 退出系统"
    };
    
    int menu_count = 8;
    int start_y = 2;
    int i;
    
    for (i = 0; i < menu_count && start_y + i < max_y - 1; i++) {
        /* 用户管理只对管理员可见 */
        if (i == MENU_USER_MGMT && role != ROLE_ADMIN) {
            continue;
        }
        
        /* 高亮显示选中项 */
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
 * 绘制表格标题行
 * 功能: 在内容区域显示数据表格的列标题
 * ============================================================================
 */
void ui_draw_table_header(WINDOW *win, const char **headers, int col_count, const int *col_widths) {
    int x = 1;
    int i;
    
    wattron(win, COLOR_PAIR(COLOR_PAIR_HEADER) | A_BOLD | A_UNDERLINE);
    
    for (i = 0; i < col_count; i++) {
        mvwprintw(win, 1, x, "%-*s", col_widths[i], headers[i]);
        x += col_widths[i] + 1;
    }
    
    wattroff(win, COLOR_PAIR(COLOR_PAIR_HEADER) | A_BOLD | A_UNDERLINE);
}

/*
 * ============================================================================
 * 绘制操作菜单
 * 功能: 在表格下方显示可用操作选项
 * ============================================================================
 */
void ui_draw_operation_menu(WINDOW *win, int y, int selected) {
    const char *ops[] = {"[A]添加", "[D]删除", "[M]修改", "[S]查询", "[O]排序", "[B]返回"};
    int op_count = 6;
    int x = 2;
    int i;
    
    for (i = 0; i < op_count; i++) {
        if (i == selected) {
            wattron(win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        mvwprintw(win, y, x, "%s", ops[i]);
        if (i == selected) {
            wattroff(win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        x += strlen(ops[i]) + 2;
    }
}

/*
 * ============================================================================
 * 患者管理 - 绘制患者表格
 * 功能: 在内容区域显示患者信息列表，类似Excel表格
 * 参数:
 *   - win: 内容窗口
 *   - start_index: 分页起始索引
 *   - selected_row: 当前选中行（高亮显示）
 * 返回: 显示的行数
 * ============================================================================
 */
int ui_draw_patient_table(WINDOW *win, int start_index, int selected_row) {
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);
    
    /* 定义表格列标题和宽度 */
    const char *headers[] = {"序号", "姓名", "年龄", "性别", "电话", "诊断", "治疗方案"};
    int col_widths[] = {4, 10, 4, 4, 13, 15, 15};
    int col_count = 7;
    
    /* 绘制表头 */
    ui_draw_table_header(win, headers, col_count, col_widths);
    
    /* 绘制数据行 */
    PatientNode *current = g_patients;
    int index = 0;
    int display_row = 0;
    int row_y = 3;  /* 数据起始行 */
    
    /* 跳过前面的记录（分页） */
    while (current != NULL && index < start_index) {
        current = current->next;
        index++;
    }
    
    /* 显示当前页的数据 */
    while (current != NULL && row_y < max_y - 3) {
        int x = 1;
        
        /* 如果是选中行，高亮显示 */
        if (display_row == selected_row) {
            wattron(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        
        /* 序号 */
        mvwprintw(win, row_y, x, "%-*d", col_widths[0], index + 1);
        x += col_widths[0] + 1;
        
        /* 姓名 */
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[1], col_widths[1], current->name);
        x += col_widths[1] + 1;
        
        /* 年龄 */
        mvwprintw(win, row_y, x, "%-*d", col_widths[2], current->age);
        x += col_widths[2] + 1;
        
        /* 性别 */
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[3], col_widths[3], current->gender);
        x += col_widths[3] + 1;
        
        /* 电话 */
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[4], col_widths[4], current->phone);
        x += col_widths[4] + 1;
        
        /* 诊断（截断显示） */
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[5], col_widths[5], current->diagnosis);
        x += col_widths[5] + 1;
        
        /* 治疗方案（截断显示） */
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[6], col_widths[6], current->treatment);
        
        if (display_row == selected_row) {
            wattroff(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        
        current = current->next;
        index++;
        display_row++;
        row_y++;
    }
    
    return display_row;
}

/*
 * ============================================================================
 * 患者管理 - 添加患者表单
 * 功能: 弹出表单窗口，让用户输入新患者信息
 * ============================================================================
 */
int ui_add_patient_form(WINDOW *parent_win) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    /* 表单窗口尺寸 */
    int form_height = 18;
    int form_width = 60;
    int start_y = (max_y - form_height) / 2;
    int start_x = (max_x - form_width) / 2;
    
    WINDOW *form_win = newwin(form_height, form_width, start_y, start_x);
    keypad(form_win, TRUE);
    
    /* 输入缓冲区 */
    char name[MAX_NAME] = "";
    char age_str[10] = "";
    char gender[MAX_GENDER] = "";
    char phone[MAX_PHONE] = "";
    char diagnosis[MAX_DESC] = "";
    char treatment[MAX_DESC] = "";
    
    int current_field = 0;
    int field_count = 7;  /* 6个输入字段 + 1个确认按钮 */
    int ch;
    int label_x = 3;
    int input_x = 15;
    
    while (1) {
        werase(form_win);
        ui_draw_box(form_win, "添加患者");
        
        /* 绘制表单字段 */
        mvwprintw(form_win, 2, label_x, "姓    名:");
        mvwprintw(form_win, 4, label_x, "年    龄:");
        mvwprintw(form_win, 6, label_x, "性    别:");
        mvwprintw(form_win, 8, label_x, "电    话:");
        mvwprintw(form_win, 10, label_x, "诊断结果:");
        mvwprintw(form_win, 12, label_x, "治疗方案:");
        
        /* 绘制输入框 */
        int i;
        for (i = 0; i < 6; i++) {
            if (current_field == i) {
                wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            }
            switch (i) {
                case 0: mvwprintw(form_win, 2, input_x, "[%-30s]", name); break;
                case 1: mvwprintw(form_win, 4, input_x, "[%-30s]", age_str); break;
                case 2: mvwprintw(form_win, 6, input_x, "[%-30s]", gender); break;
                case 3: mvwprintw(form_win, 8, input_x, "[%-30s]", phone); break;
                case 4: mvwprintw(form_win, 10, input_x, "[%-30s]", diagnosis); break;
                case 5: mvwprintw(form_win, 12, input_x, "[%-30s]", treatment); break;
            }
            if (current_field == i) {
                wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            }
        }
        
        /* 确认按钮 */
        if (current_field == 6) {
            wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        mvwprintw(form_win, 15, 15, "  [ 确认添加 ]  ");
        if (current_field == 6) {
            wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        
        mvwprintw(form_win, 15, 35, "  [ 取消 ]  ");
        
        wattron(form_win, COLOR_PAIR(COLOR_PAIR_WARNING));
        mvwprintw(form_win, 16, 3, "提示: 使用↑↓选择字段, Enter编辑, ESC取消");
        wattroff(form_win, COLOR_PAIR(COLOR_PAIR_WARNING));
        
        wrefresh(form_win);
        
        ch = wgetch(form_win);
        
        switch (ch) {
            case KEY_UP:
                if (current_field > 0) current_field--;
                break;
            case KEY_DOWN:
            case '\t':
                if (current_field < field_count - 1) current_field++;
                break;
            case '\n':
            case KEY_ENTER:
                if (current_field < 6) {
                    /* 编辑当前字段 */
                    int row_y = 2 + current_field * 2;
                    char *target = NULL;
                    int max_len = 30;
                    
                    switch (current_field) {
                        case 0: target = name; max_len = MAX_NAME - 1; break;
                        case 1: target = age_str; max_len = 9; break;
                        case 2: target = gender; max_len = MAX_GENDER - 1; break;
                        case 3: target = phone; max_len = MAX_PHONE - 1; break;
                        case 4: target = diagnosis; max_len = 30; break;
                        case 5: target = treatment; max_len = 30; break;
                    }
                    
                    if (target != NULL) {
                        /* 清空输入区域 */
                        mvwprintw(form_win, row_y, input_x, "[%-30s]", "");
                        wrefresh(form_win);
                        ui_input_string(form_win, row_y, input_x + 1, target, max_len, 0);
                    }
                    current_field++;
                    if (current_field > 6) current_field = 6;
                } else {
                    /* 确认添加 */
                    if (strlen(name) == 0 || strlen(phone) == 0) {
                        ui_show_message("错误", "姓名和电话为必填项", 2);
                    } else {
                        int age = atoi(age_str);
                        PatientNode p = make_patient(name, age, gender, phone, diagnosis, treatment);
                        add_patient(&g_patients, p);
                        save_patients(DATA_PATH_PATIENTS, g_patients);
                        ui_show_message("成功", "患者信息添加成功", 1);
                        delwin(form_win);
                        return 0;
                    }
                }
                break;
            case 27:  /* ESC */
                delwin(form_win);
                return -1;
        }
    }
}

/*
 * ============================================================================
 * 患者管理 - 修改患者表单
 * ============================================================================
 */
int ui_modify_patient_form(WINDOW *parent_win, PatientNode *patient) {
    if (patient == NULL) return -1;
    
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    int form_height = 18;
    int form_width = 60;
    int start_y = (max_y - form_height) / 2;
    int start_x = (max_x - form_width) / 2;
    
    WINDOW *form_win = newwin(form_height, form_width, start_y, start_x);
    keypad(form_win, TRUE);
    
    /* 用现有数据初始化 */
    char name[MAX_NAME];
    char age_str[10];
    char gender[MAX_GENDER];
    char phone[MAX_PHONE];
    char diagnosis[MAX_DESC];
    char treatment[MAX_DESC];
    char old_phone[MAX_PHONE];
    
    strncpy(name, patient->name, MAX_NAME - 1);
    snprintf(age_str, sizeof(age_str), "%d", patient->age);
    strncpy(gender, patient->gender, MAX_GENDER - 1);
    strncpy(phone, patient->phone, MAX_PHONE - 1);
    strncpy(old_phone, patient->phone, MAX_PHONE - 1);
    strncpy(diagnosis, patient->diagnosis, MAX_DESC - 1);
    strncpy(treatment, patient->treatment, MAX_DESC - 1);
    
    int current_field = 0;
    int field_count = 7;
    int ch;
    int label_x = 3;
    int input_x = 15;
    
    while (1) {
        werase(form_win);
        ui_draw_box(form_win, "修改患者信息");
        
        mvwprintw(form_win, 2, label_x, "姓    名:");
        mvwprintw(form_win, 4, label_x, "年    龄:");
        mvwprintw(form_win, 6, label_x, "性    别:");
        mvwprintw(form_win, 8, label_x, "电    话:");
        mvwprintw(form_win, 10, label_x, "诊断结果:");
        mvwprintw(form_win, 12, label_x, "治疗方案:");
        
        int i;
        for (i = 0; i < 6; i++) {
            if (current_field == i) {
                wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            }
            switch (i) {
                case 0: mvwprintw(form_win, 2, input_x, "[%-30s]", name); break;
                case 1: mvwprintw(form_win, 4, input_x, "[%-30s]", age_str); break;
                case 2: mvwprintw(form_win, 6, input_x, "[%-30s]", gender); break;
                case 3: mvwprintw(form_win, 8, input_x, "[%-30s]", phone); break;
                case 4: mvwprintw(form_win, 10, input_x, "[%-30s]", diagnosis); break;
                case 5: mvwprintw(form_win, 12, input_x, "[%-30s]", treatment); break;
            }
            if (current_field == i) {
                wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            }
        }
        
        if (current_field == 6) {
            wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        mvwprintw(form_win, 15, 15, "  [ 保存修改 ]  ");
        if (current_field == 6) {
            wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        
        mvwprintw(form_win, 15, 35, "  [ 取消 ]  ");
        
        wrefresh(form_win);
        
        ch = wgetch(form_win);
        
        switch (ch) {
            case KEY_UP:
                if (current_field > 0) current_field--;
                break;
            case KEY_DOWN:
            case '\t':
                if (current_field < field_count - 1) current_field++;
                break;
            case '\n':
            case KEY_ENTER:
                if (current_field < 6) {
                    int row_y = 2 + current_field * 2;
                    char *target = NULL;
                    int max_len = 30;
                    
                    switch (current_field) {
                        case 0: target = name; max_len = MAX_NAME - 1; break;
                        case 1: target = age_str; max_len = 9; break;
                        case 2: target = gender; max_len = MAX_GENDER - 1; break;
                        case 3: target = phone; max_len = MAX_PHONE - 1; break;
                        case 4: target = diagnosis; max_len = 30; break;
                        case 5: target = treatment; max_len = 30; break;
                    }
                    
                    if (target != NULL) {
                        mvwprintw(form_win, row_y, input_x, "[%-30s]", "");
                        wrefresh(form_win);
                        ui_input_string(form_win, row_y, input_x + 1, target, max_len, 0);
                    }
                    current_field++;
                    if (current_field > 6) current_field = 6;
                } else {
                    int age = atoi(age_str);
                    PatientNode newInfo = make_patient(name, age, gender, phone, diagnosis, treatment);
                    modify_patient(g_patients, old_phone, newInfo);
                    save_patients(DATA_PATH_PATIENTS, g_patients);
                    ui_show_message("成功", "患者信息修改成功", 1);
                    delwin(form_win);
                    return 0;
                }
                break;
            case 27:
                delwin(form_win);
                return -1;
        }
    }
}

/*
 * ============================================================================
 * 患者管理 - 查询患者
 * ============================================================================
 */
void ui_search_patient(WINDOW *parent_win) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    int form_height = 12;
    int form_width = 50;
    int start_y = (max_y - form_height) / 2;
    int start_x = (max_x - form_width) / 2;
    
    WINDOW *search_win = newwin(form_height, form_width, start_y, start_x);
    keypad(search_win, TRUE);
    
    int search_type = 0;  /* 0=按姓名, 1=按电话 */
    char keyword[MAX_NAME] = "";
    int ch;
    
    while (1) {
        werase(search_win);
        ui_draw_box(search_win, "查询患者");
        
        mvwprintw(search_win, 2, 3, "查询方式:");
        if (search_type == 0) {
            wattron(search_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        mvwprintw(search_win, 2, 14, "[按姓名]");
        if (search_type == 0) {
            wattroff(search_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        
        if (search_type == 1) {
            wattron(search_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        mvwprintw(search_win, 2, 24, "[按电话]");
        if (search_type == 1) {
            wattroff(search_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        
        mvwprintw(search_win, 4, 3, "关 键 词: [%-25s]", keyword);
        mvwprintw(search_win, 6, 3, "[Enter]开始查询  [←→]切换方式  [ESC]返回");
        
        wrefresh(search_win);
        
        ch = wgetch(search_win);
        
        switch (ch) {
            case KEY_LEFT:
                search_type = 0;
                break;
            case KEY_RIGHT:
                search_type = 1;
                break;
            case '\n':
            case KEY_ENTER:
                mvwprintw(search_win, 4, 14, "[%-25s]", "");
                wrefresh(search_win);
                if (ui_input_string(search_win, 4, 15, keyword, 25, 0) >= 0) {
                    PatientNode *result = NULL;
                    if (search_type == 0) {
                        result = findPatient_name(g_patients, keyword);
                    } else {
                        result = findPatient_phone(g_patients, keyword);
                    }
                    
                    if (result != NULL) {
                        char msg[256];
                        snprintf(msg, sizeof(msg), "找到: %s, %d岁, %s, %s", 
                                 result->name, result->age, result->gender, result->phone);
                        ui_show_message("查询结果", msg, 1);
                    } else {
                        ui_show_message("查询结果", "未找到匹配的患者", 3);
                    }
                }
                break;
            case 27:
                delwin(search_win);
                return;
        }
    }
}

/*
 * ============================================================================
 * 患者管理 - 排序菜单
 * ============================================================================
 */
void ui_sort_patient_menu(WINDOW *parent_win) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    int menu_height = 10;
    int menu_width = 30;
    int start_y = (max_y - menu_height) / 2;
    int start_x = (max_x - menu_width) / 2;
    
    WINDOW *menu_win = newwin(menu_height, menu_width, start_y, start_x);
    keypad(menu_win, TRUE);
    
    int selected = 0;
    int ch;
    const char *options[] = {"按姓名排序", "按电话排序", "按年龄排序", "取消"};
    int opt_count = 4;
    
    while (1) {
        werase(menu_win);
        ui_draw_box(menu_win, "选择排序方式");
        
        int i;
        for (i = 0; i < opt_count; i++) {
            if (i == selected) {
                wattron(menu_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
            }
            mvwprintw(menu_win, 2 + i * 2, 3, "  %s  ", options[i]);
            if (i == selected) {
                wattroff(menu_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
            }
        }
        
        wrefresh(menu_win);
        
        ch = wgetch(menu_win);
        
        switch (ch) {
            case KEY_UP:
                if (selected > 0) selected--;
                break;
            case KEY_DOWN:
                if (selected < opt_count - 1) selected++;
                break;
            case '\n':
            case KEY_ENTER:
                switch (selected) {
                    case 0:
                        g_patients = sort_patients_by_name(g_patients);
                        save_patients(DATA_PATH_PATIENTS, g_patients);
                        ui_show_message("成功", "已按姓名排序", 1);
                        break;
                    case 1:
                        g_patients = sort_patients_by_phone(g_patients);
                        save_patients(DATA_PATH_PATIENTS, g_patients);
                        ui_show_message("成功", "已按电话排序", 1);
                        break;
                    case 2:
                        g_patients = sort_patients_by_age(g_patients);
                        save_patients(DATA_PATH_PATIENTS, g_patients);
                        ui_show_message("成功", "已按年龄排序", 1);
                        break;
                }
                delwin(menu_win);
                return;
            case 27:
                delwin(menu_win);
                return;
        }
    }
}

/*
 * ============================================================================
 * 患者管理界面主函数
 * 功能: 显示患者列表，支持增删改查排序操作
 * ============================================================================
 */
void ui_patient_management(WINDOW *content_win) {
    int max_y, max_x;
    getmaxyx(content_win, max_y, max_x);
    
    int selected_row = 0;
    int start_index = 0;
    int total_count = count_list_nodes(g_patients, 0);
    int page_size = max_y - 6;
    int ch;
    
    while (1) {
        werase(content_win);
        ui_draw_box(content_win, "患者管理");
        
        /* 显示统计信息 */
        mvwprintw(content_win, max_y - 2, 2, "共 %d 条记录  当前: %d/%d  翻页: PgUp/PgDn", 
                  total_count, start_index + selected_row + 1, total_count);
        
        /* 绘制表格 */
        int displayed = ui_draw_patient_table(content_win, start_index, selected_row);
        
        /* 绘制操作菜单 */
        ui_draw_operation_menu(content_win, max_y - 3, -1);
        
        wrefresh(content_win);
        
        ch = wgetch(content_win);
        
        switch (ch) {
            case KEY_UP:
            case 'k':
                if (selected_row > 0) {
                    selected_row--;
                } else if (start_index > 0) {
                    start_index--;
                }
                break;
                
            case KEY_DOWN:
            case 'j':
                if (selected_row < displayed - 1) {
                    selected_row++;
                } else if (start_index + displayed < total_count) {
                    start_index++;
                }
                break;
                
            case KEY_PPAGE:  /* Page Up */
                start_index -= page_size;
                if (start_index < 0) start_index = 0;
                selected_row = 0;
                break;
                
            case KEY_NPAGE:  /* Page Down */
                start_index += page_size;
                if (start_index >= total_count) start_index = total_count - page_size;
                if (start_index < 0) start_index = 0;
                selected_row = 0;
                break;
                
            case 'a':
            case 'A':
                ui_add_patient_form(content_win);
                total_count = count_list_nodes(g_patients, 0);
                break;
                
            case 'd':
            case 'D':
                if (total_count > 0) {
                    PatientNode *p = (PatientNode *)get_node_by_index(g_patients, start_index + selected_row, 0);
                    if (p != NULL) {
                        if (ui_confirm_dialog("确认删除", "确定要删除该患者信息吗?")) {
                            g_patients = delete_patient(g_patients, p->phone);
                            save_patients(DATA_PATH_PATIENTS, g_patients);
                            total_count = count_list_nodes(g_patients, 0);
                            if (selected_row >= total_count) selected_row = total_count - 1;
                            if (selected_row < 0) selected_row = 0;
                            ui_show_message("成功", "患者信息已删除", 1);
                        }
                    }
                }
                break;
                
            case 'm':
            case 'M':
                if (total_count > 0) {
                    PatientNode *p = (PatientNode *)get_node_by_index(g_patients, start_index + selected_row, 0);
                    if (p != NULL) {
                        ui_modify_patient_form(content_win, p);
                    }
                }
                break;
                
            case 's':
            case 'S':
                ui_search_patient(content_win);
                break;
                
            case 'o':
            case 'O':
                ui_sort_patient_menu(content_win);
                break;
                
            case 'b':
            case 'B':
            case 27:
                return;
        }
        
        total_count = count_list_nodes(g_patients, 0);
    }
}

/*
 * ============================================================================
 * 医生管理 - 绘制医生表格
 * ============================================================================
 */
int ui_draw_doctor_table(WINDOW *win, int start_index, int selected_row) {
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);
    
    const char *headers[] = {"序号", "姓名", "年龄", "性别", "科室", "电话"};
    int col_widths[] = {4, 12, 4, 4, 15, 13};
    int col_count = 6;
    
    ui_draw_table_header(win, headers, col_count, col_widths);
    
    DoctorNode *current = g_doctors;
    int index = 0;
    int display_row = 0;
    int row_y = 3;
    
    while (current != NULL && index < start_index) {
        current = current->next;
        index++;
    }
    
    while (current != NULL && row_y < max_y - 3) {
        int x = 1;
        
        if (display_row == selected_row) {
            wattron(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        
        mvwprintw(win, row_y, x, "%-*d", col_widths[0], index + 1);
        x += col_widths[0] + 1;
        
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[1], col_widths[1], current->name);
        x += col_widths[1] + 1;
        
        mvwprintw(win, row_y, x, "%-*d", col_widths[2], current->age);
        x += col_widths[2] + 1;
        
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[3], col_widths[3], current->gender);
        x += col_widths[3] + 1;
        
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[4], col_widths[4], current->department);
        x += col_widths[4] + 1;
        
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[5], col_widths[5], current->phone);
        
        if (display_row == selected_row) {
            wattroff(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        
        current = current->next;
        index++;
        display_row++;
        row_y++;
    }
    
    return display_row;
}

int ui_add_doctor_form(WINDOW *parent_win) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    int form_height = 16;
    int form_width = 60;
    int start_y = (max_y - form_height) / 2;
    int start_x = (max_x - form_width) / 2;
    
    WINDOW *form_win = newwin(form_height, form_width, start_y, start_x);
    keypad(form_win, TRUE);
    
    char name[MAX_NAME] = "";
    char age_str[10] = "";
    char gender[MAX_GENDER] = "";
    char department[MAX_DEPT] = "";
    char phone[MAX_PHONE] = "";
    
    int current_field = 0;
    int field_count = 6;
    int ch;
    int label_x = 3;
    int input_x = 15;
    
    while (1) {
        werase(form_win);
        ui_draw_box(form_win, "添加医生");
        
        mvwprintw(form_win, 2, label_x, "姓    名:");
        mvwprintw(form_win, 4, label_x, "年    龄:");
        mvwprintw(form_win, 6, label_x, "性    别:");
        mvwprintw(form_win, 8, label_x, "科    室:");
        mvwprintw(form_win, 10, label_x, "电    话:");
        
        int i;
        for (i = 0; i < 5; i++) {
            if (current_field == i) {
                wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            }
            switch (i) {
                case 0: mvwprintw(form_win, 2, input_x, "[%-30s]", name); break;
                case 1: mvwprintw(form_win, 4, input_x, "[%-30s]", age_str); break;
                case 2: mvwprintw(form_win, 6, input_x, "[%-30s]", gender); break;
                case 3: mvwprintw(form_win, 8, input_x, "[%-30s]", department); break;
                case 4: mvwprintw(form_win, 10, input_x, "[%-30s]", phone); break;
            }
            if (current_field == i) {
                wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            }
        }
        
        if (current_field == 5) {
            wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        mvwprintw(form_win, 13, 15, "  [ 确认添加 ]  ");
        if (current_field == 5) {
            wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        
        wrefresh(form_win);
        
        ch = wgetch(form_win);
        
        switch (ch) {
            case KEY_UP:
                if (current_field > 0) current_field--;
                break;
            case KEY_DOWN:
            case '\t':
                if (current_field < field_count - 1) current_field++;
                break;
            case '\n':
            case KEY_ENTER:
                if (current_field < 5) {
                    int row_y = 2 + current_field * 2;
                    char *target = NULL;
                    int max_len = 30;
                    
                    switch (current_field) {
                        case 0: target = name; max_len = MAX_NAME - 1; break;
                        case 1: target = age_str; max_len = 9; break;
                        case 2: target = gender; max_len = MAX_GENDER - 1; break;
                        case 3: target = department; max_len = MAX_DEPT - 1; break;
                        case 4: target = phone; max_len = MAX_PHONE - 1; break;
                    }
                    
                    if (target != NULL) {
                        mvwprintw(form_win, row_y, input_x, "[%-30s]", "");
                        wrefresh(form_win);
                        ui_input_string(form_win, row_y, input_x + 1, target, max_len, 0);
                    }
                    current_field++;
                } else {
                    if (strlen(name) == 0 || strlen(phone) == 0) {
                        ui_show_message("错误", "姓名和电话为必填项", 2);
                    } else {
                        int age = atoi(age_str);
                        DoctorNode d = make_doctor(name, age, gender, department, phone);
                        add_doctor(&g_doctors, d);
                        save_doctors(DATA_PATH_DOCTORS, g_doctors);
                        ui_show_message("成功", "医生信息添加成功", 1);
                        delwin(form_win);
                        return 0;
                    }
                }
                break;
            case 27:
                delwin(form_win);
                return -1;
        }
    }
}

int ui_modify_doctor_form(WINDOW *parent_win, DoctorNode *doctor) {
    if (doctor == NULL) return -1;
    
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    int form_height = 16;
    int form_width = 60;
    int start_y = (max_y - form_height) / 2;
    int start_x = (max_x - form_width) / 2;
    
    WINDOW *form_win = newwin(form_height, form_width, start_y, start_x);
    keypad(form_win, TRUE);
    
    char name[MAX_NAME];
    char age_str[10];
    char gender[MAX_GENDER];
    char department[MAX_DEPT];
    char phone[MAX_PHONE];
    char old_phone[MAX_PHONE];
    
    strncpy(name, doctor->name, MAX_NAME - 1);
    snprintf(age_str, sizeof(age_str), "%d", doctor->age);
    strncpy(gender, doctor->gender, MAX_GENDER - 1);
    strncpy(department, doctor->department, MAX_DEPT - 1);
    strncpy(phone, doctor->phone, MAX_PHONE - 1);
    strncpy(old_phone, doctor->phone, MAX_PHONE - 1);
    
    int current_field = 0;
    int field_count = 6;
    int ch;
    int label_x = 3;
    int input_x = 15;
    
    while (1) {
        werase(form_win);
        ui_draw_box(form_win, "修改医生信息");
        
        mvwprintw(form_win, 2, label_x, "姓    名:");
        mvwprintw(form_win, 4, label_x, "年    龄:");
        mvwprintw(form_win, 6, label_x, "性    别:");
        mvwprintw(form_win, 8, label_x, "科    室:");
        mvwprintw(form_win, 10, label_x, "电    话:");
        
        int i;
        for (i = 0; i < 5; i++) {
            if (current_field == i) {
                wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            }
            switch (i) {
                case 0: mvwprintw(form_win, 2, input_x, "[%-30s]", name); break;
                case 1: mvwprintw(form_win, 4, input_x, "[%-30s]", age_str); break;
                case 2: mvwprintw(form_win, 6, input_x, "[%-30s]", gender); break;
                case 3: mvwprintw(form_win, 8, input_x, "[%-30s]", department); break;
                case 4: mvwprintw(form_win, 10, input_x, "[%-30s]", phone); break;
            }
            if (current_field == i) {
                wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            }
        }
        
        if (current_field == 5) {
            wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        mvwprintw(form_win, 13, 15, "  [ 保存修改 ]  ");
        if (current_field == 5) {
            wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        
        wrefresh(form_win);
        
        ch = wgetch(form_win);
        
        switch (ch) {
            case KEY_UP:
                if (current_field > 0) current_field--;
                break;
            case KEY_DOWN:
            case '\t':
                if (current_field < field_count - 1) current_field++;
                break;
            case '\n':
            case KEY_ENTER:
                if (current_field < 5) {
                    int row_y = 2 + current_field * 2;
                    char *target = NULL;
                    int max_len = 30;
                    
                    switch (current_field) {
                        case 0: target = name; max_len = MAX_NAME - 1; break;
                        case 1: target = age_str; max_len = 9; break;
                        case 2: target = gender; max_len = MAX_GENDER - 1; break;
                        case 3: target = department; max_len = MAX_DEPT - 1; break;
                        case 4: target = phone; max_len = MAX_PHONE - 1; break;
                    }
                    
                    if (target != NULL) {
                        mvwprintw(form_win, row_y, input_x, "[%-30s]", "");
                        wrefresh(form_win);
                        ui_input_string(form_win, row_y, input_x + 1, target, max_len, 0);
                    }
                    current_field++;
                } else {
                    int age = atoi(age_str);
                    DoctorNode newInfo = make_doctor(name, age, gender, department, phone);
                    modify_doctor(g_doctors, old_phone, newInfo);
                    save_doctors(DATA_PATH_DOCTORS, g_doctors);
                    ui_show_message("成功", "医生信息修改成功", 1);
                    delwin(form_win);
                    return 0;
                }
                break;
            case 27:
                delwin(form_win);
                return -1;
        }
    }
}

void ui_search_doctor(WINDOW *parent_win) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    int form_height = 12;
    int form_width = 50;
    int start_y = (max_y - form_height) / 2;
    int start_x = (max_x - form_width) / 2;
    
    WINDOW *search_win = newwin(form_height, form_width, start_y, start_x);
    keypad(search_win, TRUE);
    
    int search_type = 0;
    char keyword[MAX_NAME] = "";
    int ch;
    
    while (1) {
        werase(search_win);
        ui_draw_box(search_win, "查询医生");
        
        mvwprintw(search_win, 2, 3, "查询方式:");
        if (search_type == 0) wattron(search_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        mvwprintw(search_win, 2, 14, "[按姓名]");
        if (search_type == 0) wattroff(search_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        
        if (search_type == 1) wattron(search_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        mvwprintw(search_win, 2, 24, "[按电话]");
        if (search_type == 1) wattroff(search_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        
        mvwprintw(search_win, 4, 3, "关 键 词: [%-25s]", keyword);
        mvwprintw(search_win, 6, 3, "[Enter]开始查询  [←→]切换方式  [ESC]返回");
        
        wrefresh(search_win);
        
        ch = wgetch(search_win);
        
        switch (ch) {
            case KEY_LEFT: search_type = 0; break;
            case KEY_RIGHT: search_type = 1; break;
            case '\n':
            case KEY_ENTER:
                mvwprintw(search_win, 4, 14, "[%-25s]", "");
                wrefresh(search_win);
                if (ui_input_string(search_win, 4, 15, keyword, 25, 0) >= 0) {
                    DoctorNode *result = NULL;
                    if (search_type == 0) {
                        result = findDoctor_name(g_doctors, keyword);
                    } else {
                        result = findDoctor_phone(g_doctors, keyword);
                    }
                    
                    if (result != NULL) {
                        char msg[256];
                        snprintf(msg, sizeof(msg), "找到: %s, %d岁, %s, %s", 
                                 result->name, result->age, result->department, result->phone);
                        ui_show_message("查询结果", msg, 1);
                    } else {
                        ui_show_message("查询结果", "未找到匹配的医生", 3);
                    }
                }
                break;
            case 27:
                delwin(search_win);
                return;
        }
    }
}

void ui_sort_doctor_menu(WINDOW *parent_win) {
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    
    int menu_height = 10;
    int menu_width = 30;
    int start_y = (max_y - menu_height) / 2;
    int start_x = (max_x - menu_width) / 2;
    
    WINDOW *menu_win = newwin(menu_height, menu_width, start_y, start_x);
    keypad(menu_win, TRUE);
    
    int selected = 0;
    int ch;
    const char *options[] = {"按姓名排序", "按电话排序", "按年龄排序", "取消"};
    int opt_count = 4;
    
    while (1) {
        werase(menu_win);
        ui_draw_box(menu_win, "选择排序方式");
        
        int i;
        for (i = 0; i < opt_count; i++) {
            if (i == selected) wattron(menu_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
            mvwprintw(menu_win, 2 + i * 2, 3, "  %s  ", options[i]);
            if (i == selected) wattroff(menu_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        
        wrefresh(menu_win);
        ch = wgetch(menu_win);
        
        switch (ch) {
            case KEY_UP: if (selected > 0) selected--; break;
            case KEY_DOWN: if (selected < opt_count - 1) selected++; break;
            case '\n':
            case KEY_ENTER:
                switch (selected) {
                    case 0:
                        g_doctors = sort_doctors_by_name(g_doctors);
                        save_doctors(DATA_PATH_DOCTORS, g_doctors);
                        ui_show_message("成功", "已按姓名排序", 1);
                        break;
                    case 1:
                        g_doctors = sort_doctors_by_phone(g_doctors);
                        save_doctors(DATA_PATH_DOCTORS, g_doctors);
                        ui_show_message("成功", "已按电话排序", 1);
                        break;
                    case 2:
                        g_doctors = sort_doctors_by_age(g_doctors);
                        save_doctors(DATA_PATH_DOCTORS, g_doctors);
                        ui_show_message("成功", "已按年龄排序", 1);
                        break;
                }
                delwin(menu_win);
                return;
            case 27:
                delwin(menu_win);
                return;
        }
    }
}

void ui_doctor_management(WINDOW *content_win) {
    int max_y, max_x;
    getmaxyx(content_win, max_y, max_x);
    
    int selected_row = 0;
    int start_index = 0;
    int total_count = count_list_nodes(g_doctors, 1);
    int page_size = max_y - 6;
    int ch;
    
    while (1) {
        werase(content_win);
        ui_draw_box(content_win, "医生管理");
        
        mvwprintw(content_win, max_y - 2, 2, "共 %d 条记录", total_count);
        
        int displayed = ui_draw_doctor_table(content_win, start_index, selected_row);
        ui_draw_operation_menu(content_win, max_y - 3, -1);
        
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
                if (selected_row < displayed - 1) selected_row++;
                else if (start_index + displayed < total_count) start_index++;
                break;
            case KEY_PPAGE:
                start_index -= page_size;
                if (start_index < 0) start_index = 0;
                selected_row = 0;
                break;
            case KEY_NPAGE:
                start_index += page_size;
                if (start_index >= total_count) start_index = total_count - page_size;
                if (start_index < 0) start_index = 0;
                selected_row = 0;
                break;
            case 'a':
            case 'A':
                ui_add_doctor_form(content_win);
                total_count = count_list_nodes(g_doctors, 1);
                break;
            case 'd':
            case 'D':
                if (total_count > 0) {
                    DoctorNode *d = (DoctorNode *)get_node_by_index(g_doctors, start_index + selected_row, 1);
                    if (d != NULL && ui_confirm_dialog("确认删除", "确定要删除该医生信息吗?")) {
                        g_doctors = delete_doctor(g_doctors, d->phone);
                        save_doctors(DATA_PATH_DOCTORS, g_doctors);
                        total_count = count_list_nodes(g_doctors, 1);
                        if (selected_row >= total_count) selected_row = total_count - 1;
                        if (selected_row < 0) selected_row = 0;
                        ui_show_message("成功", "医生信息已删除", 1);
                    }
                }
                break;
            case 'm':
            case 'M':
                if (total_count > 0) {
                    DoctorNode *d = (DoctorNode *)get_node_by_index(g_doctors, start_index + selected_row, 1);
                    if (d != NULL) ui_modify_doctor_form(content_win, d);
                }
                break;
            case 's':
            case 'S':
                ui_search_doctor(content_win);
                break;
            case 'o':
            case 'O':
                ui_sort_doctor_menu(content_win);
                break;
            case 'b':
            case 'B':
            case 27:
                return;
        }
        total_count = count_list_nodes(g_doctors, 1);
    }
}

/* ============================================================================
 * 药品管理
 * ============================================================================ */
int ui_draw_drug_table(WINDOW *win, int start_index, int selected_row) {
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);
    
    const char *headers[] = {"序号", "名称", "规格", "厂家", "价格", "库存"};
    int col_widths[] = {4, 12, 15, 12, 8, 6};
    int col_count = 6;
    
    ui_draw_table_header(win, headers, col_count, col_widths);
    
    DrugNode *current = g_drugs;
    int index = 0, display_row = 0, row_y = 3;
    
    while (current != NULL && index < start_index) { current = current->next; index++; }
    
    while (current != NULL && row_y < max_y - 3) {
        int x = 1;
        if (display_row == selected_row) wattron(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        
        mvwprintw(win, row_y, x, "%-*d", col_widths[0], index + 1); x += col_widths[0] + 1;
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[1], col_widths[1], current->name); x += col_widths[1] + 1;
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[2], col_widths[2], current->spec); x += col_widths[2] + 1;
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[3], col_widths[3], current->factory); x += col_widths[3] + 1;
        mvwprintw(win, row_y, x, "%-*.2f", col_widths[4], current->price); x += col_widths[4] + 1;
        mvwprintw(win, row_y, x, "%-*d", col_widths[5], current->stock);
        
        if (display_row == selected_row) wattroff(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        current = current->next; index++; display_row++; row_y++;
    }
    return display_row;
}

int ui_add_drug_form(WINDOW *parent_win) {
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    int form_height = 16, form_width = 60;
    int start_y = (max_y - form_height) / 2, start_x = (max_x - form_width) / 2;
    WINDOW *form_win = newwin(form_height, form_width, start_y, start_x);
    keypad(form_win, TRUE);
    
    char name[MAX_NAME] = "", spec[MAX_DEPT] = "", factory[MAX_NAME] = "";
    char price_str[20] = "", stock_str[10] = "";
    int current_field = 0, ch, label_x = 3, input_x = 15;
    
    while (1) {
        werase(form_win);
        ui_draw_box(form_win, "添加药品");
        mvwprintw(form_win, 2, label_x, "药品名称:");
        mvwprintw(form_win, 4, label_x, "规    格:");
        mvwprintw(form_win, 6, label_x, "生产厂家:");
        mvwprintw(form_win, 8, label_x, "价    格:");
        mvwprintw(form_win, 10, label_x, "库    存:");
        
        int i;
        for (i = 0; i < 5; i++) {
            if (current_field == i) wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            switch (i) {
                case 0: mvwprintw(form_win, 2, input_x, "[%-30s]", name); break;
                case 1: mvwprintw(form_win, 4, input_x, "[%-30s]", spec); break;
                case 2: mvwprintw(form_win, 6, input_x, "[%-30s]", factory); break;
                case 3: mvwprintw(form_win, 8, input_x, "[%-30s]", price_str); break;
                case 4: mvwprintw(form_win, 10, input_x, "[%-30s]", stock_str); break;
            }
            if (current_field == i) wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        
        if (current_field == 5) wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        mvwprintw(form_win, 13, 15, "  [ 确认添加 ]  ");
        if (current_field == 5) wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        
        wrefresh(form_win);
        ch = wgetch(form_win);
        
        switch (ch) {
            case KEY_UP: if (current_field > 0) current_field--; break;
            case KEY_DOWN: case '\t': if (current_field < 5) current_field++; break;
            case '\n': case KEY_ENTER:
                if (current_field < 5) {
                    int row_y = 2 + current_field * 2;
                    char *target = NULL; int max_len = 30;
                    switch (current_field) {
                        case 0: target = name; max_len = MAX_NAME - 1; break;
                        case 1: target = spec; max_len = MAX_DEPT - 1; break;
                        case 2: target = factory; max_len = MAX_NAME - 1; break;
                        case 3: target = price_str; max_len = 19; break;
                        case 4: target = stock_str; max_len = 9; break;
                    }
                    if (target) {
                        mvwprintw(form_win, row_y, input_x, "[%-30s]", "");
                        wrefresh(form_win);
                        ui_input_string(form_win, row_y, input_x + 1, target, max_len, 0);
                    }
                    current_field++;
                } else {
                    if (strlen(name) == 0) {
                        ui_show_message("错误", "药品名称为必填项", 2);
                    } else {
                        double price = atof(price_str);
                        int stock = atoi(stock_str);
                        DrugNode d = make_drug(name, spec, factory, price, stock);
                        add_drug(&g_drugs, d);
                        save_drugs(DATA_PATH_DRUGS, g_drugs);
                        ui_show_message("成功", "药品信息添加成功", 1);
                        delwin(form_win);
                        return 0;
                    }
                }
                break;
            case 27: delwin(form_win); return -1;
        }
    }
}

int ui_modify_drug_form(WINDOW *parent_win, DrugNode *drug) {
    if (drug == NULL) return -1;
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    int form_height = 16, form_width = 60;
    int start_y = (max_y - form_height) / 2, start_x = (max_x - form_width) / 2;
    WINDOW *form_win = newwin(form_height, form_width, start_y, start_x);
    keypad(form_win, TRUE);
    
    char name[MAX_NAME], spec[MAX_DEPT], factory[MAX_NAME], price_str[20], stock_str[10], old_name[MAX_NAME];
    strncpy(name, drug->name, MAX_NAME - 1);
    strncpy(old_name, drug->name, MAX_NAME - 1);
    strncpy(spec, drug->spec, MAX_DEPT - 1);
    strncpy(factory, drug->factory, MAX_NAME - 1);
    snprintf(price_str, sizeof(price_str), "%.2f", drug->price);
    snprintf(stock_str, sizeof(stock_str), "%d", drug->stock);
    
    int current_field = 0, ch, label_x = 3, input_x = 15;
    
    while (1) {
        werase(form_win);
        ui_draw_box(form_win, "修改药品信息");
        mvwprintw(form_win, 2, label_x, "药品名称:");
        mvwprintw(form_win, 4, label_x, "规    格:");
        mvwprintw(form_win, 6, label_x, "生产厂家:");
        mvwprintw(form_win, 8, label_x, "价    格:");
        mvwprintw(form_win, 10, label_x, "库    存:");
        
        int i;
        for (i = 0; i < 5; i++) {
            if (current_field == i) wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            switch (i) {
                case 0: mvwprintw(form_win, 2, input_x, "[%-30s]", name); break;
                case 1: mvwprintw(form_win, 4, input_x, "[%-30s]", spec); break;
                case 2: mvwprintw(form_win, 6, input_x, "[%-30s]", factory); break;
                case 3: mvwprintw(form_win, 8, input_x, "[%-30s]", price_str); break;
                case 4: mvwprintw(form_win, 10, input_x, "[%-30s]", stock_str); break;
            }
            if (current_field == i) wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        
        if (current_field == 5) wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        mvwprintw(form_win, 13, 15, "  [ 保存修改 ]  ");
        if (current_field == 5) wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        
        wrefresh(form_win);
        ch = wgetch(form_win);
        
        switch (ch) {
            case KEY_UP: if (current_field > 0) current_field--; break;
            case KEY_DOWN: case '\t': if (current_field < 5) current_field++; break;
            case '\n': case KEY_ENTER:
                if (current_field < 5) {
                    int row_y = 2 + current_field * 2;
                    char *target = NULL; int max_len = 30;
                    switch (current_field) {
                        case 0: target = name; max_len = MAX_NAME - 1; break;
                        case 1: target = spec; max_len = MAX_DEPT - 1; break;
                        case 2: target = factory; max_len = MAX_NAME - 1; break;
                        case 3: target = price_str; max_len = 19; break;
                        case 4: target = stock_str; max_len = 9; break;
                    }
                    if (target) {
                        mvwprintw(form_win, row_y, input_x, "[%-30s]", "");
                        wrefresh(form_win);
                        ui_input_string(form_win, row_y, input_x + 1, target, max_len, 0);
                    }
                    current_field++;
                } else {
                    double price = atof(price_str);
                    int stock = atoi(stock_str);
                    DrugNode newInfo = make_drug(name, spec, factory, price, stock);
                    modify_drug(g_drugs, old_name, newInfo);
                    save_drugs(DATA_PATH_DRUGS, g_drugs);
                    ui_show_message("成功", "药品信息修改成功", 1);
                    delwin(form_win);
                    return 0;
                }
                break;
            case 27: delwin(form_win); return -1;
        }
    }
}

void ui_search_drug(WINDOW *parent_win) {
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    int form_height = 10, form_width = 50;
    int start_y = (max_y - form_height) / 2, start_x = (max_x - form_width) / 2;
    WINDOW *search_win = newwin(form_height, form_width, start_y, start_x);
    keypad(search_win, TRUE);
    char keyword[MAX_NAME] = "";
    int ch;
    
    while (1) {
        werase(search_win);
        ui_draw_box(search_win, "查询药品");
        mvwprintw(search_win, 2, 3, "药品名称: [%-25s]", keyword);
        mvwprintw(search_win, 4, 3, "[Enter]开始查询  [ESC]返回");
        wrefresh(search_win);
        ch = wgetch(search_win);
        
        switch (ch) {
            case '\n': case KEY_ENTER:
                mvwprintw(search_win, 2, 14, "[%-25s]", "");
                wrefresh(search_win);
                if (ui_input_string(search_win, 2, 15, keyword, 25, 0) >= 0) {
                    DrugNode *result = findDrug_name(g_drugs, keyword);
                    if (result) {
                        char msg[256];
                        snprintf(msg, sizeof(msg), "找到: %s, %.2f元, 库存%d", result->name, result->price, result->stock);
                        ui_show_message("查询结果", msg, 1);
                    } else {
                        ui_show_message("查询结果", "未找到匹配的药品", 3);
                    }
                }
                break;
            case 27: delwin(search_win); return;
        }
    }
}

void ui_sort_drug_menu(WINDOW *parent_win) {
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    int menu_height = 8, menu_width = 30;
    int start_y = (max_y - menu_height) / 2, start_x = (max_x - menu_width) / 2;
    WINDOW *menu_win = newwin(menu_height, menu_width, start_y, start_x);
    keypad(menu_win, TRUE);
    int selected = 0, ch;
    const char *options[] = {"按价格排序", "按库存排序", "取消"};
    
    while (1) {
        werase(menu_win);
        ui_draw_box(menu_win, "选择排序方式");
        int i;
        for (i = 0; i < 3; i++) {
            if (i == selected) wattron(menu_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
            mvwprintw(menu_win, 2 + i * 2, 3, "  %s  ", options[i]);
            if (i == selected) wattroff(menu_win, COLOR_PAIR(COLOR_PAIR_SELECT) | A_BOLD);
        }
        wrefresh(menu_win);
        ch = wgetch(menu_win);
        
        switch (ch) {
            case KEY_UP: if (selected > 0) selected--; break;
            case KEY_DOWN: if (selected < 2) selected++; break;
            case '\n': case KEY_ENTER:
                if (selected == 0) {
                    g_drugs = sort_drugs_by_price(g_drugs);
                    save_drugs(DATA_PATH_DRUGS, g_drugs);
                    ui_show_message("成功", "已按价格排序", 1);
                } else if (selected == 1) {
                    g_drugs = sort_drugs_by_stock(g_drugs);
                    save_drugs(DATA_PATH_DRUGS, g_drugs);
                    ui_show_message("成功", "已按库存排序", 1);
                }
                delwin(menu_win); return;
            case 27: delwin(menu_win); return;
        }
    }
}

void ui_drug_management(WINDOW *content_win) {
    int max_y, max_x; getmaxyx(content_win, max_y, max_x);
    int selected_row = 0, start_index = 0;
    int total_count = count_list_nodes(g_drugs, 2), page_size = max_y - 6, ch;
    
    while (1) {
        werase(content_win);
        ui_draw_box(content_win, "药品管理");
        mvwprintw(content_win, max_y - 2, 2, "共 %d 条记录", total_count);
        int displayed = ui_draw_drug_table(content_win, start_index, selected_row);
        ui_draw_operation_menu(content_win, max_y - 3, -1);
        wrefresh(content_win);
        ch = wgetch(content_win);
        
        switch (ch) {
            case KEY_UP: case 'k':
                if (selected_row > 0) selected_row--;
                else if (start_index > 0) start_index--;
                break;
            case KEY_DOWN: case 'j':
                if (selected_row < displayed - 1) selected_row++;
                else if (start_index + displayed < total_count) start_index++;
                break;
            case KEY_PPAGE:
                start_index -= page_size; if (start_index < 0) start_index = 0;
                selected_row = 0; break;
            case KEY_NPAGE:
                start_index += page_size;
                if (start_index >= total_count) start_index = total_count - page_size;
                if (start_index < 0) start_index = 0;
                selected_row = 0; break;
            case 'a': case 'A':
                ui_add_drug_form(content_win);
                total_count = count_list_nodes(g_drugs, 2);
                break;
            case 'd': case 'D':
                if (total_count > 0) {
                    DrugNode *d = (DrugNode *)get_node_by_index(g_drugs, start_index + selected_row, 2);
                    if (d && ui_confirm_dialog("确认删除", "确定要删除该药品信息吗?")) {
                        g_drugs = delete_drug(g_drugs, d->name);
                        save_drugs(DATA_PATH_DRUGS, g_drugs);
                        total_count = count_list_nodes(g_drugs, 2);
                        if (selected_row >= total_count) selected_row = total_count - 1;
                        if (selected_row < 0) selected_row = 0;
                        ui_show_message("成功", "药品信息已删除", 1);
                    }
                }
                break;
            case 'm': case 'M':
                if (total_count > 0) {
                    DrugNode *d = (DrugNode *)get_node_by_index(g_drugs, start_index + selected_row, 2);
                    if (d) ui_modify_drug_form(content_win, d);
                }
                break;
            case 's': case 'S': ui_search_drug(content_win); break;
            case 'o': case 'O': ui_sort_drug_menu(content_win); break;
            case 'b': case 'B': case 27: return;
        }
        total_count = count_list_nodes(g_drugs, 2);
    }
}

/* ============================================================================
 * 挂号管理
 * ============================================================================ */
int ui_draw_registration_table(WINDOW *win, int start_index, int selected_row) {
    int max_y, max_x; getmaxyx(win, max_y, max_x);
    const char *headers[] = {"序号", "患者", "医生", "科室", "日期"};
    int col_widths[] = {4, 12, 12, 15, 12};
    ui_draw_table_header(win, headers, 5, col_widths);
    
    RegisterNode *current = g_registrations;
    int index = 0, display_row = 0, row_y = 3;
    while (current && index < start_index) { current = current->next; index++; }
    
    while (current && row_y < max_y - 3) {
        int x = 1;
        if (display_row == selected_row) wattron(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        mvwprintw(win, row_y, x, "%-*d", col_widths[0], index + 1); x += col_widths[0] + 1;
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[1], col_widths[1], current->patientName); x += col_widths[1] + 1;
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[2], col_widths[2], current->doctorName); x += col_widths[2] + 1;
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[3], col_widths[3], current->department); x += col_widths[3] + 1;
        mvwprintw(win, row_y, x, "%-*.*s", col_widths[4], col_widths[4], current->date);
        if (display_row == selected_row) wattroff(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        current = current->next; index++; display_row++; row_y++;
    }
    return display_row;
}

int ui_add_registration_form(WINDOW *parent_win) {
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    WINDOW *form_win = newwin(14, 60, (max_y-14)/2, (max_x-60)/2);
    keypad(form_win, TRUE);
    char patientName[MAX_NAME]="", doctorName[MAX_NAME]="", department[MAX_DEPT]="", date[MAX_NAME]="";
    int current_field = 0, ch;
    
    while (1) {
        werase(form_win); ui_draw_box(form_win, "添加挂号");
        mvwprintw(form_win, 2, 3, "患者姓名:"); mvwprintw(form_win, 4, 3, "医生姓名:");
        mvwprintw(form_win, 6, 3, "科    室:"); mvwprintw(form_win, 8, 3, "日    期:");
        
        int i; for (i = 0; i < 4; i++) {
            if (current_field == i) wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
            switch(i) {
                case 0: mvwprintw(form_win, 2, 14, "[%-30s]", patientName); break;
                case 1: mvwprintw(form_win, 4, 14, "[%-30s]", doctorName); break;
                case 2: mvwprintw(form_win, 6, 14, "[%-30s]", department); break;
                case 3: mvwprintw(form_win, 8, 14, "[%-30s]", date); break;
            }
            if (current_field == i) wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT));
        }
        if (current_field == 4) wattron(form_win, COLOR_PAIR(COLOR_PAIR_SELECT)|A_BOLD);
        mvwprintw(form_win, 11, 15, "  [ 确认添加 ]  ");
        if (current_field == 4) wattroff(form_win, COLOR_PAIR(COLOR_PAIR_SELECT)|A_BOLD);
        wrefresh(form_win);
        ch = wgetch(form_win);
        
        switch(ch) {
            case KEY_UP: if (current_field > 0) current_field--; break;
            case KEY_DOWN: case '\t': if (current_field < 4) current_field++; break;
            case '\n': case KEY_ENTER:
                if (current_field < 4) {
                    char *t = NULL; int row_y = 2 + current_field*2;
                    switch(current_field) { case 0: t=patientName; break; case 1: t=doctorName; break; case 2: t=department; break; case 3: t=date; break; }
                    if (t) { mvwprintw(form_win, row_y, 14, "[%-30s]", ""); wrefresh(form_win); ui_input_string(form_win, row_y, 15, t, 30, 0); }
                    current_field++;
                } else {
                    if (strlen(patientName)==0 || strlen(doctorName)==0) { ui_show_message("错误", "患者和医生姓名必填", 2); }
                    else { RegisterNode r = make_registration(patientName, doctorName, department, date); add_registration(&g_registrations, r); save_registrations(DATA_PATH_REGISTRATIONS, g_registrations); ui_show_message("成功", "挂号成功", 1); delwin(form_win); return 0; }
                }
                break;
            case 27: delwin(form_win); return -1;
        }
    }
}

void ui_search_registration(WINDOW *parent_win) {
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    WINDOW *w = newwin(10, 50, (max_y-10)/2, (max_x-50)/2); keypad(w, TRUE);
    char kw[MAX_NAME]=""; int ch;
    while (1) {
        werase(w); ui_draw_box(w, "查询挂号");
        mvwprintw(w, 2, 3, "患者姓名: [%-25s]", kw);
        mvwprintw(w, 4, 3, "[Enter]查询  [ESC]返回");
        wrefresh(w);
        ch = wgetch(w);
        if (ch == '\n' || ch == KEY_ENTER) {
            mvwprintw(w, 2, 14, "[%-25s]", ""); wrefresh(w);
            if (ui_input_string(w, 2, 15, kw, 25, 0) >= 0) {
                RegisterNode *r = findRegistration_patient(g_registrations, kw);
                if (r) { char msg[256]; snprintf(msg, sizeof(msg), "找到: %.20s -> %.20s, %.15s", r->patientName, r->doctorName, r->department); ui_show_message("结果", msg, 1); }
                else ui_show_message("结果", "未找到", 3);
            }
        } else if (ch == 27) { delwin(w); return; }
    }
}

void ui_registration_management(WINDOW *content_win) {
    int max_y, max_x; getmaxyx(content_win, max_y, max_x);
    int sel=0, start=0, total=count_list_nodes(g_registrations, 3), ch;
    while (1) {
        werase(content_win); ui_draw_box(content_win, "挂号管理");
        mvwprintw(content_win, max_y-2, 2, "共 %d 条", total);
        int disp = ui_draw_registration_table(content_win, start, sel);
        mvwprintw(content_win, max_y-3, 2, "[A]添加 [D]删除 [S]查询 [B]返回");
        wrefresh(content_win);
        ch = wgetch(content_win);
        switch(ch) {
            case KEY_UP: case 'k': if (sel>0) sel--; else if (start>0) start--; break;
            case KEY_DOWN: case 'j': if (sel<disp-1) sel++; else if (start+disp<total) start++; break;
            case 'a': case 'A': ui_add_registration_form(content_win); total=count_list_nodes(g_registrations, 3); break;
            case 'd': case 'D':
                if (total>0) { RegisterNode *r=(RegisterNode*)get_node_by_index(g_registrations, start+sel, 3);
                if (r && ui_confirm_dialog("确认", "删除此挂号?")) { g_registrations=delete_registration(g_registrations, r->patientName, r->date); save_registrations(DATA_PATH_REGISTRATIONS, g_registrations); total=count_list_nodes(g_registrations, 3); if (sel>=total) sel=total-1; if (sel<0) sel=0; } }
                break;
            case 's': case 'S': ui_search_registration(content_win); break;
            case 'b': case 'B': case 27: return;
        }
    }
}

/* ============================================================================
 * 费用管理
 * ============================================================================ */
int ui_draw_bill_table(WINDOW *win, int start_index, int selected_row) {
    int max_y, max_x; getmaxyx(win, max_y, max_x);
    const char *headers[] = {"序号", "患者", "项目", "金额"};
    int col_widths[] = {4, 15, 20, 10};
    ui_draw_table_header(win, headers, 4, col_widths);
    BillNode *cur = g_bills; int idx=0, drow=0, ry=3;
    while (cur && idx < start_index) { cur=cur->next; idx++; }
    while (cur && ry < max_y-3) {
        int x=1; if (drow==selected_row) wattron(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        mvwprintw(win, ry, x, "%-*d", col_widths[0], idx+1); x+=col_widths[0]+1;
        mvwprintw(win, ry, x, "%-*.*s", col_widths[1], col_widths[1], cur->patientName); x+=col_widths[1]+1;
        mvwprintw(win, ry, x, "%-*.*s", col_widths[2], col_widths[2], cur->itemName); x+=col_widths[2]+1;
        mvwprintw(win, ry, x, "%-*.2f", col_widths[3], cur->amount);
        if (drow==selected_row) wattroff(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        cur=cur->next; idx++; drow++; ry++;
    }
    return drow;
}

int ui_add_bill_form(WINDOW *parent_win) {
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    WINDOW *w = newwin(12, 60, (max_y-12)/2, (max_x-60)/2); keypad(w, TRUE);
    char pn[MAX_NAME]="", item[MAX_NAME]="", amt[20]="";
    int cf=0, ch;
    while (1) {
        werase(w); ui_draw_box(w, "添加费用");
        mvwprintw(w, 2, 3, "患者姓名:"); mvwprintw(w, 4, 3, "收费项目:"); mvwprintw(w, 6, 3, "金    额:");
        int i; for (i=0;i<3;i++) { if (cf==i) wattron(w, COLOR_PAIR(COLOR_PAIR_SELECT));
            switch(i) { case 0: mvwprintw(w,2,14,"[%-30s]",pn); break; case 1: mvwprintw(w,4,14,"[%-30s]",item); break; case 2: mvwprintw(w,6,14,"[%-30s]",amt); break; }
            if (cf==i) wattroff(w, COLOR_PAIR(COLOR_PAIR_SELECT)); }
        if (cf==3) wattron(w, COLOR_PAIR(COLOR_PAIR_SELECT)|A_BOLD);
        mvwprintw(w, 9, 15, "  [ 确认 ]  ");
        if (cf==3) wattroff(w, COLOR_PAIR(COLOR_PAIR_SELECT)|A_BOLD);
        wrefresh(w); ch = wgetch(w);
        switch(ch) {
            case KEY_UP: if (cf>0) cf--; break;
            case KEY_DOWN: case '\t': if (cf<3) cf++; break;
            case '\n': case KEY_ENTER:
                if (cf<3) { char *t=NULL; int ry=2+cf*2;
                    switch(cf) { case 0: t=pn; break; case 1: t=item; break; case 2: t=amt; break; }
                    if (t) { mvwprintw(w, ry, 14, "[%-30s]", ""); wrefresh(w); ui_input_string(w, ry, 15, t, 30, 0); }
                    cf++;
                } else {
                    if (strlen(pn)==0) ui_show_message("错误", "患者姓名必填", 2);
                    else { double a=atof(amt); BillNode b=make_bill(pn, item, a); add_bill(&g_bills, b); save_bills(DATA_PATH_BILLS, g_bills); ui_show_message("成功", "费用添加成功", 1); delwin(w); return 0; }
                }
                break;
            case 27: delwin(w); return -1;
        }
    }
}

int ui_modify_bill_form(WINDOW *parent_win, BillNode *bill) {
    if (!bill) return -1;
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    WINDOW *w = newwin(12, 60, (max_y-12)/2, (max_x-60)/2); keypad(w, TRUE);
    char pn[MAX_NAME], item[MAX_NAME], amt[20], old_pn[MAX_NAME], old_item[MAX_NAME];
    strncpy(pn, bill->patientName, MAX_NAME-1); strncpy(old_pn, bill->patientName, MAX_NAME-1);
    strncpy(item, bill->itemName, MAX_NAME-1); strncpy(old_item, bill->itemName, MAX_NAME-1);
    snprintf(amt, sizeof(amt), "%.2f", bill->amount);
    int cf=0, ch;
    while (1) {
        werase(w); ui_draw_box(w, "修改费用");
        mvwprintw(w, 2, 3, "患者姓名:"); mvwprintw(w, 4, 3, "收费项目:"); mvwprintw(w, 6, 3, "金    额:");
        int i; for (i=0;i<3;i++) { if (cf==i) wattron(w, COLOR_PAIR(COLOR_PAIR_SELECT));
            switch(i) { case 0: mvwprintw(w,2,14,"[%-30s]",pn); break; case 1: mvwprintw(w,4,14,"[%-30s]",item); break; case 2: mvwprintw(w,6,14,"[%-30s]",amt); break; }
            if (cf==i) wattroff(w, COLOR_PAIR(COLOR_PAIR_SELECT)); }
        if (cf==3) wattron(w, COLOR_PAIR(COLOR_PAIR_SELECT)|A_BOLD);
        mvwprintw(w, 9, 15, "  [ 保存 ]  ");
        if (cf==3) wattroff(w, COLOR_PAIR(COLOR_PAIR_SELECT)|A_BOLD);
        wrefresh(w); ch = wgetch(w);
        switch(ch) {
            case KEY_UP: if (cf>0) cf--; break;
            case KEY_DOWN: case '\t': if (cf<3) cf++; break;
            case '\n': case KEY_ENTER:
                if (cf<3) { char *t=NULL; int ry=2+cf*2;
                    switch(cf) { case 0: t=pn; break; case 1: t=item; break; case 2: t=amt; break; }
                    if (t) { mvwprintw(w, ry, 14, "[%-30s]", ""); wrefresh(w); ui_input_string(w, ry, 15, t, 30, 0); }
                    cf++;
                } else {
                    double a=atof(amt); BillNode nb=make_bill(pn, item, a);
                    modify_bill(g_bills, old_pn, old_item, nb);
                    save_bills(DATA_PATH_BILLS, g_bills); ui_show_message("成功", "费用修改成功", 1); delwin(w); return 0;
                }
                break;
            case 27: delwin(w); return -1;
        }
    }
}

void ui_search_bill(WINDOW *parent_win) {
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    WINDOW *w = newwin(10, 50, (max_y-10)/2, (max_x-50)/2); keypad(w, TRUE);
    char kw[MAX_NAME]=""; int ch;
    while (1) {
        werase(w); ui_draw_box(w, "查询费用");
        mvwprintw(w, 2, 3, "患者姓名: [%-25s]", kw);
        mvwprintw(w, 4, 3, "[Enter]查询  [ESC]返回");
        wrefresh(w); ch = wgetch(w);
        if (ch=='\n' || ch==KEY_ENTER) {
            mvwprintw(w, 2, 14, "[%-25s]", ""); wrefresh(w);
            if (ui_input_string(w, 2, 15, kw, 25, 0) >= 0) {
                double total = calculate_total_bill(g_bills, kw);
                char msg[100]; snprintf(msg, sizeof(msg), "%s 总费用: %.2f元", kw, total);
                ui_show_message("结果", msg, 1);
            }
        } else if (ch==27) { delwin(w); return; }
    }
}

void ui_sort_bill_menu(WINDOW *parent_win) {
    g_bills = sort_bills_by_amount(g_bills);
    save_bills(DATA_PATH_BILLS, g_bills);
    ui_show_message("成功", "已按金额排序", 1);
}

void ui_bill_management(WINDOW *content_win) {
    int max_y, max_x; getmaxyx(content_win, max_y, max_x);
    int sel=0, start=0, total=count_list_nodes(g_bills, 4), ch;
    while (1) {
        werase(content_win); ui_draw_box(content_win, "费用管理");
        mvwprintw(content_win, max_y-2, 2, "共 %d 条", total);
        int disp = ui_draw_bill_table(content_win, start, sel);
        ui_draw_operation_menu(content_win, max_y-3, -1);
        wrefresh(content_win); ch = wgetch(content_win);
        switch(ch) {
            case KEY_UP: case 'k': if (sel>0) sel--; else if (start>0) start--; break;
            case KEY_DOWN: case 'j': if (sel<disp-1) sel++; else if (start+disp<total) start++; break;
            case 'a': case 'A': ui_add_bill_form(content_win); total=count_list_nodes(g_bills, 4); break;
            case 'd': case 'D':
                if (total>0) { BillNode *b=(BillNode*)get_node_by_index(g_bills, start+sel, 4);
                if (b && ui_confirm_dialog("确认", "删除此费用?")) { g_bills=delete_bill(g_bills, b->patientName, b->itemName); save_bills(DATA_PATH_BILLS, g_bills); total=count_list_nodes(g_bills, 4); if (sel>=total) sel=total-1; if (sel<0) sel=0; } }
                break;
            case 'm': case 'M': if (total>0) { BillNode *b=(BillNode*)get_node_by_index(g_bills, start+sel, 4); if (b) ui_modify_bill_form(content_win, b); } break;
            case 's': case 'S': ui_search_bill(content_win); break;
            case 'o': case 'O': ui_sort_bill_menu(content_win); break;
            case 'b': case 'B': case 27: return;
        }
        total=count_list_nodes(g_bills, 4);
    }
}

/* ============================================================================
 * 用户管理（仅管理员）
 * ============================================================================ */
int ui_draw_user_table(WINDOW *win, int start_index, int selected_row) {
    int max_y, max_x; getmaxyx(win, max_y, max_x);
    const char *headers[] = {"序号", "用户名", "角色"};
    int col_widths[] = {4, 20, 15};
    ui_draw_table_header(win, headers, 3, col_widths);
    AuthNode *cur = g_users; int idx=0, drow=0, ry=3;
    while (cur && idx < start_index) { cur=cur->next; idx++; }
    while (cur && ry < max_y-3) {
        int x=1; if (drow==selected_row) wattron(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        mvwprintw(win, ry, x, "%-*d", col_widths[0], idx+1); x+=col_widths[0]+1;
        mvwprintw(win, ry, x, "%-*.*s", col_widths[1], col_widths[1], cur->username); x+=col_widths[1]+1;
        mvwprintw(win, ry, x, "%-*s", col_widths[2], get_role_string(cur->role));
        if (drow==selected_row) wattroff(win, COLOR_PAIR(COLOR_PAIR_SELECT));
        cur=cur->next; idx++; drow++; ry++;
    }
    return drow;
}

int ui_add_user_form(WINDOW *parent_win) {
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    WINDOW *w = newwin(14, 60, (max_y-14)/2, (max_x-60)/2); keypad(w, TRUE);
    char un[MAX_NAME]="", pw[MAX_NAME]=""; int role=2, cf=0, ch;
    while (1) {
        werase(w); ui_draw_box(w, "添加用户");
        mvwprintw(w, 2, 3, "用户名:"); mvwprintw(w, 4, 3, "密  码:"); mvwprintw(w, 6, 3, "角  色:");
        if (cf==0) wattron(w, COLOR_PAIR(COLOR_PAIR_SELECT)); mvwprintw(w, 2, 12, "[%-30s]", un); if (cf==0) wattroff(w, COLOR_PAIR(COLOR_PAIR_SELECT));
        if (cf==1) wattron(w, COLOR_PAIR(COLOR_PAIR_SELECT)); char pd[32]=""; int i; for(i=0;i<(int)strlen(pw)&&i<30;i++) pd[i]='*'; mvwprintw(w, 4, 12, "[%-30s]", pd); if (cf==1) wattroff(w, COLOR_PAIR(COLOR_PAIR_SELECT));
        if (cf==2) wattron(w, COLOR_PAIR(COLOR_PAIR_SELECT)); mvwprintw(w, 6, 12, "[%s]", get_role_string(role)); if (cf==2) wattroff(w, COLOR_PAIR(COLOR_PAIR_SELECT));
        mvwprintw(w, 8, 3, "(角色: 0=管理员, 1=医生, 2=患者)");
        if (cf==3) wattron(w, COLOR_PAIR(COLOR_PAIR_SELECT)|A_BOLD); mvwprintw(w, 11, 15, "  [ 确认 ]  "); if (cf==3) wattroff(w, COLOR_PAIR(COLOR_PAIR_SELECT)|A_BOLD);
        wrefresh(w); ch = wgetch(w);
        switch(ch) {
            case KEY_UP: if (cf>0) cf--; break;
            case KEY_DOWN: case '\t': if (cf<3) cf++; break;
            case '\n': case KEY_ENTER:
                if (cf==0) { mvwprintw(w, 2, 12, "[%-30s]", ""); wrefresh(w); ui_input_string(w, 2, 13, un, 30, 0); cf++; }
                else if (cf==1) { mvwprintw(w, 4, 12, "[%-30s]", ""); wrefresh(w); ui_input_string(w, 4, 13, pw, 30, 1); cf++; }
                else if (cf==2) { role = (role + 1) % 3; }
                else {
                    if (strlen(un)==0 || strlen(pw)==0) ui_show_message("错误", "用户名密码必填", 2);
                    else if (find_user(g_users, un)) ui_show_message("错误", "用户已存在", 2);
                    else { AuthNode a = make_user(un, pw, role); add_user(&g_users, a); save_users(DATA_PATH_USERS, g_users); ui_show_message("成功", "用户添加成功", 1); delwin(w); return 0; }
                }
                break;
            case 27: delwin(w); return -1;
        }
    }
}

int ui_modify_user_form(WINDOW *parent_win, AuthNode *user) {
    if (!user) return -1;
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    WINDOW *w = newwin(12, 60, (max_y-12)/2, (max_x-60)/2); keypad(w, TRUE);
    char pw[MAX_NAME]=""; int cf=0, ch;
    while (1) {
        werase(w); ui_draw_box(w, "修改密码");
        mvwprintw(w, 2, 3, "用户名: %s", user->username);
        mvwprintw(w, 4, 3, "新密码:");
        if (cf==0) wattron(w, COLOR_PAIR(COLOR_PAIR_SELECT)); char pd[32]=""; int i; for(i=0;i<(int)strlen(pw)&&i<30;i++) pd[i]='*'; mvwprintw(w, 4, 12, "[%-30s]", pd); if (cf==0) wattroff(w, COLOR_PAIR(COLOR_PAIR_SELECT));
        if (cf==1) wattron(w, COLOR_PAIR(COLOR_PAIR_SELECT)|A_BOLD); mvwprintw(w, 8, 15, "  [ 保存 ]  "); if (cf==1) wattroff(w, COLOR_PAIR(COLOR_PAIR_SELECT)|A_BOLD);
        wrefresh(w); ch = wgetch(w);
        switch(ch) {
            case KEY_UP: if (cf>0) cf--; break;
            case KEY_DOWN: case '\t': if (cf<1) cf++; break;
            case '\n': case KEY_ENTER:
                if (cf==0) { mvwprintw(w, 4, 12, "[%-30s]", ""); wrefresh(w); ui_input_string(w, 4, 13, pw, 30, 1); cf++; }
                else { if (strlen(pw)==0) ui_show_message("错误", "密码不能为空", 2);
                    else { AuthNode ni = make_user(user->username, pw, user->role); modify_user(g_users, user->username, ni); save_users(DATA_PATH_USERS, g_users); ui_show_message("成功", "密码修改成功", 1); delwin(w); return 0; } }
                break;
            case 27: delwin(w); return -1;
        }
    }
}

void ui_user_management(WINDOW *content_win) {
    int max_y, max_x; getmaxyx(content_win, max_y, max_x);
    int sel=0, start=0, total=count_list_nodes(g_users, 5), ch;
    while (1) {
        werase(content_win); ui_draw_box(content_win, "用户管理");
        mvwprintw(content_win, max_y-2, 2, "共 %d 条", total);
        int disp = ui_draw_user_table(content_win, start, sel);
        mvwprintw(content_win, max_y-3, 2, "[A]添加 [D]删除 [M]修改密码 [B]返回");
        wrefresh(content_win); ch = wgetch(content_win);
        switch(ch) {
            case KEY_UP: case 'k': if (sel>0) sel--; else if (start>0) start--; break;
            case KEY_DOWN: case 'j': if (sel<disp-1) sel++; else if (start+disp<total) start++; break;
            case 'a': case 'A': ui_add_user_form(content_win); total=count_list_nodes(g_users, 5); break;
            case 'd': case 'D':
                if (total>0) { AuthNode *u=(AuthNode*)get_node_by_index(g_users, start+sel, 5);
                if (u && strcmp(u->username, "admin")!=0 && ui_confirm_dialog("确认", "删除此用户?")) { g_users=delete_user(g_users, u->username); save_users(DATA_PATH_USERS, g_users); total=count_list_nodes(g_users, 5); if (sel>=total) sel=total-1; if (sel<0) sel=0; }
                else if (u && strcmp(u->username, "admin")==0) ui_show_message("错误", "不能删除admin", 2); }
                break;
            case 'm': case 'M': if (total>0) { AuthNode *u=(AuthNode*)get_node_by_index(g_users, start+sel, 5); if (u) ui_modify_user_form(content_win, u); } break;
            case 'b': case 'B': case 27: return;
        }
        total=count_list_nodes(g_users, 5);
    }
}

/* ============================================================================
 * 主界面和入口函数
 * ============================================================================ */
void ui_main_screen(int role) {
    int max_y, max_x; getmaxyx(stdscr, max_y, max_x);
    
    WINDOW *header_win = newwin(1, max_x, 0, 0);
    WINDOW *sidebar_win = newwin(max_y - 2, SIDEBAR_WIDTH, 1, 0);
    WINDOW *content_win = newwin(max_y - 2, max_x - SIDEBAR_WIDTH, 1, SIDEBAR_WIDTH);
    WINDOW *status_win = newwin(1, max_x, max_y - 1, 0);
    
    keypad(sidebar_win, TRUE); keypad(content_win, TRUE);
    
    int selected = 0, ch, running = 1;
    int menu_count = (role == ROLE_ADMIN) ? 8 : 7;
    
    while (running) {
        ui_draw_header(header_win);
        ui_draw_sidebar(sidebar_win, role, selected);
        ui_draw_status_bar(status_win, NULL);
        
        werase(content_win);
        ui_draw_box(content_win, "欢迎使用医院管理系统");
        mvwprintw(content_win, 3, 3, "请从左侧菜单选择功能");
        mvwprintw(content_win, 5, 3, "使用 ↑↓ 键选择菜单项");
        mvwprintw(content_win, 6, 3, "按 Enter 进入选中的功能");
        wrefresh(content_win);
        
        ch = wgetch(sidebar_win);
        
        switch(ch) {
            case KEY_UP: case 'k': if (selected > 0) selected--; break;
            case KEY_DOWN: case 'j': if (selected < menu_count - 1) selected++; break;
            case '\n': case KEY_ENTER:
                if (role != ROLE_ADMIN && selected >= MENU_USER_MGMT) selected++;
                switch(selected) {
                    case MENU_PATIENT_MGMT: ui_patient_management(content_win); break;
                    case MENU_DOCTOR_MGMT: ui_doctor_management(content_win); break;
                    case MENU_DRUG_MGMT: ui_drug_management(content_win); break;
                    case MENU_REGISTER_MGMT: ui_registration_management(content_win); break;
                    case MENU_BILL_MGMT: ui_bill_management(content_win); break;
                    case MENU_USER_MGMT: if (role == ROLE_ADMIN) ui_user_management(content_win); break;
                    case MENU_LOGOUT: running = 0; break;
                    case MENU_EXIT: delwin(header_win); delwin(sidebar_win); delwin(content_win); delwin(status_win); return;
                }
                if (role != ROLE_ADMIN && selected > MENU_USER_MGMT) selected--;
                break;
            case 'q': case 'Q': running = 0; break;
        }
    }
    
    delwin(header_win); delwin(sidebar_win); delwin(content_win); delwin(status_win);
}

int ui_main(void) {
    if (ui_init() != 0) { printf("UI初始化失败\n"); return -1; }
    
    int running = 1;
    while (running) {
        clear(); refresh();
        int role = ui_login_screen();
        if (role == -2) { running = 0; }
        else if (role >= 0) { ui_main_screen(role); g_current_user = NULL; }
    }
    
    ui_cleanup();
    return 0;
}
