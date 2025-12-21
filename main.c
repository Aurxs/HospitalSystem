#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datastruct.h"
#include "file_io.h"

int main() {
    PatientNode *patient = load_patients("patients.txt");
    DoctorNode *doctor = load_doctors("doctors.txt");
    DrugNode *drug = load_drugs("drugs.txt");
    RegisterNode *registration = load_registrations("registrations.txt");
    BillNode *bill = load_bills("bills.txt");
}