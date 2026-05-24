#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <vector>

struct Patient {
    int id;
    std::string name;
    std::string disease;
    int room;

    // Additional fields
    int age;
    std::string cnic;
    std::string gender;
    std::string phone;
    std::string address;
    std::string admissionDate; // "YYYY-MM-DD HH:MM:SS"

    Patient* next;

    Patient(int i = 0, const std::string& n = "", const std::string& d = "", int r = 0)
        : id(i), name(n), disease(d), room(r), age(0), cnic(""),
        gender(""), phone(""), address(""), admissionDate(""), next(nullptr) {}
};

class PatientManager {
private:
    Patient* head;

public:
    PatientManager();
    ~PatientManager();

    void addPatient(int id, const std::string &name, const std::string &disease, int room,
                    int age = 0, const std::string &cnic = "", const std::string &gender = "",
                    const std::string &phone = "", const std::string &address = "");

    bool updatePatient(int id, const std::string &name, const std::string &disease, int room,
                       int age, const std::string &cnic, const std::string &gender,
                       const std::string &phone, const std::string &address);

    bool deletePatient(int id);

    Patient* searchById(int id);
    std::vector<Patient*> getAllPatients();

    void saveToFile(const std::string &filename = "patients.txt");
    void loadFromFile(const std::string &filename = "patients.txt");
};

#endif // PATIENT_H
