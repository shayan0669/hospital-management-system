#ifndef DOCTOR_H
#define DOCTOR_H

#include <string>
#include <vector>
#include <fstream>

struct Doctor {
    int id;
    std::string name;
    std::string specialization;
    double fee; // PKR
    std::vector<std::string> availableSlots;
    Doctor* left;
    Doctor* right;

    Doctor(int i = 0, const std::string &n = "", const std::string &s = "",
           double f = 0.0, const std::vector<std::string>& slots = std::vector<std::string>())
        : id(i), name(n), specialization(s), fee(f), availableSlots(slots), left(nullptr), right(nullptr) {}
};

class DoctorManager {
private:
    Doctor* root;

    Doctor* insertRec(Doctor* node, int id, const std::string& name, const std::string& spec, double fee, const std::vector<std::string>& slots);
    Doctor* searchRec(Doctor* node, int id);
    void inorderRec(Doctor* node, std::vector<Doctor*>& list);
    void saveRec(Doctor* node, std::ofstream& file);
    void freeTree(Doctor* node);
public:
    DoctorManager();
    ~DoctorManager();

    void addDoctor(int id, const std::string& name, const std::string& spec, double fee, const std::vector<std::string>& slots = {});
    Doctor* searchById(int id);
    std::vector<Doctor*> getAllDoctors();

    void saveToFile(const std::string &filename = "doctors.txt");
    void loadFromFile(const std::string &filename = "doctors.txt");
};

#endif // DOCTOR_H
