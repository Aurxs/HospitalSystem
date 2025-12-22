#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "include/datastruct.h"
#include "include/file_io.h"
#include "include/auth.h"

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

// --- UI Construction ---

GtkWidget* create_patient_view() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget *label = gtk_label_new("患者管理 (Patient Management)");
    gtk_box_append(GTK_BOX(box), label);

    // Simple list view (placeholder for real implementation)
    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled, TRUE);

    GtkStringList *string_list = gtk_string_list_new(NULL);
    PatientNode *curr = app_data.patients;
    while(curr) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s - %s", curr->name, curr->diagnosis);
        gtk_string_list_append(string_list, buffer);
        curr = curr->next;
    }

    GtkWidget *list_view = gtk_column_view_new(GTK_SELECTION_MODEL(gtk_single_selection_new(G_LIST_MODEL(string_list))));
    // In a real app, we would set up columns here. For now just a placeholder.
    // Since GtkColumnView requires more setup, let's use a simple Label for demo if list is empty
    if (app_data.patients == NULL) {
        gtk_box_append(GTK_BOX(box), gtk_label_new("暂无患者数据"));
    } else {
        // For simplicity in this demo, just listing names in a text view
        GtkWidget *text_view = gtk_text_view_new();
        gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

        curr = app_data.patients;
        while(curr) {
            char line[512];
            snprintf(line, sizeof(line), "姓名: %s, 年龄: %d, 诊断: %s\n", curr->name, curr->age, curr->diagnosis);
            GtkTextIter end;
            gtk_text_buffer_get_end_iter(buffer, &end);
            gtk_text_buffer_insert(buffer, &end, line, -1);
            curr = curr->next;
        }
        gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), text_view);
        gtk_box_append(GTK_BOX(box), scrolled);
    }

    return box;
}

GtkWidget* create_doctor_view() {
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_box_append(GTK_BOX(box), gtk_label_new("医生管理 (Doctor Management)"));

    GtkWidget *scrolled = gtk_scrolled_window_new();
    gtk_widget_set_vexpand(scrolled, TRUE);
    GtkWidget *text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));

    DoctorNode *curr = app_data.doctors;
    while(curr) {
        char line[512];
        snprintf(line, sizeof(line), "姓名: %s, 科室: %s, 电话: %s\n", curr->name, curr->department, curr->phone);
        GtkTextIter end;
        gtk_text_buffer_get_end_iter(buffer, &end);
        gtk_text_buffer_insert(buffer, &end, line, -1);
        curr = curr->next;
    }
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolled), text_view);
    gtk_box_append(GTK_BOX(box), scrolled);

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
