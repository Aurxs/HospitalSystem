//
// Created by 罗金源 on 2025/12/19.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/patient.h"
#include "test.h"

int test_count_patient = 0;
int pass_count_patient = 0;

void test_make_patient() {
    printf("测试 make_patient 函数...\n");
    test_count_patient++;

    PatientNode patient = make_patient("张三", 30, "男", "13800138000", "感冒", "多休息");
    (void) patient; // Suppress unused variable warning in Release builds

    assert(strcmp(patient.name, "张三") == 0);
    assert(patient.age == 30);
    assert(strcmp(patient.gender, "男") == 0);
    assert(strcmp(patient.phone, "13800138000") == 0);
    assert(strcmp(patient.diagnosis, "感冒") == 0);
    assert(strcmp(patient.treatment, "多休息") == 0);
    assert(patient.next == NULL);

    pass_count_patient++;
    printf("✓ make_patient 测试通过\n\n");
}

void test_add_patient() {
    printf("测试 add_patient 函数...\n");
    test_count_patient++;

    PatientNode *head = NULL;

    // 添加第一个患者
    PatientNode p1 = make_patient("李四", 25, "女", "13900139000", "发烧", "退烧药");
    PatientNode *result1 = add_patient(&head, p1);
    assert(result1 != NULL);
    assert(head != NULL);
    assert(strcmp(head->name, "李四") == 0);

    // 添加第二个患者
    PatientNode p2 = make_patient("王五", 35, "男", "13700137000", "咳嗽", "止咳糖浆");
    PatientNode *result2 = add_patient(&head, p2);
    assert(result2 != NULL);
    assert(head->next != NULL);
    assert(strcmp(head->next->name, "王五") == 0);

    // 清理内存
    while (head != NULL) {
        PatientNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_patient++;
    printf("✓ add_patient 测试通过\n\n");
}

void test_findPatient_name() {
    printf("测试 findPatient_name 函数...\n");
    test_count_patient++;

    PatientNode *head = NULL;

    PatientNode p1 = make_patient("赵六", 40, "男", "13600136000", "头痛", "止痛药");
    PatientNode p2 = make_patient("孙七", 28, "女", "13500135000", "胃痛", "胃药");
    add_patient(&head, p1);
    add_patient(&head, p2);

    // 查找存在的患者
    PatientNode *found = findPatient_name(head, "赵六");
    assert(found != NULL);
    assert(strcmp(found->name, "赵六") == 0);
    assert(found->age == 40);

    // 查找不存在的患者
    PatientNode *notFound = findPatient_name(head, "不存在");
    assert(notFound == NULL);

    // 清理内存
    while (head != NULL) {
        PatientNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_patient++;
    printf("✓ findPatient_name 测试通过\n\n");
}

void test_findPatient_phone() {
    printf("测试 findPatient_phone 函数...\n");
    test_count_patient++;

    PatientNode *head = NULL;

    PatientNode p1 = make_patient("周八", 50, "男", "13400134000", "高血压", "降压药");
    PatientNode p2 = make_patient("吴九", 22, "女", "13300133000", "过敏", "抗过敏药");
    add_patient(&head, p1);
    add_patient(&head, p2);

    // 查找存在的患者
    PatientNode *found = findPatient_phone(head, "13400134000");
    assert(found != NULL);
    assert(strcmp(found->name, "周八") == 0);
    assert(strcmp(found->phone, "13400134000") == 0);

    // 查找不存在的患者
    PatientNode *notFound = findPatient_phone(head, "99999999999");
    assert(notFound == NULL);

    // 清理内存
    while (head != NULL) {
        PatientNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_patient++;
    printf("✓ findPatient_phone 测试通过\n\n");
}

void test_modify_patient() {
    printf("测试 modify_patient 函数...\n");
    test_count_patient++;

    PatientNode *head = NULL;

    PatientNode p1 = make_patient("郑十", 45, "男", "13200132000", "糖尿病", "控制饮食");
    add_patient(&head, p1);

    // 修改患者信息
    PatientNode newInfo = make_patient("郑十一", 46, "男", "13200132000", "糖尿病稳定", "继续控制饮食");
    PatientNode *modified = modify_patient(head, "13200132000", newInfo);

    assert(modified != NULL);
    assert(strcmp(modified->name, "郑十一") == 0);
    assert(modified->age == 46);
    assert(strcmp(modified->diagnosis, "糖尿病稳定") == 0);

    // 尝试修改不存在的患者
    PatientNode newInfo2 = make_patient("test", 30, "男", "99999999999", "test", "test");
    PatientNode *notModified = modify_patient(head, "99999999999", newInfo2);
    assert(notModified == NULL);

    // 清理内存
    while (head != NULL) {
        PatientNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_patient++;
    printf("✓ modify_patient 测试通过\n\n");
}

void test_delete_patient() {
    printf("测试 delete_patient 函数...\n");
    test_count_patient++;

    PatientNode *head = NULL;

    PatientNode p1 = make_patient("冯一", 33, "女", "13100131000", "骨折", "打石膏");
    PatientNode p2 = make_patient("陈二", 27, "男", "13000130000", "扭伤", "冰敷");
    PatientNode p3 = make_patient("褚三", 55, "女", "12900129000", "心脏病", "药物治疗");
    add_patient(&head, p1);
    add_patient(&head, p2);
    add_patient(&head, p3);

    // 删除中间的患者
    head = delete_patient(head, "13000130000");
    PatientNode *found = findPatient_phone(head, "13000130000");
    assert(found == NULL);

    // 删除第一个患者
    head = delete_patient(head, "13100131000");
    found = findPatient_phone(head, "13100131000");
    assert(found == NULL);

    // 验证剩余的患者仍然存在
    found = findPatient_phone(head, "12900129000");
    assert(found != NULL);
    assert(strcmp(found->name, "褚三") == 0);

    // 清理内存
    while (head != NULL) {
        PatientNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_patient++;
    printf("✓ delete_patient 测试通过\n\n");
}

void test_sort_patients_by_phone() {
    printf("测试 sort_patients_by_phone 函数...\n");
    test_count_patient++;

    PatientNode *head = NULL;
    PatientNode p1 = make_patient("A", 20, "M", "139", "D", "T");
    PatientNode p2 = make_patient("B", 21, "M", "138", "D", "T");
    PatientNode p3 = make_patient("C", 22, "M", "137", "D", "T");

    add_patient(&head, p1);
    add_patient(&head, p2);
    add_patient(&head, p3);

    head = sort_patients_by_phone(head);

    assert(strcmp(head->phone, "137") == 0);
    assert(strcmp(head->next->phone, "138") == 0);
    assert(strcmp(head->next->next->phone, "139") == 0);

    // 清理内存
    while (head != NULL) {
        PatientNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_patient++;
    printf("✓ sort_patients_by_phone 测试通过\n\n");
}

void test_sort_patients_by_age() {
    printf("测试 sort_patients_by_age 函数...\n");
    test_count_patient++;

    PatientNode *head = NULL;
    PatientNode p1 = make_patient("A", 30, "M", "139", "D", "T");
    PatientNode p2 = make_patient("B", 20, "M", "138", "D", "T");
    PatientNode p3 = make_patient("C", 25, "M", "137", "D", "T");

    add_patient(&head, p1);
    add_patient(&head, p2);
    add_patient(&head, p3);

    head = sort_patients_by_age(head);

    assert(head->age == 20);
    assert(head->next->age == 25);
    assert(head->next->next->age == 30);

    // 清理内存
    while (head != NULL) {
        PatientNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_patient++;
    printf("✓ sort_patients_by_age 测试通过\n\n");
}

void run_patient_tests() {
    printf("========== 患者管理模块测试 ==========\n\n");

    test_make_patient();
    test_add_patient();
    test_findPatient_name();
    test_findPatient_phone();
    test_modify_patient();
    test_delete_patient();
    test_sort_patients_by_phone();
    test_sort_patients_by_age();

    printf("========================================\n");
    printf("患者模块测试完成: %d/%d 通过\n", pass_count_patient, test_count_patient);
    printf("========================================\n\n");
}