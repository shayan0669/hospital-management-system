#include "doctor.h"
#include <sstream>

DoctorManager::DoctorManager(): root(nullptr) {}
DoctorManager::~DoctorManager() { freeTree(root); }

void DoctorManager::freeTree(Doctor* node) {
    if (!node) return;
    freeTree(node->left);
    freeTree(node->right);
    delete node;
}

Doctor* DoctorManager::insertRec(Doctor* node, int id, const std::string& name, const std::string& spec, double fee, const std::vector<std::string>& slots) {
    if (!node) return new Doctor(id, name, spec, fee, slots);
    if (id < node->id) node->left = insertRec(node->left, id, name, spec, fee, slots);
    else if (id > node->id) node->right = insertRec(node->right, id, name, spec, fee, slots);
    else {
        // update existing
        node->name = name; node->specialization = spec; node->fee = fee; node->availableSlots = slots;
    }
    return node;
}

void DoctorManager::addDoctor(int id, const std::string& name, const std::string& spec, double fee, const std::vector<std::string>& slots) {
    root = insertRec(root, id, name, spec, fee, slots);
}

Doctor* DoctorManager::searchRec(Doctor* node, int id) {
    if (!node) return nullptr;
    if (node->id == id) return node;
    if (id < node->id) return searchRec(node->left, id);
    return searchRec(node->right, id);
}

Doctor* DoctorManager::searchById(int id) { return searchRec(root, id); }

void DoctorManager::inorderRec(Doctor* node, std::vector<Doctor*>& list) {
    if (!node) return;
    inorderRec(node->left, list);
    list.push_back(node);
    inorderRec(node->right, list);
}

std::vector<Doctor*> DoctorManager::getAllDoctors() {
    std::vector<Doctor*> out;
    inorderRec(root, out);
    return out;
}

void DoctorManager::saveRec(Doctor* node, std::ofstream& file) {
    if (!node) return;
    std::ostringstream ss;
    for (size_t i=0;i<node->availableSlots.size();++i) {
        if (i) ss << ";";
        ss << node->availableSlots[i];
    }
    file << node->id << ",\"" << node->name << "\",\"" << node->specialization << "\"," << node->fee << ",\"" << ss.str() << "\"\n";
    saveRec(node->left, file);
    saveRec(node->right, file);
}

void DoctorManager::saveToFile(const std::string &filename) {
    std::ofstream out(filename);
    if (!out) return;
    saveRec(root, out);
}

static std::string stripQ(const std::string &s) {
    if (s.size()>=2 && s.front()=='"' && s.back()=='"') return s.substr(1, s.size()-2);
    return s;
}

static std::vector<std::string> splitSlots(const std::string &s) {
    std::vector<std::string> out;
    std::string cur;
    for (char c: s) {
        if (c==';') { if (!cur.empty()) out.push_back(cur); cur.clear(); }
        else cur.push_back(c);
    }
    if (!cur.empty()) out.push_back(cur);
    return out;
}

void DoctorManager::loadFromFile(const std::string &filename) {
    std::ifstream in(filename);
    if (!in) return;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        // parse CSV with quotes
        std::vector<std::string> parts;
        std::string cur; bool inQ=false;
        for (size_t i=0;i<line.size();++i) {
            char c = line[i];
            if (c=='"') { inQ = !inQ; cur.push_back(c); }
            else if (c==',' && !inQ) { parts.push_back(cur); cur.clear(); }
            else cur.push_back(c);
        }
        parts.push_back(cur);
        if (parts.size() < 5) continue;
        int id = std::stoi(parts[0]);
        std::string name = stripQ(parts[1]);
        std::string spec = stripQ(parts[2]);
        double fee = std::stod(parts[3]);
        std::string slotsCsv = stripQ(parts[4]);
        std::vector<std::string> slots = splitSlots(slotsCsv);
        addDoctor(id, name, spec, fee, slots);
    }
}
