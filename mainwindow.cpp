#include "mainwindow.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QFont>
#include <QListWidgetItem>
#include <QPdfWriter>
#include <QPainter>
#include <QDir>
#include <iomanip>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

static QString formatPKR(double amount) {
    std::ostringstream ss;
    ss.setf(std::ios::fixed); ss<<std::setprecision(2)<<amount;
    return QString("Rs. ") + QString::fromStdString(ss.str());
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    loadAllData();
    refreshPatientTable();
    refreshDoctorTable();
    refreshAppointmentList();
    refreshBillHistory();
    refreshDashboard();
    setWindowTitle("Hospital Management System - Qt (Full)");
    resize(1200, 760);
}

MainWindow::~MainWindow() { saveAllData(); }

void MainWindow::setupUI() {
    central = new QWidget(this);
    setCentralWidget(central);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    tabs = new QTabWidget(central);
    mainLayout->addWidget(tabs);

    // Patients Tab
    QWidget *pTab = new QWidget;
    QVBoxLayout *pL = new QVBoxLayout(pTab);
    patientTable = new QTableWidget(0, 8);
    patientTable->setHorizontalHeaderLabels({"ID","Name","Disease","Room","Age","Gender","Phone","Admitted"});
    patientTable->horizontalHeader()->setStretchLastSection(true);
    connect(patientTable, &QTableWidget::cellClicked, this, &MainWindow::onPatientTableClicked);
    pL->addWidget(patientTable);

    QWidget *pForm = new QWidget;
    QHBoxLayout *pF = new QHBoxLayout(pForm);
    pIdEdit = new QLineEdit(); pIdEdit->setPlaceholderText("ID");
    pNameEdit = new QLineEdit(); pNameEdit->setPlaceholderText("Name");
    pDiseaseEdit = new QLineEdit(); pDiseaseEdit->setPlaceholderText("Disease");
    pRoomSpin = new QSpinBox(); pRoomSpin->setRange(1,1000);
    pAgeSpin = new QSpinBox(); pAgeSpin->setRange(0,150);
    pGenderBox = new QComboBox(); pGenderBox->addItems({"Male","Female","Other"});
    pPhoneEdit = new QLineEdit(); pPhoneEdit->setPlaceholderText("Phone");
    pCnicEdit = new QLineEdit(); pCnicEdit->setPlaceholderText("CNIC");
    pAddressEdit = new QLineEdit(); pAddressEdit->setPlaceholderText("Address");
    pAddBtn = new QPushButton("Add");
    pUpdateBtn = new QPushButton("Update");
    pDeleteBtn = new QPushButton("Delete");

    pF->addWidget(pIdEdit); pF->addWidget(pNameEdit); pF->addWidget(pDiseaseEdit);
    pF->addWidget(pRoomSpin); pF->addWidget(pAgeSpin); pF->addWidget(pGenderBox);
    pF->addWidget(pPhoneEdit); pF->addWidget(pCnicEdit); pF->addWidget(pAddressEdit);
    pF->addWidget(pAddBtn); pF->addWidget(pUpdateBtn); pF->addWidget(pDeleteBtn);
    pL->addWidget(pForm);

    connect(pAddBtn, &QPushButton::clicked, this, &MainWindow::onAddPatient);
    connect(pUpdateBtn, &QPushButton::clicked, this, &MainWindow::onUpdatePatient);
    connect(pDeleteBtn, &QPushButton::clicked, this, &MainWindow::onDeletePatient);

    tabs->addTab(pTab, "Patients");

    // Doctors Tab
    QWidget *dTab = new QWidget;
    QVBoxLayout *dL = new QVBoxLayout(dTab);
    doctorTable = new QTableWidget(0,4);
    doctorTable->setHorizontalHeaderLabels({"ID","Name","Specialization","Fee (PKR)"});
    doctorTable->horizontalHeader()->setStretchLastSection(true);
    connect(doctorTable, &QTableWidget::cellClicked, this, &MainWindow::onDoctorTableClicked);
    dL->addWidget(doctorTable);

    QWidget *dForm = new QWidget;
    QHBoxLayout *dF = new QHBoxLayout(dForm);
    dIdEdit = new QLineEdit(); dIdEdit->setPlaceholderText("ID");
    dNameEdit = new QLineEdit(); dNameEdit->setPlaceholderText("Name");
    dSpecEdit = new QLineEdit(); dSpecEdit->setPlaceholderText("Specialization");
    dFeeSpin = new QDoubleSpinBox(); dFeeSpin->setRange(0,1e7); dFeeSpin->setDecimals(2);
    dSlotsBox = new QComboBox(); dSlotsBox->setEditable(true);
    dSlotsBox->setPlaceholderText("Slots comma-separated (09:00 AM,09:30 AM,...)");
    dAddBtn = new QPushButton("Add/Update Doctor");

    dF->addWidget(dIdEdit); dF->addWidget(dNameEdit); dF->addWidget(dSpecEdit); dF->addWidget(dFeeSpin); dF->addWidget(dSlotsBox); dF->addWidget(dAddBtn);
    dL->addWidget(dForm);
    connect(dAddBtn, &QPushButton::clicked, this, &MainWindow::onAddDoctor);

    tabs->addTab(dTab, "Doctors");

    // Appointments Tab
    QWidget *aTab = new QWidget;
    QVBoxLayout *aL = new QVBoxLayout(aTab);
    apptList = new QListWidget();
    aL->addWidget(apptList);

    QWidget *aForm = new QWidget;
    QHBoxLayout *aF = new QHBoxLayout(aForm);
    aPatientEdit = new QLineEdit(); aPatientEdit->setPlaceholderText("Patient Name");
    aDoctorSelect = new QComboBox();
    aSlotSelect = new QComboBox();
    aScheduleBtn = new QPushButton("Schedule");
    aProcessBtn = new QPushButton("Process Next");

    aF->addWidget(aPatientEdit); aF->addWidget(aDoctorSelect); aF->addWidget(aSlotSelect); aF->addWidget(aScheduleBtn); aF->addWidget(aProcessBtn);
    aL->addWidget(aForm);

    connect(aScheduleBtn, &QPushButton::clicked, this, &MainWindow::onScheduleAppt);
    connect(aProcessBtn, &QPushButton::clicked, this, &MainWindow::onProcessAppt);
    connect(aDoctorSelect, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onDoctorSelectedForSlot);

    tabs->addTab(aTab, "Appointments");

    // Billing Tab
    QWidget *bTab = new QWidget;
    QVBoxLayout *bL = new QVBoxLayout(bTab);

    QWidget *bForm = new QWidget;
    QHBoxLayout *bF = new QHBoxLayout(bForm);
    billPatientIdEdit = new QLineEdit(); billPatientIdEdit->setPlaceholderText("Patient ID");
    billDoctorSelect = new QComboBox();
    billDaysSpin = new QSpinBox(); billDaysSpin->setRange(0,365);
    billTreatmentSpin = new QDoubleSpinBox(); billTreatmentSpin->setRange(0,1e7); billTreatmentSpin->setDecimals(2);
    billGenBtn = new QPushButton("Generate Bill");
    billUndoBtn = new QPushButton("Undo Last");
    billExportBtn = new QPushButton("Export PDF");

    bF->addWidget(billPatientIdEdit); bF->addWidget(billDoctorSelect); bF->addWidget(new QLabel("Days")); bF->addWidget(billDaysSpin);
    bF->addWidget(new QLabel("Treatment (PKR)")); bF->addWidget(billTreatmentSpin); bF->addWidget(billGenBtn); bF->addWidget(billUndoBtn); bF->addWidget(billExportBtn);
    bL->addWidget(bForm);

    totalLabel = new QLabel();
    QFont f; f.setPointSize(12); f.setBold(true);
    totalLabel->setFont(f);
    totalLabel->setStyleSheet("color: darkgreen;");
    bL->addWidget(totalLabel);

    billHistoryList = new QListWidget();
    bL->addWidget(new QLabel("Bill History:"));
    bL->addWidget(billHistoryList);

    connect(billGenBtn, &QPushButton::clicked, this, &MainWindow::onGenerateBill);
    connect(billUndoBtn, &QPushButton::clicked, this, &MainWindow::onUndoBill);
    connect(billExportBtn, &QPushButton::clicked, this, &MainWindow::onExportBillPdf);

    tabs->addTab(bTab, "Billing");

    // Dashboard
    QWidget *dash = new QWidget;
    QHBoxLayout *dshL = new QHBoxLayout(dash);
    lblTotalPatients = new QLabel("Patients: 0");
    lblTotalDoctors = new QLabel("Doctors: 0");
    lblUpcomingAppts = new QLabel("Appointments: 0");
    lblRevenue = new QLabel("Revenue: Rs. 0.00");
    dshL->addWidget(lblTotalPatients); dshL->addWidget(lblTotalDoctors); dshL->addWidget(lblUpcomingAppts); dshL->addWidget(lblRevenue);
    mainLayout->addWidget(dash);
}

void MainWindow::loadAllData() {
    pm.loadFromFile();
    dm.loadFromFile();
    am.loadFromFile();
    bs.loadFromFile();
}

void MainWindow::saveAllData() {
    pm.saveToFile();
    dm.saveToFile();
    am.saveToFile();
    bs.saveToFile();
}

void MainWindow::refreshPatientTable() {
    patientTable->setRowCount(0);
    auto list = pm.getAllPatients();
    for (auto p : list) {
        int r = patientTable->rowCount();
        patientTable->insertRow(r);
        patientTable->setItem(r,0, new QTableWidgetItem(QString::number(p->id)));
        patientTable->setItem(r,1, new QTableWidgetItem(QString::fromStdString(p->name)));
        patientTable->setItem(r,2, new QTableWidgetItem(QString::fromStdString(p->disease)));
        patientTable->setItem(r,3, new QTableWidgetItem(QString::number(p->room)));
        patientTable->setItem(r,4, new QTableWidgetItem(QString::number(p->age)));
        patientTable->setItem(r,5, new QTableWidgetItem(QString::fromStdString(p->gender)));
        patientTable->setItem(r,6, new QTableWidgetItem(QString::fromStdString(p->phone)));
        patientTable->setItem(r,7, new QTableWidgetItem(QString::fromStdString(p->admissionDate)));
    }
    refreshDashboard();
}

void MainWindow::refreshDoctorTable() {
    doctorTable->setRowCount(0);
    auto list = dm.getAllDoctors();
    for (auto d : list) {
        int r = doctorTable->rowCount();
        doctorTable->insertRow(r);
        doctorTable->setItem(r,0, new QTableWidgetItem(QString::number(d->id)));
        doctorTable->setItem(r,1, new QTableWidgetItem(QString::fromStdString(d->name)));
        doctorTable->setItem(r,2, new QTableWidgetItem(QString::fromStdString(d->specialization)));
        doctorTable->setItem(r,3, new QTableWidgetItem(formatPKR(d->fee)));
    }
    refreshDoctorDropdowns();
    refreshDashboard();
}

void MainWindow::refreshDoctorDropdowns() {
    aDoctorSelect->clear();
    billDoctorSelect->clear();
    aDoctorSelect->addItem("Select Doctor");
    billDoctorSelect->addItem("None");
    auto list = dm.getAllDoctors();
    for (auto d : list) {
        QString label = QString::fromStdString(d->name) + " (" + QString::fromStdString(d->specialization) + ")";
        aDoctorSelect->addItem(label, d->id);
        billDoctorSelect->addItem(label, d->id);
    }
}

void MainWindow::refreshAppointmentList() {
    apptList->clear();
    auto list = am.getAllAppointments();
    for (auto ap : list) {
        std::string line = ap->patientName + " with " + ap->doctorName + " @ " + ap->slot;
        QListWidgetItem *it = new QListWidgetItem(QString::fromStdString(line));
        it->setToolTip(QString::fromStdString("Patient: " + ap->patientName + "\nDoctor: " + ap->doctorName + "\nSlot: " + ap->slot));
        apptList->addItem(it);
    }
    refreshDashboard();
}

void MainWindow::refreshBillHistory() {
    billHistoryList->clear();
    auto recs = bs.getAllRecords();
    double totalRevenue = 0.0;
    for (auto r : recs) {
        std::ostringstream ss;
        ss.setf(std::ios::fixed); ss<<std::setprecision(2)<<r->amount;
        QString line = QString::fromStdString(r->patientName) + " - " + QString("Rs. ") + QString::fromStdString(ss.str()) + " (" + QString::fromStdString(r->timestamp) + ")";
        QListWidgetItem *it = new QListWidgetItem(line);
        it->setForeground(QColor("darkGreen"));
        QFont f; f.setBold(true);
        it->setFont(f);
        billHistoryList->addItem(it);
        totalRevenue += r->amount;
    }
    totalLabel->setText(QString("<b>Total (recent): </b>") + formatPKR(totalRevenue));
    lblRevenue->setText("Revenue: " + formatPKR(totalRevenue));
}

void MainWindow::refreshDashboard() {
    lblTotalPatients->setText("Patients: " + QString::number(pm.getAllPatients().size()));
    lblTotalDoctors->setText("Doctors: " + QString::number(dm.getAllDoctors().size()));
    lblUpcomingAppts->setText("Appointments: " + QString::number(am.getAllAppointments().size()));
}

void MainWindow::onAddPatient() {
    bool ok;
    int id = pIdEdit->text().toInt(&ok);
    if (!ok || pNameEdit->text().isEmpty()) { QMessageBox::warning(this, "Input", "Invalid ID or name"); return; }
    pm.addPatient(id, pNameEdit->text().toStdString(), pDiseaseEdit->text().toStdString(), pRoomSpin->value(),
                  pAgeSpin->value(), pCnicEdit->text().toStdString(), pGenderBox->currentText().toStdString(),
                  pPhoneEdit->text().toStdString(), pAddressEdit->text().toStdString());
    refreshPatientTable();
}

void MainWindow::onUpdatePatient() {
    int id = pIdEdit->text().toInt();
    Patient* p = pm.searchById(id);
    if (!p) { QMessageBox::warning(this,"Error","Patient not found"); return; }
    pm.updatePatient(id, pNameEdit->text().toStdString(), pDiseaseEdit->text().toStdString(), pRoomSpin->value(),
                     pAgeSpin->value(), pCnicEdit->text().toStdString(), pGenderBox->currentText().toStdString(),
                     pPhoneEdit->text().toStdString(), pAddressEdit->text().toStdString());
    refreshPatientTable();
}

void MainWindow::onDeletePatient() {
    int id = pIdEdit->text().toInt();
    if (!pm.deletePatient(id)) { QMessageBox::warning(this,"Error","Patient not found"); return; }
    refreshPatientTable();
}

void MainWindow::onPatientTableClicked(int row, int) {
    if (row < 0 || row >= patientTable->rowCount()) return;
    pIdEdit->setText(patientTable->item(row,0)->text());
    pNameEdit->setText(patientTable->item(row,1)->text());
    pDiseaseEdit->setText(patientTable->item(row,2)->text());
    pRoomSpin->setValue(patientTable->item(row,3)->text().toInt());
    pAgeSpin->setValue(patientTable->item(row,4)->text().toInt());
    pGenderBox->setCurrentText(patientTable->item(row,5)->text());
    pPhoneEdit->setText(patientTable->item(row,6)->text());
}

void MainWindow::onAddDoctor() {
    bool ok;
    int id = dIdEdit->text().toInt(&ok);
    if (!ok || dNameEdit->text().isEmpty()) { QMessageBox::warning(this,"Error","Invalid doctor ID or name"); return; }
    QString slotsText = dSlotsBox->currentText();
    vector<string> slots;
    string s = slotsText.toStdString();
    stringstream ss(s);
    string token;
    while (std::getline(ss, token, ',')) {
        size_t start = token.find_first_not_of(" \t");
        size_t end = token.find_last_not_of(" \t");
        if (start!=std::string::npos && end!=std::string::npos)
            slots.push_back(token.substr(start, end-start+1));
        else if (!token.empty()) slots.push_back(token);
    }
    dm.addDoctor(id, dNameEdit->text().toStdString(), dSpecEdit->text().toStdString(), dFeeSpin->value(), slots);
    refreshDoctorTable();
}

void MainWindow::onDoctorTableClicked(int row, int) {
    if (row < 0 || row >= doctorTable->rowCount()) return;
    dIdEdit->setText(doctorTable->item(row,0)->text());
    dNameEdit->setText(doctorTable->item(row,1)->text());
    dSpecEdit->setText(doctorTable->item(row,2)->text());
    QString fee = doctorTable->item(row,3)->text(); fee.remove("Rs."); fee = fee.trimmed();
    dFeeSpin->setValue(fee.toDouble());
    int id = dIdEdit->text().toInt();
    Doctor* doc = dm.searchById(id);
    if (doc) {
        QString all;
        for (auto &sl: doc->availableSlots) {
            if (!all.isEmpty()) all += ",";
            all += QString::fromStdString(sl);
        }
        dSlotsBox->setCurrentText(all);
    }
}

void MainWindow::onDoctorSelectedForSlot(int index) {
    aSlotSelect->clear();
    if (index <= 0) return;
    int docId = aDoctorSelect->currentData().toInt();
    Doctor* doc = dm.searchById(docId);
    if (!doc) return;
    for (auto &sl: doc->availableSlots) aSlotSelect->addItem(QString::fromStdString(sl));
}

void MainWindow::onScheduleAppt() {
    std::string patient = aPatientEdit->text().toStdString();
    int idx = aDoctorSelect->currentIndex();
    if (idx <= 0) { QMessageBox::warning(this,"Error","Select a doctor"); return; }
    int did = aDoctorSelect->currentData().toInt();
    Doctor* doc = dm.searchById(did);
    if (!doc) { QMessageBox::warning(this,"Error","Doctor not found"); return; }
    QString slotQ = aSlotSelect->currentText();
    if (slotQ.isEmpty()) { QMessageBox::warning(this,"Error","Select slot"); return; }
    std::string slot = slotQ.toStdString();
    if (am.isSlotBooked(doc->name, slot)) {
        QMessageBox::warning(this,"Booked","This slot is already booked for the selected doctor.");
        return;
    }
    am.schedule(patient, doc->name, slot);
    refreshAppointmentList();
}

void MainWindow::onProcessAppt() {
    std::string p,d,s;
    if (!am.processNext(p,d,s)) { QMessageBox::information(this,"Info","No appointments"); return; }
    QMessageBox::information(this,"Processed", QString::fromStdString(p + " with " + d + " @ " + s));
    refreshAppointmentList();
}

void MainWindow::onGenerateBill() {
    bool ok;
    int pid = billPatientIdEdit->text().toInt(&ok);
    if (!ok) { QMessageBox::warning(this,"Error","Invalid Patient ID"); return; }
    Patient* p = pm.searchById(pid);
    if (!p) { QMessageBox::warning(this,"Error","Patient not found"); return; }
    int did = billDoctorSelect->currentData().toInt();
    double docFee = 0;
    if (did != 0) {
        Doctor* d = dm.searchById(did);
        if (!d) { QMessageBox::warning(this,"Error","Doctor not found"); return; }
        docFee = d->fee;
    }
    int days = billDaysSpin->value();
    double treat = billTreatmentSpin->value();
    double roomRatePerDay = 1500.0; // PKR
    double roomCharges = roomRatePerDay * days;
    double total = roomCharges + docFee + treat;
    bs.push(p->name, total);
    refreshBillHistory();
    QMessageBox::information(this,"Bill Generated", QString::fromStdString(p->name + " - " + formatPKR(total).toStdString()));
}

void MainWindow::onUndoBill() {
    std::string name; double amt; std::string ts;
    if (!bs.pop(name, amt, ts)) { QMessageBox::information(this,"Info","No billing history"); return; }
    refreshBillHistory();
    QMessageBox::information(this,"Undo", QString::fromStdString("Refunded " + name + " - " + std::to_string(amt)));
}

void MainWindow::onExportBillPdf() {
    BillRecord* top = bs.peek();
    if (!top) {
        QMessageBox::warning(this, "Error", "No bill to export! Generate a bill first.");
        return;
    }
    std::string name = top->patientName;
    double amount = top->amount;
    std::string timestamp = top->timestamp;

    QDir().mkpath("invoices");

    QString filename = "invoices/" + QString::fromStdString(name) + "_" +
                       QString::fromStdString(timestamp).replace(" ", "_").replace(":", "-") + ".pdf";

    QPdfWriter pdf(filename);
    pdf.setPageSize(QPageSize(QPageSize::A4));
    pdf.setPageMargins(QMarginsF(20, 20, 20, 20));

    QPainter painter(&pdf);
    QFont titleFont("Arial", 16, QFont::Bold);
    QFont subFont("Arial", 11);
    QFont boldFont("Arial", 10, QFont::Bold);

    painter.setFont(titleFont);
    painter.drawText(80, 80, "🏥 City Hospital - Invoice");

    painter.setFont(subFont);
    painter.drawText(80, 120, "Generated On: " + QString::fromStdString(timestamp));

    painter.drawText(80, 150, "---------------------------------------------------------");

    painter.setFont(boldFont);
    painter.drawText(80, 190, "Patient Name:");
    painter.setFont(subFont);
    painter.drawText(260, 190, QString::fromStdString(name));

    painter.setFont(boldFont);
    painter.drawText(80, 220, "Amount:");
    painter.setFont(subFont);
    painter.drawText(260, 220, formatPKR(amount));

    painter.setFont(boldFont);
    painter.drawText(80, 260, "Breakdown:");

    painter.setFont(subFont);
    painter.drawText(80, 295, "- Room Charges (1500/day): Included");
    painter.drawText(80, 320, "- Doctor Fee: Included");
    painter.drawText(80, 345, "- Treatment Charges: Included");

    painter.drawText(80, 380, "---------------------------------------------------------");

    painter.setFont(titleFont);
    painter.drawText(80, 420, "Total: " + formatPKR(amount));

    painter.end();

    QMessageBox::information(this, "Invoice Exported", "PDF invoice saved to:\n" + filename);
}
