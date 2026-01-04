//
// Registration Management Tests
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/registration.h"
#include "test.h"

int test_count_registration = 0;
int pass_count_registration = 0;

void test_make_registration() {
    printf("测试 make_registration 函数...\n");
    test_count_registration++;

    RegisterNode reg = make_registration("R001", "P001", "张三", "李医生", "内科", "2023-10-01");
    (void) reg;

    assert(strcmp(reg.patientName, "张三") == 0);
    assert(strcmp(reg.doctorName, "李医生") == 0);
    assert(strcmp(reg.department, "内科") == 0);
    assert(strcmp(reg.date, "2023-10-01") == 0);
    assert(reg.next == NULL);

    pass_count_registration++;
    printf("✓ make_registration 测试通过\n\n");
}

void test_add_registration() {
    printf("测试 add_registration 函数...\n");
    test_count_registration++;

    RegisterNode *head = NULL;

    // 添加第一个挂号记录
    RegisterNode r1 = make_registration("R001", "P001", "李四", "王医生", "外科", "2023-10-02");
    RegisterNode *result1 = add_registration(&head, r1);
    assert(result1 != NULL);
    assert(head != NULL);
    assert(strcmp(head->patientName, "李四") == 0);

    // 添加第二个挂号记录
    RegisterNode r2 = make_registration("R001", "P001", "王五", "张医生", "儿科", "2023-10-03");
    RegisterNode *result2 = add_registration(&head, r2);
    assert(result2 != NULL);
    assert(head->next != NULL);
    assert(strcmp(head->next->patientName, "王五") == 0);

    // 清理内存
    free_registration_list(head);

    pass_count_registration++;
    printf("✓ add_registration 测试通过\n\n");
}

void test_findRegistration_patient() {
    printf("测试 findRegistration_patient 函数...\n");
    test_count_registration++;

    RegisterNode *head = NULL;

    RegisterNode r1 = make_registration("R001", "P001", "赵六", "孙医生", "心内科", "2023-10-04");
    RegisterNode r2 = make_registration("R001", "P001", "孙七", "周医生", "妇产科", "2023-10-05");
    add_registration(&head, r1);
    add_registration(&head, r2);

    // 查找存在的挂号记录
    RegisterNode *found = findRegistration_patient(head, "赵六");
    assert(found != NULL);
    assert(strcmp(found->patientName, "赵六") == 0);
    assert(strcmp(found->doctorName, "孙医生") == 0);

    // 查找不存在的挂号记录
    RegisterNode *notFound = findRegistration_patient(head, "不存在");
    assert(notFound == NULL);

    // 清理内存
    free_registration_list(head);

    pass_count_registration++;
    printf("✓ findRegistration_patient 测试通过\n\n");
}

void test_findRegistration_doctor() {
    printf("测试 findRegistration_doctor 函数...\n");
    test_count_registration++;

    RegisterNode *head = NULL;

    RegisterNode r1 = make_registration("R001", "P001", "周八", "吴医生", "骨科", "2023-10-06");
    RegisterNode r2 = make_registration("R001", "P001", "吴九", "郑医生", "眼科", "2023-10-07");
    add_registration(&head, r1);
    add_registration(&head, r2);

    // 查找存在的挂号记录
    RegisterNode *found = findRegistration_doctor(head, "吴医生");
    assert(found != NULL);
    assert(strcmp(found->doctorName, "吴医生") == 0);
    assert(strcmp(found->patientName, "周八") == 0);

    // 查找不存在的挂号记录
    RegisterNode *notFound = findRegistration_doctor(head, "不存在医生");
    assert(notFound == NULL);

    // 清理内存
    free_registration_list(head);

    pass_count_registration++;
    printf("✓ findRegistration_doctor 测试通过\n\n");
}

void test_findRegistration_department() {
    printf("测试 findRegistration_department 函数...\n");
    test_count_registration++;

    RegisterNode *head = NULL;

    RegisterNode r1 = make_registration("R001", "P001", "郑十", "冯医生", "神经科", "2023-10-08");
    RegisterNode r2 = make_registration("R001", "P001", "冯一", "陈医生", "皮肤科", "2023-10-09");
    add_registration(&head, r1);
    add_registration(&head, r2);

    // 查找存在的挂号记录
    RegisterNode *found = findRegistration_department(head, "神经科");
    assert(found != NULL);
    assert(strcmp(found->department, "神经科") == 0);
    assert(strcmp(found->patientName, "郑十") == 0);

    // 查找不存在的挂号记录
    RegisterNode *notFound = findRegistration_department(head, "不存在科室");
    assert(notFound == NULL);

    // 清理内存
    free_registration_list(head);

    pass_count_registration++;
    printf("✓ findRegistration_department 测试通过\n\n");
}

void test_findRegistration_date() {
    printf("测试 findRegistration_date 函数...\n");
    test_count_registration++;

    RegisterNode *head = NULL;

    RegisterNode r1 = make_registration("R001", "P001", "陈二", "褚医生", "泌尿科", "2023-10-10");
    RegisterNode r2 = make_registration("R001", "P001", "褚三", "卫医生", "肿瘤科", "2023-10-11");
    add_registration(&head, r1);
    add_registration(&head, r2);

    // 查找存在的挂号记录
    RegisterNode *found = findRegistration_date(head, "2023-10-10");
    assert(found != NULL);
    assert(strcmp(found->date, "2023-10-10") == 0);
    assert(strcmp(found->patientName, "陈二") == 0);

    // 查找不存在的挂号记录
    RegisterNode *notFound = findRegistration_date(head, "2099-01-01");
    assert(notFound == NULL);

    // 清理内存
    free_registration_list(head);

    pass_count_registration++;
    printf("✓ findRegistration_date 测试通过\n\n");
}

void test_delete_registration() {
    printf("测试 delete_registration 函数...\n");
    test_count_registration++;

    RegisterNode *head = NULL;

    RegisterNode r1 = make_registration("R001", "P001", "卫四", "蒋医生", "呼吸科", "2023-10-12");
    RegisterNode r2 = make_registration("R001", "P001", "蒋五", "沈医生", "消化科", "2023-10-13");
    RegisterNode r3 = make_registration("R001", "P001", "沈六", "韩医生", "内分泌科", "2023-10-14");
    add_registration(&head, r1);
    add_registration(&head, r2);
    add_registration(&head, r3);

    // 删除中间的挂号记录
    head = delete_registration(head, "蒋五", "2023-10-13");
    RegisterNode *found = findRegistration_patient(head, "蒋五");
    assert(found == NULL);

    // 删除第一个挂号记录
    head = delete_registration(head, "卫四", "2023-10-12");
    found = findRegistration_patient(head, "卫四");
    assert(found == NULL);

    // 验证剩余的挂号记录仍然存在
    found = findRegistration_patient(head, "沈六");
    assert(found != NULL);
    assert(strcmp(found->patientName, "沈六") == 0);

    // 清理内存
    free_registration_list(head);

    pass_count_registration++;
    printf("✓ delete_registration 测试通过\n\n");
}

void run_registration_tests() {
    printf("========== 挂号管理模块测试 ==========\n\n");

    test_make_registration();
    test_add_registration();
    test_findRegistration_patient();
    test_findRegistration_doctor();
    test_findRegistration_department();
    test_findRegistration_date();
    test_delete_registration();

    printf("========================================\n");
    printf("挂号模块测试完成: %d/%d 通过\n", pass_count_registration, test_count_registration);
    printf("========================================\n\n");
}
