#include "appointment.h"
#include <fstream>

ApptManager::ApptManager(): front(nullptr), rear(nullptr) {}
ApptManager::~ApptManager() {
    while (front) {
        Appointment* t = front;
        front = front->next;
        delete t;
    }
    rear = nullptr;
}

void ApptManager::schedule(const std::string& patient, const std::string& doctor, const std::string& slot) {
    Appointment* node = new Appointment(patient, doctor, slot);
    if (!rear) { front = rear = node; return; }
    rear->next = node;
    rear = node;
}

bool ApptManager::processNext(std::string& pOut, std::string& dOut, std::string& slotOut) {
    if (!front) return false;
    Appointment* t = front;
    pOut = t->patientName;
    dOut = t->doctorName;
    slotOut = t->slot;
    front = front->next;
    if (!front) rear = nullptr;
    delete t;
    return true;
}

bool ApptManager::isSlotBooked(const std::string& doctor, const std::string& slot) const {
    Appointment* cur = front;
    while (cur) {
        if (cur->doctorName == doctor && cur->slot == slot) return true;
        cur = cur->next;
    }
    return false;
}

std::vector<Appointment*> ApptManager::getAllAppointments() const {
    std::vector<Appointment*> out;
    Appointment* cur = front;
    while (cur) { out.push_back(cur); cur = cur->next; }
    return out;
}

void ApptManager::saveToFile(const std::string &filename) {
    std::ofstream out(filename);
    Appointment* cur = front;
    while (cur) {
        out << "\"" << cur->patientName << "\"," << "\"" << cur->doctorName << "\"," << "\"" << cur->slot << "\"\n";
        cur = cur->next;
    }
}

static std::string stripQ(const std::string &s) {
    if (s.size()>=2 && s.front()=='"' && s.back()=='"') return s.substr(1, s.size()-2);
    return s;
}

void ApptManager::loadFromFile(const std::string &filename) {
    std::ifstream in(filename);
    if (!in) return;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts;
        std::string cur; bool inQ=false;
        for (size_t i=0;i<line.size();++i) {
            char c=line[i];
            if (c=='"') { inQ=!inQ; cur.push_back(c); }
            else if (c==',' && !inQ) { parts.push_back(cur); cur.clear(); }
            else cur.push_back(c);
        }
        parts.push_back(cur);
        if (parts.size() < 3) continue;
        std::string p = stripQ(parts[0]);
        std::string d = stripQ(parts[1]);
        std::string s = stripQ(parts[2]);
        schedule(p,d,s);
    }
}
