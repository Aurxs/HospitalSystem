#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/drug.h"
#include "test.h"

int test_count_drug = 0;
int pass_count_drug = 0;

void test_make_drug() {
    printf("测试 make_drug 函数...\n");
    test_count_drug++;

    DrugNode drug = make_drug("阿莫西林", "0.25g*24s", "白云山制药", 15.5, 100);

    assert(strcmp(drug.name, "阿莫西林") == 0);
    assert(strcmp(drug.spec, "0.25g*24s") == 0);
    assert(strcmp(drug.factory, "白云山制药") == 0);
    assert(drug.price == 15.5);
    assert(drug.stock == 100);
    assert(drug.next == NULL);

    pass_count_drug++;
    printf("✓ make_drug 测试通过\n\n");
}

void test_add_drug() {
    printf("测试 add_drug 函数...\n");
    test_count_drug++;

    DrugNode *head = NULL;

    // 添加第一个药品
    DrugNode d1 = make_drug("布洛芬", "0.1g*100片", "新华制药", 25.0, 500);
    DrugNode *result1 = add_drug(&head, d1);
    assert(result1 != NULL);
    assert(head != NULL);
    assert(strcmp(head->name, "布洛芬") == 0);

    // 添加第二个药品
    DrugNode d2 = make_drug("头孢拉定", "0.25g*12粒", "华北制药", 12.0, 200);
    DrugNode *result2 = add_drug(&head, d2);
    assert(result2 != NULL);
    assert(head->next != NULL);
    assert(strcmp(head->next->name, "头孢拉定") == 0);

    // 清理内存
    while (head != NULL) {
        DrugNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_drug++;
    printf("✓ add_drug 测试通过\n\n");
}

void test_findDrug_name() {
    printf("测试 findDrug_name 函数...\n");
    test_count_drug++;

    DrugNode *head = NULL;

    DrugNode d1 = make_drug("感冒灵颗粒", "10g*9袋", "华润三九", 15.0, 100);
    DrugNode d2 = make_drug("连花清瘟胶囊", "0.35g*24粒", "以岭药业", 20.0, 300);
    add_drug(&head, d1);
    add_drug(&head, d2);

    // 查找存在的药品
    DrugNode *found = findDrug_name(head, "感冒灵颗粒");
    assert(found != NULL);
    assert(strcmp(found->name, "感冒灵颗粒") == 0);
    assert(found->price == 15.0);

    // 查找不存在的药品
    DrugNode *notFound = findDrug_name(head, "不存在的药");
    assert(notFound == NULL);

    // 清理内存
    while (head != NULL) {
        DrugNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_drug++;
    printf("✓ findDrug_name 测试通过\n\n");
}

void test_modify_drug() {
    printf("测试 modify_drug 函数...\n");
    test_count_drug++;

    DrugNode *head = NULL;

    DrugNode d1 = make_drug("阿司匹林", "100mg*30片", "拜耳医药", 18.0, 50);
    add_drug(&head, d1);

    // 修改药品信息
    DrugNode newInfo = make_drug("阿司匹林肠溶片", "100mg*30片", "拜耳医药", 19.5, 60);
    DrugNode *modified = modify_drug(head, "阿司匹林", newInfo);

    assert(modified != NULL);
    assert(strcmp(modified->name, "阿司匹林肠溶片") == 0);
    assert(modified->price == 19.5);
    assert(modified->stock == 60);

    // 尝试修改不存在的药品
    DrugNode newInfo2 = make_drug("未知药", "N/A", "N/A", 0, 0);
    DrugNode *notModified = modify_drug(head, "不存在的药", newInfo2);
    assert(notModified == NULL);

    // 清理内存
    while (head != NULL) {
        DrugNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_drug++;
    printf("✓ modify_drug 测试通过\n\n");
}

void test_delete_drug() {
    printf("测试 delete_drug 函数...\n");
    test_count_drug++;

    DrugNode *head = NULL;

    DrugNode d1 = make_drug("药A", "S1", "F1", 10.0, 10);
    DrugNode d2 = make_drug("药B", "S2", "F2", 20.0, 20);
    DrugNode d3 = make_drug("药C", "S3", "F3", 30.0, 30);
    add_drug(&head, d1);
    add_drug(&head, d2);
    add_drug(&head, d3);

    // 删除中间的药品
    head = delete_drug(head, "药B");
    DrugNode *found = findDrug_name(head, "药B");
    assert(found == NULL);

    // 删除第一个药品
    head = delete_drug(head, "药A");
    found = findDrug_name(head, "药A");
    assert(found == NULL);

    // 验证剩余的药品仍然存在
    found = findDrug_name(head, "药C");
    assert(found != NULL);
    assert(strcmp(found->name, "药C") == 0);

    // 清理内存
    while (head != NULL) {
        DrugNode *temp = head;
        head = head->next;
        free(temp);
    }

    pass_count_drug++;
    printf("✓ delete_drug 测试通过\n\n");
}

void run_drug_tests() {
    printf("========== 药品管理模块测试 ==========\n\n");

    test_make_drug();
    test_add_drug();
    test_findDrug_name();
    test_modify_drug();
    test_delete_drug();

    printf("========================================\n");
    printf("药品模块测试完成: %d/%d 通过\n", pass_count_drug, test_count_drug);
    printf("========================================\n\n");
}
