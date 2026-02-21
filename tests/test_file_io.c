//
// File I/O Tests
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "../include/file_io.h"
#include "../include/patient.h"
#include "../include/doctor.h"
#include "../include/drug.h"
#include "../include/registration.h"
#include "../include/bill.h"
#include "../include/auth.h"
#include "test.h"

int test_count_file_io = 0;
int pass_count_file_io = 0;

// 临时文件名定义
#define TEST_PATIENT_FILE "/tmp/test_patients.dat"
#define TEST_DOCTOR_FILE "/tmp/test_doctors.dat"
#define TEST_DRUG_FILE "/tmp/test_drugs.dat"
#define TEST_REGISTRATION_FILE "/tmp/test_registrations.dat"
#define TEST_BILL_FILE "/tmp/test_bills.dat"
#define TEST_AUTH_FILE "/tmp/test_auth.dat"

void test_save_load_patients() {
    printf("测试 save_patients/load_patients 函数...\n");
    test_count_file_io++;

    // 创建患者链表
    PatientNode *head = NULL;
    PatientNode p1 = make_patient("张三", 30, "男", "13800138000", "感冒", "多休息");
    PatientNode p2 = make_patient("李四", 25, "女", "13900139000", "发烧", "退烧药");
    add_patient(&head, p1);
    add_patient(&head, p2);

    // 保存到文件
    int result = save_patients(TEST_PATIENT_FILE, head);
    assert(result == 1);

    // 从文件读取
    PatientNode *loadedHead = load_patients(TEST_PATIENT_FILE);
    assert(loadedHead != NULL);

    // 验证数据
    assert(strcmp(loadedHead->name, "张三") == 0);
    assert(loadedHead->age == 30);
    assert(strcmp(loadedHead->gender, "男") == 0);
    assert(strcmp(loadedHead->phone, "13800138000") == 0);

    assert(loadedHead->next != NULL);
    assert(strcmp(loadedHead->next->name, "李四") == 0);
    assert(loadedHead->next->age == 25);

    // 清理内存
    while (head != NULL) {
        PatientNode *temp = head;
        head = head->next;
        free(temp);
    }
    while (loadedHead != NULL) {
        PatientNode *temp = loadedHead;
        loadedHead = loadedHead->next;
        free(temp);
    }

    // 删除测试文件
    remove(TEST_PATIENT_FILE);

    pass_count_file_io++;
    printf("✓ save_patients/load_patients 测试通过\n\n");
}

void test_save_load_doctors() {
    printf("测试 save_doctors/load_doctors 函数...\n");
    test_count_file_io++;

    // 创建医生链表
    DoctorNode *head = NULL;
    DoctorNode d1 = make_doctor("王医生", 40, "男", "内科", "13800138001", "周一上午");
    DoctorNode d2 = make_doctor("张医生", 35, "女", "外科", "13900139001", "周三全天");
    add_doctor(&head, d1);
    add_doctor(&head, d2);

    // 保存到文件
    int result = save_doctors(TEST_DOCTOR_FILE, head);
    assert(result == 1);

    // 从文件读取
    DoctorNode *loadedHead = load_doctors(TEST_DOCTOR_FILE);
    assert(loadedHead != NULL);

    // 验证数据
    assert(strcmp(loadedHead->name, "王医生") == 0);
    assert(loadedHead->age == 40);
    assert(strcmp(loadedHead->department, "内科") == 0);
    assert(strcmp(loadedHead->schedule, "周一上午") == 0);

    assert(loadedHead->next != NULL);
    assert(strcmp(loadedHead->next->name, "张医生") == 0);
    assert(strcmp(loadedHead->next->department, "外科") == 0);
    assert(strcmp(loadedHead->next->schedule, "周三全天") == 0);

    // 清理内存
    while (head != NULL) {
        DoctorNode *temp = head;
        head = head->next;
        free(temp);
    }
    while (loadedHead != NULL) {
        DoctorNode *temp = loadedHead;
        loadedHead = loadedHead->next;
        free(temp);
    }

    // 删除测试文件
    remove(TEST_DOCTOR_FILE);

    pass_count_file_io++;
    printf("✓ save_doctors/load_doctors 测试通过\n\n");
}

void test_save_load_drugs() {
    printf("测试 save_drugs/load_drugs 函数...\n");
    test_count_file_io++;

    // 创建药品链表
    DrugNode *head = NULL;
    DrugNode drug1 = make_drug("阿莫西林", "0.25g*24s", "白云山制药", 15.5, 100);
    DrugNode drug2 = make_drug("布洛芬", "0.1g*100片", "新华制药", 25.0, 500);
    add_drug(&head, drug1);
    add_drug(&head, drug2);

    // 保存到文件
    int result = save_drugs(TEST_DRUG_FILE, head);
    assert(result == 1);

    // 从文件读取
    DrugNode *loadedHead = load_drugs(TEST_DRUG_FILE);
    assert(loadedHead != NULL);

    // 验证数据
    assert(strcmp(loadedHead->name, "阿莫西林") == 0);
    assert(fabs(loadedHead->price - 15.5) < 0.01);
    assert(loadedHead->stock == 100);

    assert(loadedHead->next != NULL);
    assert(strcmp(loadedHead->next->name, "布洛芬") == 0);
    assert(fabs(loadedHead->next->price - 25.0) < 0.01);

    // 清理内存
    while (head != NULL) {
        DrugNode *temp = head;
        head = head->next;
        free(temp);
    }
    while (loadedHead != NULL) {
        DrugNode *temp = loadedHead;
        loadedHead = loadedHead->next;
        free(temp);
    }

    // 删除测试文件
    remove(TEST_DRUG_FILE);

    pass_count_file_io++;
    printf("✓ save_drugs/load_drugs 测试通过\n\n");
}

void test_save_load_registrations() {
    printf("测试 save_registrations/load_registrations 函数...\n");
    test_count_file_io++;

    // 创建挂号记录链表
    RegisterNode *head = NULL;
    RegisterNode r1 = make_registration("张三", "王医生", "内科", "2023-10-01");
    RegisterNode r2 = make_registration("李四", "张医生", "外科", "2023-10-02");
    add_registration(&head, r1);
    add_registration(&head, r2);

    // 保存到文件
    int result = save_registrations(TEST_REGISTRATION_FILE, head);
    assert(result == 1);

    // 从文件读取
    RegisterNode *loadedHead = load_registrations(TEST_REGISTRATION_FILE);
    assert(loadedHead != NULL);

    // 验证数据
    assert(strcmp(loadedHead->patientName, "张三") == 0);
    assert(strcmp(loadedHead->doctorName, "王医生") == 0);
    assert(strcmp(loadedHead->department, "内科") == 0);
    assert(strcmp(loadedHead->date, "2023-10-01") == 0);

    assert(loadedHead->next != NULL);
    assert(strcmp(loadedHead->next->patientName, "李四") == 0);
    assert(strcmp(loadedHead->next->doctorName, "张医生") == 0);

    // 清理内存
    free_registration_list(head);
    free_registration_list(loadedHead);

    // 删除测试文件
    remove(TEST_REGISTRATION_FILE);

    pass_count_file_io++;
    printf("✓ save_registrations/load_registrations 测试通过\n\n");
}

void test_save_load_bills() {
    printf("测试 save_bills/load_bills 函数...\n");
    test_count_file_io++;

    // 创建费用记录链表
    BillNode *head = NULL;
    BillNode b1 = make_bill("张三", "挂号费", 15.0);
    BillNode b2 = make_bill("张三", "检查费", 200.0);
    BillNode b3 = make_bill("李四", "药费", 85.0);
    add_bill(&head, b1);
    add_bill(&head, b2);
    add_bill(&head, b3);

    // 保存到文件
    int result = save_bills(TEST_BILL_FILE, head);
    assert(result == 1);

    // 从文件读取
    BillNode *loadedHead = load_bills(TEST_BILL_FILE);
    assert(loadedHead != NULL);

    // 验证数据
    assert(strcmp(loadedHead->patientName, "张三") == 0);
    assert(strcmp(loadedHead->itemName, "挂号费") == 0);
    assert(fabs(loadedHead->amount - 15.0) < 0.01);

    assert(loadedHead->next != NULL);
    assert(strcmp(loadedHead->next->itemName, "检查费") == 0);

    assert(loadedHead->next->next != NULL);
    assert(strcmp(loadedHead->next->next->patientName, "李四") == 0);

    // 清理内存
    free_bill_list(head);
    free_bill_list(loadedHead);

    // 删除测试文件
    remove(TEST_BILL_FILE);

    pass_count_file_io++;
    printf("✓ save_bills/load_bills 测试通过\n\n");
}

void test_load_bills_invalid_amount() {
    printf("测试 load_bills 非法金额过滤...\n");
    test_count_file_io++;

    FILE *fp = fopen(TEST_BILL_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "张三|挂号费|-10\n");
    fprintf(fp, "李四|检查费|abc\n");
    fprintf(fp, "王五|药费|88.5\n");
    fclose(fp);

    BillNode *loadedHead = load_bills(TEST_BILL_FILE);
    assert(loadedHead != NULL);
    assert(strcmp(loadedHead->patientName, "王五") == 0);
    assert(fabs(loadedHead->amount - 88.5) < 0.01);
    assert(loadedHead->next == NULL);

    free_bill_list(loadedHead);
    remove(TEST_BILL_FILE);

    pass_count_file_io++;
    printf("✓ load_bills 非法金额过滤测试通过\n\n");
}

void test_load_nonexistent_file() {
    printf("测试 加载不存在的文件...\n");
    test_count_file_io++;

    // 尝试加载不存在的文件
    PatientNode *patients = load_patients("/tmp/nonexistent_file.dat");
    assert(patients == NULL);

    DoctorNode *doctors = load_doctors("/tmp/nonexistent_file.dat");
    assert(doctors == NULL);

    DrugNode *drugs = load_drugs("/tmp/nonexistent_file.dat");
    assert(drugs == NULL);

    RegisterNode *registrations = load_registrations("/tmp/nonexistent_file.dat");
    assert(registrations == NULL);

    BillNode *bills = load_bills("/tmp/nonexistent_file.dat");
    assert(bills == NULL);

    pass_count_file_io++;
    printf("✓ 加载不存在的文件 测试通过\n\n");
}

void test_save_load_users() {
    printf("测试 save_users/load_users 函数...\n");
    test_count_file_io++;

    // 创建用户链表
    AuthNode *head = NULL;
    AuthNode u1 = make_user("admin", "admin123", 0);
    AuthNode u2 = make_user("guest", "guest123", 2);
    add_user(&head, u1);
    add_user(&head, u2);

    // 保存到文件
    int result = save_users(TEST_AUTH_FILE, head);
    assert(result == 1);

    // 从文件读取
    AuthNode *loadedHead = load_users(TEST_AUTH_FILE);
    assert(loadedHead != NULL);

    // 验证数据
    AuthNode *found = find_user(loadedHead, "admin");
    assert(found != NULL);
    assert(authenticate_user(loadedHead, "admin", "admin123") == 1);

    found = find_user(loadedHead, "guest");
    assert(found != NULL);
    assert(authenticate_user(loadedHead, "guest", "guest123") == 1);

    // 清理内存
    while (head != NULL) {
        AuthNode *temp = head;
        head = head->next;
        free(temp);
    }
    while (loadedHead != NULL) {
        AuthNode *temp = loadedHead;
        loadedHead = loadedHead->next;
        free(temp);
    }

    // 删除测试文件
    remove(TEST_AUTH_FILE);

    pass_count_file_io++;
    printf("✓ save_users/load_users 测试通过\n\n");
}

void test_load_users_invalid_role() {
    printf("测试 load_users 非法角色降级...\n");
    test_count_file_io++;

    FILE *fp = fopen(TEST_AUTH_FILE, "w");
    assert(fp != NULL);
    fprintf(fp, "hacker|h2$1234567890abcdef1234567890abcdef|99\n");
    fprintf(fp, "unknown|h2$aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa|abc\n");
    fprintf(fp, "doctor|h2$bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb|1\n");
    fclose(fp);

    AuthNode *loadedHead = load_users(TEST_AUTH_FILE);
    assert(loadedHead != NULL);

    AuthNode *hacker = find_user(loadedHead, "hacker");
    AuthNode *unknown = find_user(loadedHead, "unknown");
    AuthNode *doctor = find_user(loadedHead, "doctor");
    assert(hacker != NULL && hacker->role == 2);
    assert(unknown != NULL && unknown->role == 2);
    assert(doctor != NULL && doctor->role == 1);

    while (loadedHead != NULL) {
        AuthNode *temp = loadedHead;
        loadedHead = loadedHead->next;
        free(temp);
    }

    remove(TEST_AUTH_FILE);

    pass_count_file_io++;
    printf("✓ load_users 非法角色降级测试通过\n\n");
}

void run_file_io_tests() {
    printf("========== 文件读写模块测试 ==========\n\n");

    test_save_load_patients();
    test_save_load_doctors();
    test_save_load_drugs();
    test_save_load_registrations();
    test_save_load_bills();
    test_load_bills_invalid_amount();
    test_save_load_users();
    test_load_users_invalid_role();
    test_load_nonexistent_file();

    printf("========================================\n");
    printf("文件读写模块测试完成: %d/%d 通过\n", pass_count_file_io, test_count_file_io);
    printf("========================================\n\n");
}