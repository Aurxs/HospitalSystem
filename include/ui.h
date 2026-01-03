/**
 * ============================================================================
 * 文件名称: ui.h
 * 作者: 罗金源
 * 创建日期: 2025/12/19
 * 修改日期: 2026/01/03
 * 描述: 医院管理系统用户界面头文件
 *       本文件定义了系统的所有用户界面相关函数
 *       使用 ncurses 库实现终端图形界面
 * ============================================================================
 */

#ifndef HOSPITALSYSTEM_UI_H
#define HOSPITALSYSTEM_UI_H

/*
 * ============================================================================
 * 引入依赖头文件
 * ============================================================================
 */
#include "datastruct.h"     /* 数据结构定义（患者、医生、药品等节点结构） */
#include <ncurses.h>        /* ncurses库，用于终端图形界面 */

/*
 * ============================================================================
 * 常量定义
 * 说明: 这些常量用于控制界面布局和样式
 * ============================================================================
 */

/* 颜色对定义 - 用于ncurses颜色配置 */
#define COLOR_PAIR_TITLE     1    /* 标题颜色对（通常为白底蓝字或类似高亮） */
#define COLOR_PAIR_MENU      2    /* 菜单项颜色对 */
#define COLOR_PAIR_SELECT    3    /* 选中项颜色对（高亮显示） */
#define COLOR_PAIR_BORDER    4    /* 边框颜色对 */
#define COLOR_PAIR_ERROR     5    /* 错误信息颜色对（通常为红色） */
#define COLOR_PAIR_SUCCESS   6    /* 成功信息颜色对（通常为绿色） */
#define COLOR_PAIR_WARNING   7    /* 警告信息颜色对（通常为黄色） */
#define COLOR_PAIR_HEADER    8    /* 表头颜色对 */

/* 界面布局常量 */
#define SIDEBAR_WIDTH        20   /* 左侧菜单栏宽度 */
#define MIN_CONTENT_WIDTH    60   /* 内容区域最小宽度 */
#define MIN_SCREEN_HEIGHT    24   /* 屏幕最小高度 */
#define MIN_SCREEN_WIDTH     80   /* 屏幕最小宽度 */
#define TABLE_ROW_HEIGHT     1    /* 表格每行高度 */
#define TABLE_PAGE_SIZE      15   /* 表格每页显示行数 */
#define INPUT_FIELD_WIDTH    40   /* 输入框宽度 */

/* 用户角色定义 */
#define ROLE_ADMIN           0    /* 管理员角色 */
#define ROLE_DOCTOR          1    /* 医生角色 */
#define ROLE_PATIENT         2    /* 患者角色 */

/* 主菜单选项定义 */
#define MENU_PATIENT_MGMT    0    /* 患者管理 */
#define MENU_DOCTOR_MGMT     1    /* 医生管理 */
#define MENU_DRUG_MGMT       2    /* 药品管理 */
#define MENU_REGISTER_MGMT   3    /* 挂号管理 */
#define MENU_BILL_MGMT       4    /* 费用管理 */
#define MENU_USER_MGMT       5    /* 用户管理（仅管理员） */
#define MENU_LOGOUT          6    /* 退出登录 */
#define MENU_EXIT            7    /* 退出系统 */

/* 操作类型定义 */
#define OP_ADD               0    /* 添加操作 */
#define OP_DELETE            1    /* 删除操作 */
#define OP_MODIFY            2    /* 修改操作 */
#define OP_SEARCH            3    /* 查询操作 */
#define OP_SORT              4    /* 排序操作 */
#define OP_BACK              5    /* 返回上级菜单 */

/*
 * ============================================================================
 * 全局变量声明
 * 说明: 这些变量在整个UI模块中共享使用
 * ============================================================================
 */

/* 数据链表头指针 - 指向各类数据的链表头 */
extern PatientNode *g_patients; /* 患者数据链表头 */
extern DoctorNode *g_doctors; /* 医生数据链表头 */
extern DrugNode *g_drugs; /* 药品数据链表头 */
extern RegisterNode *g_registrations; /* 挂号记录链表头 */
extern BillNode *g_bills; /* 费用记录链表头 */
extern AuthNode *g_users; /* 用户账户链表头 */

/* 当前登录用户信息 */
extern AuthNode *g_current_user; /* 当前登录的用户节点 */

/*
 * ============================================================================
 * 核心界面函数声明
 * 说明: 这些是主要的界面控制函数
 * ============================================================================
 */

/**
 * 函数名: ui_init
 * 功能: 初始化用户界面
 * 说明: 初始化ncurses环境，设置颜色、键盘模式等
 *       在程序启动时调用一次
 * 参数: 无
 * 返回值: 0-成功, -1-失败
 */
int ui_init(void);

/**
 * 函数名: ui_cleanup
 * 功能: 清理用户界面资源
 * 说明: 关闭ncurses环境，恢复终端设置
 *       在程序退出前调用
 * 参数: 无
 * 返回值: 无
 */
void ui_cleanup(void);

/**
 * 函数名: ui_main
 * 功能: 运行主界面
 * 说明: 这是UI的主入口函数，负责整个程序的界面流程控制
 *       调用此函数会进入登录界面，登录成功后进入主界面
 * 参数: 无
 * 返回值: 0-正常退出
 */
int ui_main(void);

/*
 * ============================================================================
 * 登录界面函数声明
 * 说明: 处理用户登录相关的界面
 * ============================================================================
 */

/**
 * 函数名: ui_login_screen
 * 功能: 显示登录界面
 * 说明: 显示系统登录界面，允许用户输入用户名和密码
 *       验证成功后返回用户角色
 * 参数: 无
 * 返回值: 成功返回用户角色(0/1/2)，失败返回-1，退出返回-2
 */
int ui_login_screen(void);

/**
 * 函数名: ui_draw_login_form
 * 功能: 绘制登录表单
 * 说明: 在屏幕中央绘制登录表单，包含用户名和密码输入框
 * 参数: 
 *   - win: ncurses窗口指针
 * 返回值: 无
 */
void ui_draw_login_form(WINDOW *win);

/*
 * ============================================================================
 * 主界面函数声明
 * 说明: 主界面布局和导航相关函数
 * ============================================================================
 */

/**
 * 函数名: ui_main_screen
 * 功能: 显示主界面
 * 说明: 显示系统主界面，包含左侧菜单栏和右侧内容区域
 *       根据用户角色显示不同的菜单选项
 * 参数: 
 *   - role: 当前用户角色
 * 返回值: 无
 */
void ui_main_screen(int role);

/**
 * 函数名: ui_draw_sidebar
 * 功能: 绘制左侧菜单栏
 * 说明: 在屏幕左侧绘制功能菜单，根据用户角色显示不同选项
 * 参数: 
 *   - win: 菜单栏窗口指针
 *   - role: 用户角色
 *   - selected: 当前选中的菜单项索引
 * 返回值: 无
 */
void ui_draw_sidebar(WINDOW *win, int role, int selected);

/**
 * 函数名: ui_draw_header
 * 功能: 绘制顶部标题栏
 * 说明: 在屏幕顶部显示系统名称和当前用户信息
 * 参数: 
 *   - win: 标题栏窗口指针
 * 返回值: 无
 */
void ui_draw_header(WINDOW *win);

/**
 * 函数名: ui_draw_status_bar
 * 功能: 绘制底部状态栏
 * 说明: 在屏幕底部显示操作提示和快捷键说明
 * 参数: 
 *   - win: 状态栏窗口指针
 *   - message: 要显示的状态信息
 * 返回值: 无
 */
void ui_draw_status_bar(WINDOW *win, const char *message);

/*
 * ============================================================================
 * 通用表格显示函数声明
 * 说明: 类似Excel的数据表格显示功能
 * ============================================================================
 */

/**
 * 函数名: ui_draw_table_header
 * 功能: 绘制表格标题行
 * 说明: 在内容区域绘制数据表格的标题行（列名）
 * 参数: 
 *   - win: 内容窗口指针
 *   - headers: 列标题数组
 *   - col_count: 列数
 *   - col_widths: 每列宽度数组
 * 返回值: 无
 */
void ui_draw_table_header(WINDOW *win, const char **headers, int col_count, const int *col_widths);

/**
 * 函数名: ui_draw_operation_menu
 * 功能: 绘制操作菜单
 * 说明: 在表格下方显示可用操作选项（添加/删除/修改/查询/排序）
 * 参数: 
 *   - win: 内容窗口指针
 *   - y: 菜单起始行位置
 *   - selected: 当前选中的操作
 * 返回值: 无
 */
void ui_draw_operation_menu(WINDOW *win, int y, int selected);

/*
 * ============================================================================
 * 患者管理界面函数声明
 * ============================================================================
 */

/**
 * 函数名: ui_patient_management
 * 功能: 患者管理界面主函数
 * 说明: 显示患者列表，支持增删改查排序操作
 * 参数: 
 *   - content_win: 内容区域窗口指针
 * 返回值: 无
 */
void ui_patient_management(WINDOW *content_win);

/**
 * 函数名: ui_draw_patient_table
 * 功能: 绘制患者数据表格
 * 说明: 在内容区域显示患者信息列表
 * 参数: 
 *   - win: 内容窗口指针
 *   - start_index: 显示起始索引（分页用）
 *   - selected_row: 当前选中行
 * 返回值: 显示的行数
 */
int ui_draw_patient_table(WINDOW *win, int start_index, int selected_row);

/**
 * 函数名: ui_add_patient_form
 * 功能: 显示添加患者表单
 * 说明: 弹出表单窗口，让用户输入新患者信息
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 0-成功添加, -1-取消或失败
 */
int ui_add_patient_form(WINDOW *parent_win);

/**
 * 函数名: ui_modify_patient_form
 * 功能: 显示修改患者表单
 * 说明: 弹出表单窗口，让用户修改选中患者的信息
 * 参数: 
 *   - parent_win: 父窗口指针
 *   - patient: 要修改的患者节点
 * 返回值: 0-成功修改, -1-取消或失败
 */
int ui_modify_patient_form(WINDOW *parent_win, PatientNode *patient);

/**
 * 函数名: ui_search_patient
 * 功能: 患者查询界面
 * 说明: 允许用户按姓名或电话查询患者
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 无
 */
void ui_search_patient(WINDOW *parent_win);

/**
 * 函数名: ui_sort_patient_menu
 * 功能: 患者排序菜单
 * 说明: 显示排序选项（按姓名/电话/年龄排序）
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 无
 */
void ui_sort_patient_menu(WINDOW *parent_win);

/*
 * ============================================================================
 * 医生管理界面函数声明
 * ============================================================================
 */

/**
 * 函数名: ui_doctor_management
 * 功能: 医生管理界面主函数
 * 说明: 显示医生列表，支持增删改查排序操作
 * 参数: 
 *   - content_win: 内容区域窗口指针
 * 返回值: 无
 */
void ui_doctor_management(WINDOW *content_win);

/**
 * 函数名: ui_draw_doctor_table
 * 功能: 绘制医生数据表格
 * 说明: 在内容区域显示医生信息列表
 * 参数: 
 *   - win: 内容窗口指针
 *   - start_index: 显示起始索引（分页用）
 *   - selected_row: 当前选中行
 * 返回值: 显示的行数
 */
int ui_draw_doctor_table(WINDOW *win, int start_index, int selected_row);

/**
 * 函数名: ui_add_doctor_form
 * 功能: 显示添加医生表单
 * 说明: 弹出表单窗口，让用户输入新医生信息
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 0-成功添加, -1-取消或失败
 */
int ui_add_doctor_form(WINDOW *parent_win);

/**
 * 函数名: ui_modify_doctor_form
 * 功能: 显示修改医生表单
 * 说明: 弹出表单窗口，让用户修改选中医生的信息
 * 参数: 
 *   - parent_win: 父窗口指针
 *   - doctor: 要修改的医生节点
 * 返回值: 0-成功修改, -1-取消或失败
 */
int ui_modify_doctor_form(WINDOW *parent_win, DoctorNode *doctor);

/**
 * 函数名: ui_search_doctor
 * 功能: 医生查询界面
 * 说明: 允许用户按姓名或电话查询医生
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 无
 */
void ui_search_doctor(WINDOW *parent_win);

/**
 * 函数名: ui_sort_doctor_menu
 * 功能: 医生排序菜单
 * 说明: 显示排序选项（按姓名/电话/年龄排序）
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 无
 */
void ui_sort_doctor_menu(WINDOW *parent_win);

/*
 * ============================================================================
 * 药品管理界面函数声明
 * ============================================================================
 */

/**
 * 函数名: ui_drug_management
 * 功能: 药品管理界面主函数
 * 说明: 显示药品列表，支持增删改查排序操作
 * 参数: 
 *   - content_win: 内容区域窗口指针
 * 返回值: 无
 */
void ui_drug_management(WINDOW *content_win);

/**
 * 函数名: ui_draw_drug_table
 * 功能: 绘制药品数据表格
 * 说明: 在内容区域显示药品信息列表
 * 参数: 
 *   - win: 内容窗口指针
 *   - start_index: 显示起始索引（分页用）
 *   - selected_row: 当前选中行
 * 返回值: 显示的行数
 */
int ui_draw_drug_table(WINDOW *win, int start_index, int selected_row);

/**
 * 函数名: ui_add_drug_form
 * 功能: 显示添加药品表单
 * 说明: 弹出表单窗口，让用户输入新药品信息
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 0-成功添加, -1-取消或失败
 */
int ui_add_drug_form(WINDOW *parent_win);

/**
 * 函数名: ui_modify_drug_form
 * 功能: 显示修改药品表单
 * 说明: 弹出表单窗口，让用户修改选中药品的信息
 * 参数: 
 *   - parent_win: 父窗口指针
 *   - drug: 要修改的药品节点
 * 返回值: 0-成功修改, -1-取消或失败
 */
int ui_modify_drug_form(WINDOW *parent_win, DrugNode *drug);

/**
 * 函数名: ui_search_drug
 * 功能: 药品查询界面
 * 说明: 允许用户按名称查询药品
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 无
 */
void ui_search_drug(WINDOW *parent_win);

/**
 * 函数名: ui_sort_drug_menu
 * 功能: 药品排序菜单
 * 说明: 显示排序选项（按价格/库存排序）
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 无
 */
void ui_sort_drug_menu(WINDOW *parent_win);

/*
 * ============================================================================
 * 挂号管理界面函数声明
 * ============================================================================
 */

/**
 * 函数名: ui_registration_management
 * 功能: 挂号管理界面主函数
 * 说明: 显示挂号记录列表，支持增删查操作
 * 参数: 
 *   - content_win: 内容区域窗口指针
 * 返回值: 无
 */
void ui_registration_management(WINDOW *content_win);

/**
 * 函数名: ui_draw_registration_table
 * 功能: 绘制挂号记录数据表格
 * 说明: 在内容区域显示挂号记录列表
 * 参数: 
 *   - win: 内容窗口指针
 *   - start_index: 显示起始索引（分页用）
 *   - selected_row: 当前选中行
 * 返回值: 显示的行数
 */
int ui_draw_registration_table(WINDOW *win, int start_index, int selected_row);

/**
 * 函数名: ui_add_registration_form
 * 功能: 显示添加挂号记录表单
 * 说明: 弹出表单窗口，让用户输入新挂号信息
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 0-成功添加, -1-取消或失败
 */
int ui_add_registration_form(WINDOW *parent_win);

/**
 * 函数名: ui_search_registration
 * 功能: 挂号记录查询界面
 * 说明: 允许用户按患者/医生/科室/日期查询挂号记录
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 无
 */
void ui_search_registration(WINDOW *parent_win);

/*
 * ============================================================================
 * 费用管理界面函数声明
 * ============================================================================
 */

/**
 * 函数名: ui_bill_management
 * 功能: 费用管理界面主函数
 * 说明: 显示费用记录列表，支持增删改查排序操作
 * 参数: 
 *   - content_win: 内容区域窗口指针
 * 返回值: 无
 */
void ui_bill_management(WINDOW *content_win);

/**
 * 函数名: ui_draw_bill_table
 * 功能: 绘制费用记录数据表格
 * 说明: 在内容区域显示费用记录列表
 * 参数: 
 *   - win: 内容窗口指针
 *   - start_index: 显示起始索引（分页用）
 *   - selected_row: 当前选中行
 * 返回值: 显示的行数
 */
int ui_draw_bill_table(WINDOW *win, int start_index, int selected_row);

/**
 * 函数名: ui_add_bill_form
 * 功能: 显示添加费用记录表单
 * 说明: 弹出表单窗口，让用户输入新费用信息
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 0-成功添加, -1-取消或失败
 */
int ui_add_bill_form(WINDOW *parent_win);

/**
 * 函数名: ui_modify_bill_form
 * 功能: 显示修改费用记录表单
 * 说明: 弹出表单窗口，让用户修改选中费用记录的信息
 * 参数: 
 *   - parent_win: 父窗口指针
 *   - bill: 要修改的费用记录节点
 * 返回值: 0-成功修改, -1-取消或失败
 */
int ui_modify_bill_form(WINDOW *parent_win, BillNode *bill);

/**
 * 函数名: ui_search_bill
 * 功能: 费用记录查询界面
 * 说明: 允许用户按患者姓名或收费项目查询费用记录
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 无
 */
void ui_search_bill(WINDOW *parent_win);

/**
 * 函数名: ui_sort_bill_menu
 * 功能: 费用记录排序菜单
 * 说明: 显示排序选项（按金额排序）
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 无
 */
void ui_sort_bill_menu(WINDOW *parent_win);

/*
 * ============================================================================
 * 用户管理界面函数声明（仅管理员可用）
 * ============================================================================
 */

/**
 * 函数名: ui_user_management
 * 功能: 用户管理界面主函数
 * 说明: 显示用户账户列表，支持增删改操作（仅管理员可用）
 * 参数: 
 *   - content_win: 内容区域窗口指针
 * 返回值: 无
 */
void ui_user_management(WINDOW *content_win);

/**
 * 函数名: ui_draw_user_table
 * 功能: 绘制用户数据表格
 * 说明: 在内容区域显示用户账户列表
 * 参数: 
 *   - win: 内容窗口指针
 *   - start_index: 显示起始索引（分页用）
 *   - selected_row: 当前选中行
 * 返回值: 显示的行数
 */
int ui_draw_user_table(WINDOW *win, int start_index, int selected_row);

/**
 * 函数名: ui_add_user_form
 * 功能: 显示添加用户表单
 * 说明: 弹出表单窗口，让管理员输入新用户信息
 * 参数: 
 *   - parent_win: 父窗口指针
 * 返回值: 0-成功添加, -1-取消或失败
 */
int ui_add_user_form(WINDOW *parent_win);

/**
 * 函数名: ui_modify_user_form
 * 功能: 显示修改用户表单
 * 说明: 弹出表单窗口，让管理员修改选中用户的信息
 * 参数: 
 *   - parent_win: 父窗口指针
 *   - user: 要修改的用户节点
 * 返回值: 0-成功修改, -1-取消或失败
 */
int ui_modify_user_form(WINDOW *parent_win, AuthNode *user);

/*
 * ============================================================================
 * 工具函数声明
 * 说明: 这些是辅助UI功能的工具函数
 * ============================================================================
 */

/**
 * 函数名: ui_show_message
 * 功能: 显示消息框
 * 说明: 在屏幕中央显示一个消息框，等待用户按键确认
 * 参数: 
 *   - title: 消息框标题
 *   - message: 消息内容
 *   - type: 消息类型(0-普通, 1-成功, 2-错误, 3-警告)
 * 返回值: 无
 */
void ui_show_message(const char *title, const char *message, int type);

/**
 * 函数名: ui_confirm_dialog
 * 功能: 显示确认对话框
 * 说明: 在屏幕中央显示确认对话框，让用户选择是或否
 * 参数: 
 *   - title: 对话框标题
 *   - message: 确认消息
 * 返回值: 1-用户选择是, 0-用户选择否
 */
int ui_confirm_dialog(const char *title, const char *message);

/**
 * 函数名: ui_input_string
 * 功能: 获取用户字符串输入
 * 说明: 在指定位置显示输入框，获取用户输入的字符串
 * 参数: 
 *   - win: 窗口指针
 *   - y: 输入框Y坐标
 *   - x: 输入框X坐标
 *   - buffer: 接收输入的缓冲区
 *   - max_len: 最大输入长度
 *   - hidden: 是否隐藏输入（用于密码）
 * 返回值: 输入的字符数，-1表示取消
 */
int ui_input_string(WINDOW *win, int y, int x, char *buffer, int max_len, int hidden);

/**
 * 函数名: ui_input_int
 * 功能: 获取用户整数输入
 * 说明: 在指定位置显示输入框，获取用户输入的整数
 * 参数: 
 *   - win: 窗口指针
 *   - y: 输入框Y坐标
 *   - x: 输入框X坐标
 *   - value: 接收输入值的指针
 * 返回值: 0-成功, -1-取消或无效输入
 */
int ui_input_int(WINDOW *win, int y, int x, int *value);

/**
 * 函数名: ui_input_double
 * 功能: 获取用户浮点数输入
 * 说明: 在指定位置显示输入框，获取用户输入的浮点数
 * 参数: 
 *   - win: 窗口指针
 *   - y: 输入框Y坐标
 *   - x: 输入框X坐标
 *   - value: 接收输入值的指针
 * 返回值: 0-成功, -1-取消或无效输入
 */
int ui_input_double(WINDOW *win, int y, int x, double *value);

/**
 * 函数名: ui_center_string
 * 功能: 居中显示字符串
 * 说明: 在窗口的指定行居中显示字符串
 * 参数: 
 *   - win: 窗口指针
 *   - y: 行号
 *   - str: 要显示的字符串
 * 返回值: 无
 */
void ui_center_string(WINDOW *win, int y, const char *str);

/**
 * 函数名: ui_draw_box
 * 功能: 绘制带标题的边框
 * 说明: 在窗口周围绘制边框，并在顶部显示标题
 * 参数: 
 *   - win: 窗口指针
 *   - title: 标题字符串（可为NULL）
 * 返回值: 无
 */
void ui_draw_box(WINDOW *win, const char *title);

/**
 * 函数名: get_node_by_index
 * 功能: 根据索引获取链表节点
 * 说明: 遍历链表获取指定索引位置的节点
 * 参数: 
 *   - head: 链表头指针
 *   - index: 节点索引（从0开始）
 *   - type: 链表类型(0-患者, 1-医生, 2-药品, 3-挂号, 4-费用, 5-用户)
 * 返回值: 节点指针，未找到返回NULL
 */
void *get_node_by_index(void *head, int index, int type);

/**
 * 函数名: count_list_nodes
 * 功能: 统计链表节点数量
 * 说明: 遍历链表统计节点总数
 * 参数: 
 *   - head: 链表头指针
 *   - type: 链表类型(0-患者, 1-医生, 2-药品, 3-挂号, 4-费用, 5-用户)
 * 返回值: 节点数量
 */
int count_list_nodes(void *head, int type);

/**
 * 函数名: get_role_string
 * 功能: 获取角色名称字符串
 * 说明: 将角色编号转换为中文名称
 * 参数: 
 *   - role: 角色编号
 * 返回值: 角色名称字符串
 */
const char *get_role_string(int role);

#endif //HOSPITALSYSTEM_UI_H
