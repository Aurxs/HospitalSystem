#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/datastruct.h"
#include "include/file_io.h"
#include "include/auth.h"
#include "include/patient.h"
#include "include/doctor.h"

// Global Application Data
typedef struct {
    PatientNode *patients;
    DoctorNode *doctors;
    DrugNode *drugs;
    RegisterNode *registrations;
    BillNode *bills;
    AuthNode *users;
    AuthNode *currentUser;

    GtkWidget *window;
    GtkWidget *login_box;
    GtkWidget *main_box;
    GtkWidget *stack;
} AppData;

AppData app_data;

// Function Prototypes
void show_login_screen(GtkApplication *app);
void show_main_window();
void on_login_clicked(GtkWidget *widget, gpointer data);
void load_all_data();
void save_all_data();

// Helper to show error dialog
void show_error(const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app_data.window),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_ERROR,
                                               GTK_BUTTONS_OK,
                                               "%s", message);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
    gtk_widget_set_visible(dialog, TRUE);
}

// Helper to show info dialog
void show_info(const char *message) {
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(app_data.window),
                                               GTK_DIALOG_MODAL,
                                               GTK_MESSAGE_INFO,
                                               GTK_BUTTONS_OK,
                                               "%s", message);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
    gtk_widget_set_visible(dialog, TRUE);
}

// --- List View Helpers ---

void setup_list_item(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data) {
    GtkWidget *label = gtk_label_new("");
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_list_item_set_child(list_item, label);
}

void bind_list_item(GtkListItemFactory *factory, GtkListItem *list_item, gpointer user_data) {
    GtkWidget *label = gtk_list_item_get_child(list_item);
    GtkStringObject *strobj = (GtkStringObject*)gtk_list_item_get_item(list_item);
    gtk_label_set_text(GTK_LABEL(label), gtk_string_object_get_string(strobj));
}

// --- Dialog Helpers ---

typedef struct {
    GtkWidget *dialog;
    GtkWidget *entry_name;
    GtkWidget *entry_age;
    GtkWidget *entry_gender;
    GtkWidget *entry_phone;
    GtkWidget *entry_extra1; // Diagnosis or Department
    GtkWidget *entry_extra2; // Treatment or nothing
    char original_phone[MAX_PHONE];
    gboolean is_edit;
} DialogData;


// --- Data Loading/Saving ---

void load_all_data() {
    app_data.patients = load_patients("data/patients.txt");
    app_data.doctors = load_doctors("data/doctors.txt");
    app_data.drugs = load_drugs("data/drugs.txt");
    app_data.registrations = load_registrations("data/registrations.txt");
    app_data.bills = load_bills("data/bills.txt");
    app_data.users = load_users("data/auth.txt");

    // Ensure at least one admin exists if no users
    if (app_data.users == NULL) {
        AuthNode admin = make_user("admin", "admin", 0);
        add_user(&app_data.users, admin);
        save_users("data/auth.txt", app_data.users);
    }
}

void save_all_data() {
    save_patients("data/patients.txt", app_data.patients);
    save_doctors("data/doctors.txt", app_data.doctors);
    save_drugs("data/drugs.txt", app_data.drugs);
    save_registrations("data/registrations.txt", app_data.registrations);
    save_bills("data/bills.txt", app_data.bills);
    save_users("data/auth.txt", app_data.users);
}

// --- Patient Management ---

void on_patient_dialog_response(GtkDialog *dialog, int response_id, gpointer user_data) {
    DialogData *data = (DialogData*)user_data;
    if (response_id == GTK_RESPONSE_ACCEPT) {
        const char *name = gtk_editable_get_text(GTK_EDITABLE(data->entry_name));
        const char *age_str = gtk_editable_get_text(GTK_EDITABLE(data->entry_age));
        const char *gender = gtk_editable_get_text(GTK_EDITABLE(data->entry_gender));
        const char *phone = gtk_editable_get_text(GTK_EDITABLE(data->entry_phone));
        const char *diagnosis = gtk_editable_get_text(GTK_EDITABLE(data->entry_extra1));
        const char *treatment = gtk_editable_get_text(GTK_EDITABLE(data->entry_extra2));

        int age = atoi(age_str);
        PatientNode new_patient = make_patient(name, age, gender, phone, diagnosis, treatment);

        if (data->is_edit) {
            modify_patient(app_data.patients, data->original_phone, new_patient);
        } else {
            add_patient(&app_data.patients, new_patient);
        }
        save_all_data();
        show_main_window(); // Refresh UI
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
    g_free(data);
}

void open_patient_dialog(PatientNode *patient) {
    DialogData *data = g_new0(DialogData, 1);
    data->is_edit = (patient != NULL);
    if (patient) {
        strcpy(data->original_phone, patient->phone);
    }

    data->dialog = gtk_dialog_new_with_buttons(patient ? "编辑患者" : "添加患者",
                                               GTK_WINDOW(app_data.window),
                                               GTK_DIALOG_MODAL,
                                               "取消", GTK_RESPONSE_CANCEL,
                                               "保存", GTK_RESPONSE_ACCEPT,
                                               NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(data->dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_widget_set_margin_top(grid, 10);
    gtk_widget_set_margin_bottom(grid, 10);
    gtk_widget_set_margin_start(grid, 10);
    gtk_widget_set_margin_end(grid, 10);
    gtk_box_append(GTK_BOX(content_area), grid);

    int row = 0;
    GtkWidget *label;

    label = gtk_label_new("姓名:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_name = gtk_entry_new();
    if (patient) gtk_editable_set_text(GTK_EDITABLE(data->entry_name), patient->name);
    gtk_grid_attach(GTK_GRID(grid), data->entry_name, 1, row, 1, 1); row++;

    label = gtk_label_new("年龄:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_age = gtk_entry_new();
    if (patient) { char buf[10]; sprintf(buf, "%d", patient->age); gtk_editable_set_text(GTK_EDITABLE(data->entry_age), buf); }
    gtk_grid_attach(GTK_GRID(grid), data->entry_age, 1, row, 1, 1); row++;

    label = gtk_label_new("性别:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_gender = gtk_entry_new();
    if (patient) gtk_editable_set_text(GTK_EDITABLE(data->entry_gender), patient->gender);
    gtk_grid_attach(GTK_GRID(grid), data->entry_gender, 1, row, 1, 1); row++;

    label = gtk_label_new("电话:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_phone = gtk_entry_new();
    if (patient) gtk_editable_set_text(GTK_EDITABLE(data->entry_phone), patient->phone);
    gtk_grid_attach(GTK_GRID(grid), data->entry_phone, 1, row, 1, 1); row++;

    label = gtk_label_new("诊断:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_extra1 = gtk_entry_new();
    if (patient) gtk_editable_set_text(GTK_EDITABLE(data->entry_extra1), patient->diagnosis);
    gtk_grid_attach(GTK_GRID(grid), data->entry_extra1, 1, row, 1, 1); row++;

    label = gtk_label_new("治疗:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_extra2 = gtk_entry_new();
    if (patient) gtk_editable_set_text(GTK_EDITABLE(data->entry_extra2), patient->treatment);
    gtk_grid_attach(GTK_GRID(grid), data->entry_extra2, 1, row, 1, 1); row++;

    g_signal_connect(data->dialog, "response", G_CALLBACK(on_patient_dialog_response), data);
    gtk_widget_set_visible(data->dialog, TRUE);
}

void on_add_patient_clicked(GtkWidget *btn, gpointer data) {
    open_patient_dialog(NULL);
}

void on_edit_patient_clicked(GtkWidget *btn, gpointer data) {
    GtkSelectionModel *model = GTK_SELECTION_MODEL(data);
    guint pos = gtk_single_selection_get_selected(GTK_SINGLE_SELECTION(model));
    if (pos == GTK_INVALID_LIST_POSITION) return;

    GtkStringObject *obj = GTK_STRING_OBJECT(g_list_model_get_item(G_LIST_MODEL(model), pos));
    const char *str = gtk_string_object_get_string(obj);
    char buffer[1024];
    strncpy(buffer, str, sizeof(buffer));
    char *token = strtok(buffer, "|"); // Name
    strtok(NULL, "|"); // Age
    strtok(NULL, "|"); // Gender
    token = strtok(NULL, "|"); // Phone
    if (token) {
        while(*token == ' ') token++;
        char *end = token + strlen(token) - 1;
        while(end > token && *end == ' ') *end-- = '\0';
        PatientNode *p = findPatient_phone(app_data.patients, token);
        if (p) open_patient_dialog(p);
    }
}

void on_delete_patient_clicked(GtkWidget *btn, gpointer data) {
    GtkSelectionModel *model = GTK_SELECTION_MODEL(data);
    guint pos = gtk_single_selection_get_selected(GTK_SINGLE_SELECTION(model));
    if (pos == GTK_INVALID_LIST_POSITION) return;

    GtkStringObject *obj = GTK_STRING_OBJECT(g_list_model_get_item(G_LIST_MODEL(model), pos));
    const char *str = gtk_string_object_get_string(obj);
    char buffer[1024];
    strncpy(buffer, str, sizeof(buffer));
    char *token = strtok(buffer, "|"); // Name
    strtok(NULL, "|"); // Age
    strtok(NULL, "|"); // Gender
    token = strtok(NULL, "|"); // Phone
    if (token) {
        while(*token == ' ') token++;
        char *end = token + strlen(token) - 1;
        while(end > token && *end == ' ') *end-- = '\0';
        app_data.patients = delete_patient(app_data.patients, token);
        save_all_data();
        show_main_window();
    }
}

// --- Doctor Management ---

void on_doctor_dialog_response(GtkDialog *dialog, int response_id, gpointer user_data) {
    DialogData *data = (DialogData*)user_data;
    if (response_id == GTK_RESPONSE_ACCEPT) {
        const char *name = gtk_editable_get_text(GTK_EDITABLE(data->entry_name));
        const char *age_str = gtk_editable_get_text(GTK_EDITABLE(data->entry_age));
        const char *gender = gtk_editable_get_text(GTK_EDITABLE(data->entry_gender));
        const char *phone = gtk_editable_get_text(GTK_EDITABLE(data->entry_phone));
        const char *dept = gtk_editable_get_text(GTK_EDITABLE(data->entry_extra1));

        int age = atoi(age_str);
        DoctorNode new_doctor = make_doctor(name, age, gender, dept, phone);

        if (data->is_edit) {
            modify_doctor(app_data.doctors, data->original_phone, new_doctor);
        } else {
            add_doctor(&app_data.doctors, new_doctor);
        }
        save_all_data();
        show_main_window();
    }
    gtk_window_destroy(GTK_WINDOW(dialog));
    g_free(data);
}

void open_doctor_dialog(DoctorNode *doctor) {
    DialogData *data = g_new0(DialogData, 1);
    data->is_edit = (doctor != NULL);
    if (doctor) {
        strcpy(data->original_phone, doctor->phone);
    }

    data->dialog = gtk_dialog_new_with_buttons(doctor ? "编辑医生" : "添加医生",
                                               GTK_WINDOW(app_data.window),
                                               GTK_DIALOG_MODAL,
                                               "取消", GTK_RESPONSE_CANCEL,
                                               "保存", GTK_RESPONSE_ACCEPT,
                                               NULL);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(data->dialog));
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_widget_set_margin_top(grid, 10);
    gtk_widget_set_margin_bottom(grid, 10);
    gtk_widget_set_margin_start(grid, 10);
    gtk_widget_set_margin_end(grid, 10);
    gtk_box_append(GTK_BOX(content_area), grid);

    int row = 0;
    GtkWidget *label;

    label = gtk_label_new("姓名:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_name = gtk_entry_new();
    if (doctor) gtk_editable_set_text(GTK_EDITABLE(data->entry_name), doctor->name);
    gtk_grid_attach(GTK_GRID(grid), data->entry_name, 1, row, 1, 1); row++;

    label = gtk_label_new("年龄:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_age = gtk_entry_new();
    if (doctor) { char buf[10]; sprintf(buf, "%d", doctor->age); gtk_editable_set_text(GTK_EDITABLE(data->entry_age), buf); }
    gtk_grid_attach(GTK_GRID(grid), data->entry_age, 1, row, 1, 1); row++;

    label = gtk_label_new("性别:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_gender = gtk_entry_new();
    if (doctor) gtk_editable_set_text(GTK_EDITABLE(data->entry_gender), doctor->gender);
    gtk_grid_attach(GTK_GRID(grid), data->entry_gender, 1, row, 1, 1); row++;

    label = gtk_label_new("电话:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_phone = gtk_entry_new();
    if (doctor) gtk_editable_set_text(GTK_EDITABLE(data->entry_phone), doctor->phone);
    gtk_grid_attach(GTK_GRID(grid), data->entry_phone, 1, row, 1, 1); row++;

    label = gtk_label_new("科室:"); gtk_grid_attach(GTK_GRID(grid), label, 0, row, 1, 1);
    data->entry_extra1 = gtk_entry_new();
    if (doctor) gtk_editable_set_text(GTK_EDITABLE(data->entry_extra1), doctor->department);
    gtk_grid_attach(GTK_GRID(grid), data->entry_extra1, 1, row, 1, 1); row++;

    g_signal_connect(data->dialog, "response", G_CALLBACK(on_doctor_dialog_response), data);
    gtk_widget_set_visible(data->dialog, TRUE);
}

void on_add_doctor_clicked(GtkWidget *btn, gpointer data) {
    open_doctor_dialog(NULL);
}

void on_edit_doctor_clicked(GtkWidget *btn, gpointer data) {
    GtkSelectionModel *model = GTK_SELECTION_MODEL(data);
    guint pos = gtk_single_selection_get_selected(GTK_SINGLE_SELECTION(model));
    if (pos == GTK_INVALID_LIST_POSITION) return;

    GtkStringObject *obj = GTK_STRING_OBJECT(g_list_model_get_item(G_LIST_MODEL(model), pos));
    const char *str = gtk_string_object_get_string(obj);
    char buffer[1024];
    strncpy(buffer, str, sizeof(buffer));
    char *token = strtok(buffer, "|"); // Name
    strtok(NULL, "|"); // Age
    strtok(NULL, "|"); // Gender
    strtok(NULL, "|"); // Department
    token = strtok(NULL, "|"); // Phone
    if (token) {
        while(*token == ' ') token++;
        char *end = token + strlen(token) - 1;
        while(end > token && *end == ' ') *end-- = '\0';
        DoctorNode *d = findDoctor_phone(app_data.doctors, token);
        if (d) open_doctor_dialog(d);
    }
}

void on_delete_doctor_clicked(GtkWidget *btn, gpointer data) {
    GtkSelectionModel *model = GTK_SELECTION_MODEL(data);
    guint pos = gtk_single_selection_get_selected(GTK_SINGLE_SELECTION(model));
    if (pos == GTK_INVALID_LIST_POSITION) return;

    GtkStringObject *obj = GTK_STRING_OBJECT(g_list_model_get_item(G_LIST_MODEL(model), pos));
    const char *str = gtk_string_object_get_string(obj);
    char buffer[1024];
    strncpy(buffer, str, sizeof(buffer));
    char *token = strtok(buffer, "|"); // Name
    strtok(NULL, "|"); // Age
    strtok(NULL, "|"); // Gender
    strtok(NULL, "|"); // Department
    token = strtok(NULL, "|"); // Phone
    if (token) {
        while(*token == ' ') token++;
        char *end = token + strlen(token) - 1;
        while(end > token && *end == ' ') *end-- = '\0';
        app_data.doctors = delete_doctor(app_data.doctors, token);
        save_all_data();
        show_main_window();
    }
}

// --- UI Construction ---

GtkWidget* create_patient_view() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label = gtk_label_new("患者管理 (Patient Management)");
    gtk_box_append(GTK_BOX(box), label);

    // Toolbar
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(box), toolbar);

    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_box_append(GTK_BOX(box), scrolled);

    GtkStringList *string_list = gtk_string_list_new(NULL);
    PatientNode *curr = app_data.patients;
    while(curr) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%s | %d | %s | %s | %s | %s",
                 curr->name, curr->age, curr->gender, curr->phone, curr->diagnosis, curr->treatment);
        gtk_string_list_append(string_list, buffer);
        curr = curr->next;
    }

    GtkSingleSelection *selection_model = gtk_single_selection_new(G_LIST_MODEL(string_list));
    GtkWidget *list_view = gtk_column_view_new(GTK_SELECTION_MODEL(selection_model));

    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_list_item), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_list_item), NULL);

    GtkColumnViewColumn *column = gtk_column_view_column_new("信息 (姓名 | 年龄 | 性别 | 电话 | 诊断 | 治疗)", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(list_view), column);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), list_view);

    // Buttons
    if (app_data.currentUser->role == 0 || app_data.currentUser->role == 1) {
        GtkWidget *btn_add = gtk_button_new_with_label("添加");
        g_signal_connect(btn_add, "clicked", G_CALLBACK(on_add_patient_clicked), NULL);
        gtk_box_append(GTK_BOX(toolbar), btn_add);

        GtkWidget *btn_edit = gtk_button_new_with_label("修改");
        g_signal_connect(btn_edit, "clicked", G_CALLBACK(on_edit_patient_clicked), selection_model);
        gtk_box_append(GTK_BOX(toolbar), btn_edit);

        GtkWidget *btn_del = gtk_button_new_with_label("删除");
        g_signal_connect(btn_del, "clicked", G_CALLBACK(on_delete_patient_clicked), selection_model);
        gtk_box_append(GTK_BOX(toolbar), btn_del);
    }

    return box;
}

GtkWidget* create_doctor_view() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(box), gtk_label_new("医生管理 (Doctor Management)"));

    // Toolbar
    GtkWidget *toolbar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_append(GTK_BOX(box), toolbar);

    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled, TRUE);
    gtk_box_append(GTK_BOX(box), scrolled);

    GtkStringList *string_list = gtk_string_list_new(NULL);
    DoctorNode *curr = app_data.doctors;
    while(curr) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%s | %d | %s | %s | %s",
                 curr->name, curr->age, curr->gender, curr->department, curr->phone);
        gtk_string_list_append(string_list, buffer);
        curr = curr->next;
    }

    GtkSingleSelection *selection_model = gtk_single_selection_new(G_LIST_MODEL(string_list));
    GtkWidget *list_view = gtk_column_view_new(GTK_SELECTION_MODEL(selection_model));

    GtkListItemFactory *factory = gtk_signal_list_item_factory_new();
    g_signal_connect(factory, "setup", G_CALLBACK(setup_list_item), NULL);
    g_signal_connect(factory, "bind", G_CALLBACK(bind_list_item), NULL);

    GtkColumnViewColumn *column = gtk_column_view_column_new("信息 (姓名 | 年龄 | 性别 | 科室 | 电话)", factory);
    gtk_column_view_append_column(GTK_COLUMN_VIEW(list_view), column);

    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), list_view);

    // Buttons (Only for Admin)
    if (app_data.currentUser->role == 0) {
        GtkWidget *btn_add = gtk_button_new_with_label("添加");
        g_signal_connect(btn_add, "clicked", G_CALLBACK(on_add_doctor_clicked), NULL);
        gtk_box_append(GTK_BOX(toolbar), btn_add);

        GtkWidget *btn_edit = gtk_button_new_with_label("修改");
        g_signal_connect(btn_edit, "clicked", G_CALLBACK(on_edit_doctor_clicked), selection_model);
        gtk_box_append(GTK_BOX(toolbar), btn_edit);

        GtkWidget *btn_del = gtk_button_new_with_label("删除");
        g_signal_connect(btn_del, "clicked", G_CALLBACK(on_delete_doctor_clicked), selection_model);
        gtk_box_append(GTK_BOX(toolbar), btn_del);
    }

    return box;
}

void show_main_window() {
    // Clear existing children of window
    GtkWidget *child = gtk_window_get_child(GTK_WINDOW(app_data.window));
    if (child) {
        gtk_window_set_child(GTK_WINDOW(app_data.window), NULL);
    }

    app_data.main_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_window_set_child(GTK_WINDOW(app_data.window), app_data.main_box);

    // Sidebar
    GtkWidget *sidebar = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_widget_set_size_request(sidebar, 150, -1);
    gtk_box_append(GTK_BOX(app_data.main_box), sidebar);

    GtkWidget *stack = gtk_stack_new();
    app_data.stack = stack;
    gtk_widget_set_hexpand(stack, TRUE);
    gtk_box_append(GTK_BOX(app_data.main_box), stack);

    GtkWidget *stack_switcher = gtk_stack_switcher_new();
    gtk_stack_switcher_set_stack(GTK_STACK_SWITCHER(stack_switcher), GTK_STACK(stack));
    gtk_box_append(GTK_BOX(sidebar), stack_switcher);

    // Logout button
    GtkWidget *logout_btn = gtk_button_new_with_label("注销 (Logout)");
    gtk_widget_set_valign(logout_btn, GTK_ALIGN_END);
    gtk_widget_set_vexpand(logout_btn, TRUE);
    g_signal_connect_swapped(logout_btn, "clicked", G_CALLBACK(show_login_screen), NULL); // Simplified logout
    gtk_box_append(GTK_BOX(sidebar), logout_btn);

    // Add pages based on role
    int role = app_data.currentUser->role; // 0: Admin, 1: Doctor, 2: Patient

    if (role == 0 || role == 1) {
        gtk_stack_add_titled(GTK_STACK(stack), create_patient_view(), "patients", "患者管理");
    }

    if (role == 0 || role == 2) {
        gtk_stack_add_titled(GTK_STACK(stack), create_doctor_view(), "doctors", "医生列表");
    }

    // Add more views as needed...
    GtkWidget *welcome = gtk_label_new("欢迎使用医院管理系统");
    gtk_stack_add_titled(GTK_STACK(stack), welcome, "home", "首页");
}

void on_login_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget **)data;
    const char *username = gtk_editable_get_text(GTK_EDITABLE(entries[0]));
    const char *password = gtk_editable_get_text(GTK_EDITABLE(entries[1]));

    if (authenticate_user(app_data.users, username, password)) {
        app_data.currentUser = find_user(app_data.users, username);
        show_main_window();
    } else {
        show_error("用户名或密码错误！");
    }
}

void show_login_screen(GtkApplication *app) {
    // If window already exists, just clear it
    if (app_data.window == NULL) {
        app_data.window = gtk_application_window_new(app);
        gtk_window_set_title(GTK_WINDOW(app_data.window), "医院管理系统");
        gtk_window_set_default_size(GTK_WINDOW(app_data.window), 800, 600);
    } else {
        gtk_window_set_child(GTK_WINDOW(app_data.window), NULL);
    }

    app_data.login_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_halign(app_data.login_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(app_data.login_box, GTK_ALIGN_CENTER);

    GtkWidget *title = gtk_label_new("系统登录");
    gtk_widget_add_css_class(title, "title-1");
    gtk_box_append(GTK_BOX(app_data.login_box), title);

    GtkWidget *user_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(user_entry), "用户名");
    gtk_box_append(GTK_BOX(app_data.login_box), user_entry);

    GtkWidget *pass_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(pass_entry), "密码");
    gtk_entry_set_visibility(GTK_ENTRY(pass_entry), FALSE);
    gtk_box_append(GTK_BOX(app_data.login_box), pass_entry);

    GtkWidget *login_btn = gtk_button_new_with_label("登录");

    // Pass entries to callback
    GtkWidget **entries = g_new(GtkWidget*, 2);
    entries[0] = user_entry;
    entries[1] = pass_entry;

    g_signal_connect(login_btn, "clicked", G_CALLBACK(on_login_clicked), entries);
    gtk_box_append(GTK_BOX(app_data.login_box), login_btn);

    gtk_window_set_child(GTK_WINDOW(app_data.window), app_data.login_box);
    gtk_window_present(GTK_WINDOW(app_data.window));
}

static void activate(GtkApplication *app, gpointer user_data) {
    load_all_data();
    show_login_screen(app);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.hospital", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
