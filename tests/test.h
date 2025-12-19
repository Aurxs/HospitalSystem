#ifndef TEST_H
#define TEST_H

// 患者测试函数和变量声明
extern int test_count;
extern int pass_count;
void run_patient_tests();

// 医生测试函数和变量声明
extern int test_count_doctor;
extern int pass_count_doctor;
void run_doctor_tests();

#endif // TEST_H
