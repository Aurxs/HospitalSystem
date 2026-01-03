//
// Bill Management Tests
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "../include/bill.h"
#include "test.h"

int test_count_bill = 0;
int pass_count_bill = 0;

void test_make_bill() {
    printf("测试 make_bill 函数...\n");
    test_count_bill++;

    BillNode bill = make_bill("张三", "CT检查", 500.0);

    assert(strcmp(bill.patientName, "张三") == 0);
    assert(strcmp(bill.itemName, "CT检查") == 0);
    assert(fabs(bill.amount - 500.0) < 0.01);
    assert(bill.next == NULL);

    pass_count_bill++;
    printf("✓ make_bill 测试通过\n\n");
}

void test_add_bill() {
    printf("测试 add_bill 函数...\n");
    test_count_bill++;

    BillNode *head = NULL;

    // 添加第一个费用记录
    BillNode b1 = make_bill("李四", "血常规", 50.0);
    BillNode *result1 = add_bill(&head, b1);
    assert(result1 != NULL);
    assert(head != NULL);
    assert(strcmp(head->patientName, "李四") == 0);

    // 添加第二个费用记录
    BillNode b2 = make_bill("王五", "X光检查", 200.0);
    BillNode *result2 = add_bill(&head, b2);
    assert(result2 != NULL);
    assert(head->next != NULL);
    assert(strcmp(head->next->patientName, "王五") == 0);

    // 清理内存
    free_bill_list(head);

    pass_count_bill++;
    printf("✓ add_bill 测试通过\n\n");
}

void test_findBill_patient() {
    printf("测试 findBill_patient 函数...\n");
    test_count_bill++;

    BillNode *head = NULL;

    BillNode b1 = make_bill("赵六", "挂号费", 15.0);
    BillNode b2 = make_bill("孙七", "药费", 120.0);
    add_bill(&head, b1);
    add_bill(&head, b2);

    // 查找存在的费用记录
    BillNode *found = findBill_patient(head, "赵六");
    assert(found != NULL);
    assert(strcmp(found->patientName, "赵六") == 0);
    assert(strcmp(found->itemName, "挂号费") == 0);

    // 查找不存在的费用记录
    BillNode *notFound = findBill_patient(head, "不存在");
    assert(notFound == NULL);

    // 清理内存
    free_bill_list(head);

    pass_count_bill++;
    printf("✓ findBill_patient 测试通过\n\n");
}

void test_findBill_item() {
    printf("测试 findBill_item 函数...\n");
    test_count_bill++;

    BillNode *head = NULL;

    BillNode b1 = make_bill("周八", "MRI检查", 800.0);
    BillNode b2 = make_bill("吴九", "B超检查", 150.0);
    add_bill(&head, b1);
    add_bill(&head, b2);

    // 查找存在的费用记录
    BillNode *found = findBill_item(head, "MRI检查");
    assert(found != NULL);
    assert(strcmp(found->itemName, "MRI检查") == 0);
    assert(strcmp(found->patientName, "周八") == 0);

    // 查找不存在的费用记录
    BillNode *notFound = findBill_item(head, "不存在的项目");
    assert(notFound == NULL);

    // 清理内存
    free_bill_list(head);

    pass_count_bill++;
    printf("✓ findBill_item 测试通过\n\n");
}

void test_calculate_total_bill() {
    printf("测试 calculate_total_bill 函数...\n");
    test_count_bill++;

    BillNode *head = NULL;

    // 为同一个患者添加多个费用记录
    BillNode b1 = make_bill("郑十", "挂号费", 15.0);
    BillNode b2 = make_bill("郑十", "检查费", 200.0);
    BillNode b3 = make_bill("郑十", "药费", 85.0);
    BillNode b4 = make_bill("冯一", "挂号费", 20.0);  // 其他患者
    add_bill(&head, b1);
    add_bill(&head, b2);
    add_bill(&head, b3);
    add_bill(&head, b4);

    // 计算郑十的总费用
    double total = calculate_total_bill(head, "郑十");
    assert(fabs(total - 300.0) < 0.01);  // 15 + 200 + 85 = 300

    // 计算冯一的总费用
    total = calculate_total_bill(head, "冯一");
    assert(fabs(total - 20.0) < 0.01);

    // 计算不存在的患者的总费用
    total = calculate_total_bill(head, "不存在");
    assert(fabs(total - 0.0) < 0.01);

    // 清理内存
    free_bill_list(head);

    pass_count_bill++;
    printf("✓ calculate_total_bill 测试通过\n\n");
}

void test_modify_bill() {
    printf("测试 modify_bill 函数...\n");
    test_count_bill++;

    BillNode *head = NULL;

    BillNode b1 = make_bill("陈二", "手术费", 5000.0);
    add_bill(&head, b1);

    // 修改费用记录
    BillNode newInfo = make_bill("陈二", "手术费", 5500.0);
    BillNode *modified = modify_bill(head, "陈二", "手术费", newInfo);

    assert(modified != NULL);
    assert(strcmp(modified->patientName, "陈二") == 0);
    assert(fabs(modified->amount - 5500.0) < 0.01);

    // 尝试修改不存在的费用记录
    BillNode newInfo2 = make_bill("不存在", "不存在项目", 0);
    BillNode *notModified = modify_bill(head, "不存在", "不存在项目", newInfo2);
    assert(notModified == NULL);

    // 清理内存
    free_bill_list(head);

    pass_count_bill++;
    printf("✓ modify_bill 测试通过\n\n");
}

void test_delete_bill() {
    printf("测试 delete_bill 函数...\n");
    test_count_bill++;

    BillNode *head = NULL;

    BillNode b1 = make_bill("褚三", "费用A", 100.0);
    BillNode b2 = make_bill("卫四", "费用B", 200.0);
    BillNode b3 = make_bill("蒋五", "费用C", 300.0);
    add_bill(&head, b1);
    add_bill(&head, b2);
    add_bill(&head, b3);

    // 删除中间的费用记录
    head = delete_bill(head, "卫四", "费用B");
    BillNode *found = findBill_patient(head, "卫四");
    assert(found == NULL);

    // 删除第一个费用记录
    head = delete_bill(head, "褚三", "费用A");
    found = findBill_patient(head, "褚三");
    assert(found == NULL);

    // 验证剩余的费用记录仍然存在
    found = findBill_patient(head, "蒋五");
    assert(found != NULL);
    assert(strcmp(found->patientName, "蒋五") == 0);

    // 清理内存
    free_bill_list(head);

    pass_count_bill++;
    printf("✓ delete_bill 测试通过\n\n");
}

void test_sort_bills_by_amount() {
    printf("测试 sort_bills_by_amount 函数...\n");
    test_count_bill++;

    BillNode *head = NULL;
    BillNode b1 = make_bill("A", "I", 300.0);
    BillNode b2 = make_bill("B", "I", 100.0);
    BillNode b3 = make_bill("C", "I", 200.0);

    add_bill(&head, b1);
    add_bill(&head, b2);
    add_bill(&head, b3);

    head = sort_bills_by_amount(head);

    assert(head->amount == 100.0);
    assert(head->next->amount == 200.0);
    assert(head->next->next->amount == 300.0);

    // 清理内存
    free_bill_list(head);

    pass_count_bill++;
    printf("✓ sort_bills_by_amount 测试通过\n\n");
}

void run_bill_tests() {
    printf("========== 费用管理模块测试 ==========\n\n");

    test_make_bill();
    test_add_bill();
    test_findBill_patient();
    test_findBill_item();
    test_calculate_total_bill();
    test_modify_bill();
    test_delete_bill();
    test_sort_bills_by_amount();

    printf("========================================\n");
    printf("费用模块测试完成: %d/%d 通过\n", pass_count_bill, test_count_bill);
    printf("========================================\n\n");
}
