//
// Created by 罗金源 on 2025/12/19.
//
#include <stdio.h>
#include "test.h"
int main() {
    printf("\n");
    printf("********************************************\n");
    printf("*       医院管理系统单元测试程序           *\n");
    printf("********************************************\n\n");

    run_patient_tests();
    run_doctor_tests();

    int total_tests = test_count_patient + test_count_doctor;
    int total_passed = pass_count_patient + pass_count_doctor;

    printf("\n");
    printf("============================================\n");
    printf("           测试总结报告\n");
    printf("============================================\n");
    printf("总测试数: %d\n", total_tests);
    printf("通过数: %d\n", total_passed);
    printf("失败数: %d\n", total_tests - total_passed);
    if (total_passed == total_tests) {
        printf("状态: ✓ 全部通过\n");
    } else {
        printf("状态: ✗ 有测试失败\n");
    }
    printf("============================================\n\n");

    return (total_passed == total_tests) ? 0 : 1;
}