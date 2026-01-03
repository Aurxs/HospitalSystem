//
// Created by 罗金源 on 2025/12/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/doctor.h"
#include "test.h"

int test_count_doctor = 0;
int pass_count_doctor = 0;

void test_make_doctor() {
    printf("测试 make_doctor 函数...\n");
    test_count_doctor++;

    DoctorNode doctor = make_doctor("李医生", 40, "男", "内科", "13800138001");
    (void) doctor;

    assert(strcmp(doctor.name, "李医生") == 0);
    assert(doctor.age == 40);
    assert(strcmp(doctor.gender, "男") == 0);
    assert(strcmp(doctor.department, "内科") == 0);
    assert(strcmp(doctor.phone, "13800138001") == 0);
    assert(doctor.next == NULL);

    pass_count_doctor++;
    printf("✓ make_doctor 测试通过\n\n");
}

void test_add_doctor() {
    printf("测试 add_doctor 函数...\n");
    test_count_doctor++;

    DoctorNode *head = NULL;

    // 添加第一个医生
    DoctorNode d1 = make_doctor("王医生", 35, "女", "外科", "13900139001");
    DoctorNode *result1 = add_doctor(&head, d1);
    assert(result1 != NULL);
    assert(head != NULL);
    assert(strcmp(head->name, "王医生") == 0);

    // 添加第二个医生
    DoctorNode d2 = make_doctor("张医生", 45, "男", "儿科", "13700137001");
    DoctorNode *result2 = add_doctor(&head, d2);
    assert(result2 != NULL);
    assert(head->next != NULL);
    assert(strcmp(head->next->name, "张医生") == 0);

    // 清理内存
    while (head != NULL) {
        DoctorNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_doctor++;
    printf("✓ add_doctor 测试通过\n\n");
}

void test_findDoctor_name() {
    printf("测试 findDoctor_name 函数...\n");
    test_count_doctor++;

    DoctorNode *head = NULL;

    DoctorNode d1 = make_doctor("赵医生", 38, "男", "心内科", "13600136001");
    DoctorNode d2 = make_doctor("孙医生", 32, "女", "妇产科", "13500135001");
    add_doctor(&head, d1);
    add_doctor(&head, d2);

    // 查找存在的医生
    DoctorNode *found = findDoctor_name(head, "赵医生");
    assert(found != NULL);
    assert(strcmp(found->name, "赵医生") == 0);
    assert(found->age == 38);
    assert(strcmp(found->department, "心内科") == 0);

    // 查找不存在的医生
    DoctorNode *notFound = findDoctor_name(head, "不存在医生");
    assert(notFound == NULL);

    // 清理内存
    while (head != NULL) {
        DoctorNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_doctor++;
    printf("✓ findDoctor_name 测试通过\n\n");
}

void test_findDoctor_phone() {
    printf("测试 findDoctor_phone 函数...\n");
    test_count_doctor++;

    DoctorNode *head = NULL;

    DoctorNode d1 = make_doctor("周医生", 50, "男", "骨科", "13400134001");
    DoctorNode d2 = make_doctor("吴医生", 29, "女", "眼科", "13300133001");
    add_doctor(&head, d1);
    add_doctor(&head, d2);

    // 查找存在的医生
    DoctorNode *found = findDoctor_phone(head, "13400134001");
    assert(found != NULL);
    assert(strcmp(found->name, "周医生") == 0);
    assert(strcmp(found->phone, "13400134001") == 0);
    assert(strcmp(found->department, "骨科") == 0);

    // 查找不存在的医生
    DoctorNode *notFound = findDoctor_phone(head, "99999999999");
    assert(notFound == NULL);

    // 清理内存
    while (head != NULL) {
        DoctorNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_doctor++;
    printf("✓ findDoctor_phone 测试通过\n\n");
}

void test_modify_doctor() {
    printf("测试 modify_doctor 函数...\n");
    test_count_doctor++;

    DoctorNode *head = NULL;

    DoctorNode d1 = make_doctor("郑医生", 42, "男", "神经科", "13200132001");
    add_doctor(&head, d1);

    // 修改医生信息
    DoctorNode newInfo = make_doctor("郑医生", 43, "男", "神经外科", "13200132001");
    DoctorNode *modified = modify_doctor(head, "13200132001", newInfo);

    assert(modified != NULL);
    assert(strcmp(modified->name, "郑医生") == 0);
    assert(modified->age == 43);
    assert(strcmp(modified->department, "神经外科") == 0);

    // 尝试修改不存在的医生
    DoctorNode newInfo2 = make_doctor("test", 30, "男", "测试科", "99999999999");
    DoctorNode *notModified = modify_doctor(head, "99999999999", newInfo2);
    assert(notModified == NULL);

    // 清理内存
    while (head != NULL) {
        DoctorNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_doctor++;
    printf("✓ modify_doctor 测试通过\n\n");
}

void test_delete_doctor() {
    printf("测试 delete_doctor 函数...\n");
    test_count_doctor++;

    DoctorNode *head = NULL;

    DoctorNode d1 = make_doctor("冯医生", 36, "女", "皮肤科", "13100131001");
    DoctorNode d2 = make_doctor("陈医生", 44, "男", "泌尿科", "13000130001");
    DoctorNode d3 = make_doctor("褚医生", 52, "女", "肿瘤科", "12900129001");
    add_doctor(&head, d1);
    add_doctor(&head, d2);
    add_doctor(&head, d3);

    // 删除中间的医生
    head = delete_doctor(head, "13000130001");
    DoctorNode *found = findDoctor_phone(head, "13000130001");
    assert(found == NULL);

    // 删除第一个医生
    head = delete_doctor(head, "13100131001");
    found = findDoctor_phone(head, "13100131001");
    assert(found == NULL);

    // 验证剩余的医生仍然存在
    found = findDoctor_phone(head, "12900129001");
    assert(found != NULL);
    assert(strcmp(found->name, "褚医生") == 0);

    // 清理内存
    while (head != NULL) {
        DoctorNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_doctor++;
    printf("✓ delete_doctor 测试通过\n\n");
}

void test_sort_doctors_by_name() {
    printf("测试 sort_doctors_by_name 函数...\n");
    test_count_doctor++;

    DoctorNode *head = NULL;
    DoctorNode d1 = make_doctor("C", 30, "M", "D", "139");
    DoctorNode d2 = make_doctor("A", 31, "M", "D", "138");
    DoctorNode d3 = make_doctor("B", 32, "M", "D", "137");

    add_doctor(&head, d1);
    add_doctor(&head, d2);
    add_doctor(&head, d3);

    head = sort_doctors_by_name(head);

    assert(strcmp(head->name, "A") == 0);
    assert(strcmp(head->next->name, "B") == 0);
    assert(strcmp(head->next->next->name, "C") == 0);

    // 清理内存
    while (head != NULL) {
        DoctorNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_doctor++;
    printf("✓ sort_doctors_by_name 测试通过\n\n");
}

void test_sort_doctors_by_phone() {
    printf("测试 sort_doctors_by_phone 函数...\n");
    test_count_doctor++;

    DoctorNode *head = NULL;
    DoctorNode d1 = make_doctor("A", 30, "M", "D", "139");
    DoctorNode d2 = make_doctor("B", 31, "M", "D", "137");
    DoctorNode d3 = make_doctor("C", 32, "M", "D", "138");

    add_doctor(&head, d1);
    add_doctor(&head, d2);
    add_doctor(&head, d3);

    head = sort_doctors_by_phone(head);

    assert(strcmp(head->phone, "137") == 0);
    assert(strcmp(head->next->phone, "138") == 0);
    assert(strcmp(head->next->next->phone, "139") == 0);

    // 清理内存
    while (head != NULL) {
        DoctorNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_doctor++;
    printf("✓ sort_doctors_by_phone 测试通过\n\n");
}

void test_sort_doctors_by_age() {
    printf("测试 sort_doctors_by_age 函数...\n");
    test_count_doctor++;

    DoctorNode *head = NULL;
    DoctorNode d1 = make_doctor("A", 32, "M", "D", "139");
    DoctorNode d2 = make_doctor("B", 30, "M", "D", "138");
    DoctorNode d3 = make_doctor("C", 31, "M", "D", "137");

    add_doctor(&head, d1);
    add_doctor(&head, d2);
    add_doctor(&head, d3);

    head = sort_doctors_by_age(head);

    assert(head->age == 30);
    assert(head->next->age == 31);
    assert(head->next->next->age == 32);

    // 清理内存
    while (head != NULL) {
        DoctorNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_doctor++;
    printf("✓ sort_doctors_by_age 测试通过\n\n");
}

void run_doctor_tests() {
    printf("========== 医生管理模块测试 ==========\n\n");

    test_make_doctor();
    test_add_doctor();
    test_findDoctor_name();
    test_findDoctor_phone();
    test_modify_doctor();
    test_delete_doctor();
    test_sort_doctors_by_name();
    test_sort_doctors_by_phone();
    test_sort_doctors_by_age();

    printf("========================================\n");
    printf("医生模块测试完成: %d/%d 通过\n", pass_count_doctor, test_count_doctor);
    printf("========================================\n\n");
}