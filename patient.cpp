#include "patient.h"
#include <fstream>
#include <sstream>
#include <ctime>

static std::string nowString() {
    std::time_t t = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
}

PatientManager::PatientManager(): head(nullptr) {}
PatientManager::~PatientManager() {
    while (head) {
        Patient* t = head;
        head = head->next;
        delete t;
    }
}

void PatientManager::addPatient(int id, const std::string &name, const std::string &disease, int room,
                                int age, const std::string &cnic, const std::string &gender,
                                const std::string &phone, const std::string &address)
{
    Patient* p = new Patient(id, name, disease, room);
    p->age = age;
    p->cnic = cnic;
    p->gender = gender;
    p->phone = phone;
    p->address = address;
    p->admissionDate = nowString();

    // Insert at head
    p->next = head;
    head = p;
}

bool PatientManager::updatePatient(int id, const std::string &name, const std::string &disease, int room,
                                   int age, const std::string &cnic, const std::string &gender,
                                   const std::string &phone, const std::string &address)
{
    Patient* p = searchById(id);
    if (!p) return false;
    p->name = name;
    p->disease = disease;
    p->room = room;
    p->age = age;
    p->cnic = cnic;
    p->gender = gender;
    p->phone = phone;
    p->address = address;
    return true;
}

bool PatientManager::deletePatient(int id) {
    Patient* cur = head;
    Patient* prev = nullptr;
    while (cur) {
        if (cur->id == id) {
            if (prev) prev->next = cur->next;
            else head = cur->next;
            delete cur;
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}

Patient* PatientManager::searchById(int id) {
    Patient* cur = head;
    while (cur) {
        if (cur->id == id) return cur;
        cur = cur->next;
    }
    return nullptr;
}

std::vector<Patient*> PatientManager::getAllPatients() {
    std::vector<Patient*> out;
    Patient* cur = head;
    while (cur) { out.push_back(cur); cur = cur->next; }
    return out;
}

void PatientManager::saveToFile(const std::string &filename) {
    std::ofstream out(filename);
    Patient* cur = head;
    while (cur) {
        // CSV: id,"name","disease",room,age,"cnic","gender","phone","address","admissionDate"
        out << cur->id << ",\"" << cur->name << "\",\"" << cur->disease << "\"," << cur->room
            << "," << cur->age << ",\"" << cur->cnic << "\",\"" << cur->gender << "\","
            << "\"" << cur->phone << "\",\"" << cur->address << "\",\"" << cur->admissionDate << "\"\n";
        cur = cur->next;
    }
}

static std::vector<std::string> splitCSVSafe(const std::string &line) {
    std::vector<std::string> res;
    std::string cur;
    bool inQuotes = false;
    for (size_t i=0;i<line.size();++i) {
        char c = line[i];
        if (c == '"') { inQuotes = !inQuotes; cur.push_back(c); }
        else if (c == ',' && !inQuotes) { res.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    res.push_back(cur);
    return res;
}

static std::string stripQuotes(const std::string &s) {
    if (s.size() >= 2 && s.front()=='"' && s.back()=='"') return s.substr(1, s.size()-2);
    return s;
}

void PatientManager::loadFromFile(const std::string &filename) {
    std::ifstream in(filename);
    if (!in.is_open()) return;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto parts = splitCSVSafe(line);
        if (parts.size() < 10) continue;
        int id = std::stoi(parts[0]);
        std::string name = stripQuotes(parts[1]);
        std::string disease = stripQuotes(parts[2]);
        int room = std::stoi(parts[3]);
        int age = std::stoi(parts[4]);
        std::string cnic = stripQuotes(parts[5]);
        std::string gender = stripQuotes(parts[6]);
        std::string phone = stripQuotes(parts[7]);
        std::string address = stripQuotes(parts[8]);
        std::string adm = stripQuotes(parts[9]);

        Patient* p = new Patient(id, name, disease, room);
        p->age = age; p->cnic = cnic; p->gender = gender; p->phone = phone; p->address = address; p->admissionDate = adm;
        p->next = head;
        head = p;
    }
}
