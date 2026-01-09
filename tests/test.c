#include <stdio.h>
#include "test.h"
int main() {
    printf("\n");
    printf("********************************************\n");
    printf("*       医院管理系统单元测试程序           *\n");
    printf("********************************************\n\n");

    run_patient_tests();
    run_doctor_tests();
    run_drug_tests();
    run_registration_tests();
    run_bill_tests();
    run_file_io_tests();
    run_auth_tests();

    int total_tests = test_count_patient + test_count_doctor + test_count_drug
                    + test_count_registration + test_count_bill + test_count_file_io + test_count_auth;
    int total_passed = pass_count_patient + pass_count_doctor + pass_count_drug
                     + pass_count_registration + pass_count_bill + pass_count_file_io + pass_count_auth;

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