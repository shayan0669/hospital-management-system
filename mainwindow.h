#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QListWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>

#include "patient.h"
#include "doctor.h"
#include "appointment.h"
#include "billing.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    // backend
    PatientManager pm;
    DoctorManager dm;
    ApptManager am;
    BillingStack bs;

    // UI
    QWidget *central;
    QTabWidget *tabs;

    // Patients
    QTableWidget *patientTable;
    QLineEdit *pIdEdit, *pNameEdit, *pDiseaseEdit, *pPhoneEdit, *pCnicEdit, *pAddressEdit;
    QSpinBox *pRoomSpin, *pAgeSpin;
    QComboBox *pGenderBox;
    QPushButton *pAddBtn, *pUpdateBtn, *pDeleteBtn;

    // Doctors
    QTableWidget *doctorTable;
    QLineEdit *dIdEdit, *dNameEdit, *dSpecEdit;
    QDoubleSpinBox *dFeeSpin;
    QComboBox *dSlotsBox; // editable: comma-separated
    QPushButton *dAddBtn;

    // Appointments
    QListWidget *apptList;
    QLineEdit *aPatientEdit;
    QComboBox *aDoctorSelect; // doctor display
    QComboBox *aSlotSelect;
    QPushButton *aScheduleBtn, *aProcessBtn;

    // Billing
    QLineEdit *billPatientIdEdit;
    QComboBox *billDoctorSelect;
    QSpinBox *billDaysSpin;
    QDoubleSpinBox *billTreatmentSpin;
    QPushButton *billGenBtn, *billUndoBtn, *billExportBtn;
    QListWidget *billHistoryList;
    QLabel *totalLabel;

    // Dashboard
    QLabel *lblTotalPatients;
    QLabel *lblTotalDoctors;
    QLabel *lblUpcomingAppts;
    QLabel *lblRevenue;

    // Helpers
    void setupUI();
    void loadAllData();
    void saveAllData();
    void refreshPatientTable();
    void refreshDoctorTable();
    void refreshAppointmentList();
    void refreshBillHistory();
    void refreshDoctorDropdowns();
    void refreshDashboard();

private slots:
    // Patients
    void onAddPatient();
    void onUpdatePatient();
    void onDeletePatient();
    void onPatientTableClicked(int row, int column);

    // Doctors
    void onAddDoctor();
    void onDoctorTableClicked(int row, int column);
    void onDoctorSelectedForSlot(int index);

    // Appointments
    void onScheduleAppt();
    void onProcessAppt();

    // Billing
    void onGenerateBill();
    void onUndoBill();
    void onExportBillPdf();
};

#endif // MAINWINDOW_H
