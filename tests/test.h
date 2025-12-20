#ifndef TEST_H
#define TEST_H

// 患者测试函数和变量声明
extern int test_count_patient;
extern int pass_count_patient;
void run_patient_tests();

// 医生测试函数和变量声明
extern int test_count_doctor;
extern int pass_count_doctor;
void run_doctor_tests();

extern int test_count_drug;
extern int pass_count_drug;
void run_drug_tests();

// 挂号管理测试函数和变量声明
extern int test_count_registration;
extern int pass_count_registration;
void run_registration_tests();

// 费用管理测试函数和变量声明
extern int test_count_bill;
extern int pass_count_bill;
void run_bill_tests();

// 文件读写测试函数和变量声明
extern int test_count_file_io;
extern int pass_count_file_io;
void run_file_io_tests();

#endif // TEST_H