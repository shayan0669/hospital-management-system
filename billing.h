#ifndef BILLING_H
#define BILLING_H

#include <string>
#include <vector>

struct BillRecord {
    std::string patientName;
    double amount; // PKR
    std::string timestamp;
    BillRecord* next;
    BillRecord(const std::string &n, double a, const std::string &t)
        : patientName(n), amount(a), timestamp(t), next(nullptr) {}
};

class BillingStack {
private:
    BillRecord* top;
public:
    BillingStack();
    ~BillingStack();

    void push(const std::string& name, double amt);
    bool pop(std::string& nameOut, double& amtOut, std::string& timeOut);
    BillRecord* peek();

    std::vector<BillRecord*> getAllRecords() const;

    void saveToFile(const std::string &filename = "billing.txt");
    void loadFromFile(const std::string &filename = "billing.txt");
};

#endif // BILLING_H
