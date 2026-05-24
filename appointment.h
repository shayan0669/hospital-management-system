#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <string>
#include <vector>

struct Appointment {
    std::string patientName;
    std::string doctorName;
    std::string slot; // e.g., "09:00 AM"
    Appointment* next;
    Appointment(const std::string &p, const std::string &d, const std::string &s)
        : patientName(p), doctorName(d), slot(s), next(nullptr) {}
};

class ApptManager {
private:
    Appointment* front;
    Appointment* rear;

public:
    ApptManager();
    ~ApptManager();

    void schedule(const std::string& patient, const std::string& doctor, const std::string& slot);
    bool processNext(std::string& pOut, std::string& dOut, std::string& slotOut);

    // check double-booking
    bool isSlotBooked(const std::string& doctor, const std::string& slot) const;

    std::vector<Appointment*> getAllAppointments() const;

    void saveToFile(const std::string &filename = "appointments.txt");
    void loadFromFile(const std::string &filename = "appointments.txt");
};

#endif // APPOINTMENT_H
