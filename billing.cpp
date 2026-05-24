#include "billing.h"
#include <fstream>
#include <ctime>
#include <sstream>

static std::string nowStr() {
    std::time_t t = std::time(nullptr);
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&t));
    return std::string(buf);
}

BillingStack::BillingStack(): top(nullptr) {}
BillingStack::~BillingStack() {
    while (top) {
        BillRecord* t = top;
        top = top->next;
        delete t;
    }
}

void BillingStack::push(const std::string& name, double amt) {
    BillRecord* n = new BillRecord(name, amt, nowStr());
    n->next = top;
    top = n;
}

bool BillingStack::pop(std::string& nameOut, double& amtOut, std::string& timeOut) {
    if (!top) return false;
    BillRecord* t = top;
    nameOut = t->patientName;
    amtOut = t->amount;
    timeOut = t->timestamp;
    top = top->next;
    delete t;
    return true;
}

BillRecord* BillingStack::peek() { return top; }

std::vector<BillRecord*> BillingStack::getAllRecords() const {
    std::vector<BillRecord*> out;
    BillRecord* cur = top;
    while (cur) { out.push_back(cur); cur = cur->next; }
    return out;
}

void BillingStack::saveToFile(const std::string &filename) {
    std::ofstream out(filename);
    BillRecord* cur = top;
    while (cur) {
        out << "\"" << cur->patientName << "\"," << cur->amount << ",\"" << cur->timestamp << "\"\n";
        cur = cur->next;
    }
}

static std::string stripQ(const std::string &s) {
    if (s.size()>=2 && s.front()=='"' && s.back()=='"') return s.substr(1, s.size()-2);
    return s;
}

void BillingStack::loadFromFile(const std::string &filename) {
    std::ifstream in(filename);
    if (!in) return;
    std::string line;
    // Note: file timestamps will not be preserved into the stack order; we re-push them with current timestamp.
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        std::vector<std::string> parts; std::string cur; bool inQ=false;
        for (size_t i=0;i<line.size();++i) {
            char c=line[i];
            if (c=='"') { inQ=!inQ; cur.push_back(c); }
            else if (c==',' && !inQ) { parts.push_back(cur); cur.clear(); }
            else cur.push_back(c);
        }
        parts.push_back(cur);
        if (parts.size() < 3) continue;
        std::string n = stripQ(parts[0]);
        double a = std::stod(parts[1]);
        std::string t = stripQ(parts[2]);
        push(n, a); // timestamp will be now
    }
}
