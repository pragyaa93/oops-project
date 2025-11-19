#include "Hospital.h"
#include "CSVUtils.h"
#include <stdexcept>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

// --------------------------------------------------
//  INTERNAL BILLING FUNCTION (based on doctor specialty)
// --------------------------------------------------
static double getConsultationFeeBase(const std::string &specialty) {
    if (specialty == "Cardiology") return 800;
    if (specialty == "Neurology") return 900;
    if (specialty == "Orthopedics") return 700;
    if (specialty == "Dermatology") return 500;
    if (specialty == "Gynecology") return 600;
    if (specialty == "General Medicine") return 400;
    if (specialty == "Oncology") return 850;
    if (specialty == "Pediatrics") return 500;
    if (specialty == "ENT") return 450;
    if (specialty == "Ophthalmology") return 550;
    if (specialty == "Endocrinology") return 700;
    if (specialty == "Nephrology") return 750;
    if (specialty == "Gastroenterology") return 720;
    if (specialty == "Pulmonology") return 650;
    if (specialty == "Urology") return 680;
    if (specialty == "Rheumatology") return 670;
    // reasonable defaults for specialties not listed
    if (specialty == "Physiotherapy") return 500;
    if (specialty == "General Surgery") return 600;
    if (specialty == "Psychiatry") return 500;
    return 500; // default
}

// --------------------------------------------------
//  PATIENTS
// --------------------------------------------------

Patient Hospital::addPatient(const std::string &name, int age, const std::string &gender, const std::string &contact) {
    Patient p(nextPatientId++, name, age, gender, contact);
    patients.push_back(p);
    return p;
}

bool Hospital::editPatient(int id, const std::string &name, int age, const std::string &gender, const std::string &contact) {
    for (auto &p : patients) {
        if (p.getId() == id) {
            p.setName(name);
            p.setAge(age);
            p.setGender(gender);
            p.setContact(contact);
            return true;
        }
    }
    return false;
}

bool Hospital::deletePatient(int id) {
    auto it = std::remove_if(patients.begin(), patients.end(), [&](const Patient &p){ return p.getId() == id; });
    if (it != patients.end()) {
        patients.erase(it, patients.end());
        // also remove any appointments for this patient (simple cleanup)
        appointments.erase(std::remove_if(appointments.begin(), appointments.end(),
            [&](const Appointment &a){ return a.getPatientId() == id; }), appointments.end());
        return true;
    }
    return false;
}

std::optional<Patient> Hospital::findPatientById(int id) const {
    for (const auto &p : patients)
        if (p.getId() == id) return p;
    return std::nullopt;
}

std::vector<Patient> Hospital::searchPatientsByName(const std::string &q) const {
    std::vector<Patient> out;
    std::string lowq = q;
    std::transform(lowq.begin(), lowq.end(), lowq.begin(), ::tolower);
    for (const auto &p : patients) {
        std::string n = p.getName();
        std::transform(n.begin(), n.end(), n.begin(), ::tolower);
        if (n.find(lowq) != std::string::npos) out.push_back(p);
    }
    return out;
}

// --------------------------------------------------
//  DOCTORS
// --------------------------------------------------

Doctor Hospital::addDoctor(const std::string &name, const std::string &spec, const std::string &contact) {
    Doctor d(nextDoctorId++, name, spec, contact);
    doctors.push_back(d);
    return d;
}

bool Hospital::editDoctor(int id, const std::string &name, const std::string &spec, const std::string &contact) {
    for (auto &d : doctors) {
        if (d.getId() == id) {
            d.setName(name);
            d.setSpecialty(spec);
            d.setContact(contact);
            return true;
        }
    }
    return false;
}

bool Hospital::deleteDoctor(int id) {
    auto it = std::remove_if(doctors.begin(), doctors.end(), [&](const Doctor &d){ return d.getId() == id; });
    if (it != doctors.end()) {
        doctors.erase(it, doctors.end());
        // remove appointments for that doctor
        appointments.erase(std::remove_if(appointments.begin(), appointments.end(),
            [&](const Appointment &a){ return a.getDoctorId() == id; }), appointments.end());
        return true;
    }
    return false;
}

std::optional<Doctor> Hospital::findDoctorById(int id) const {
    for (const auto &d : doctors)
        if (d.getId() == id) return d;
    return std::nullopt;
}

std::vector<Doctor> Hospital::searchDoctorsByName(const std::string &q) const {
    std::vector<Doctor> out;
    std::string lowq = q;
    std::transform(lowq.begin(), lowq.end(), lowq.begin(), ::tolower);
    for (const auto &d : doctors) {
        std::string n = d.getName();
        std::transform(n.begin(), n.end(), n.begin(), ::tolower);
        if (n.find(lowq) != std::string::npos) out.push_back(d);
    }
    return out;
}

// --------------------------------------------------
//  APPOINTMENTS
// --------------------------------------------------

Appointment Hospital::bookAppointment(int patientId, int doctorId, const std::string &date, const std::string &time) {
    if (!findPatientById(patientId)) throw std::runtime_error("Patient not found");
    auto doc = findDoctorById(doctorId);
    if (!doc)   throw std::runtime_error("Doctor not found");

    // Check for clash: same doctor, same date, same time
    for (const auto &a : appointments) {
        if (a.getDoctorId() == doctorId && a.getDate() == date && a.getTime() == time) {
            throw std::runtime_error("Doctor not available at the chosen date/time (clash detected).");
        }
    }

    Appointment a(nextAppointmentId++, patientId, doctorId, date, time);
    appointments.push_back(a);
    return a;
}

const std::vector<Appointment> &Hospital::getAllAppointments() const {
    return appointments;
}

// --------------------------------------------------
//  BILLING
// --------------------------------------------------

Billing Hospital::generateBill(int appointmentId) {
    // find appointment
    const Appointment *ap = nullptr;
    for (const auto &a : appointments) {
        if (a.getId() == appointmentId) {
            ap = &a;
            break;
        }
    }
    if (!ap) throw std::runtime_error("Appointment not found");

    // find doctor
    auto docopt = findDoctorById(ap->getDoctorId());
    if (!docopt) throw std::runtime_error("Doctor not found");

    double base = getConsultationFeeBase(docopt->getSpecialty());
    double gst = 0.18 * base;
    double total = base + gst;

    // round to nearest rupee (integer)
    double totalRounded = std::round(total);

    Billing b(
        nextBillId++,
        ap->getId(),
        docopt->getId(),
        totalRounded,
        "Consultation Fee (incl. GST 18%)",
        ap->getDate()
    );

    bills.push_back(b);
    return b;
}

const std::vector<Billing> &Hospital::getAllBills() const {
    return bills;
}

// --------------------------------------------------
// LOAD (CSV)
// --------------------------------------------------

void Hospital::loadPatients(const std::string &file) {
    auto rows = CSV::readCSV(file);
    patients.clear();
    nextPatientId = 1;
    for (auto &r : rows) {
        if (r.size() < 5) continue;
        int id = 0;
        try { id = std::stoi(r[0]); } catch(...) { continue; }
        patients.emplace_back(id, r[1], std::stoi(r[2]), r[3], r[4]);
        if (id >= nextPatientId) nextPatientId = id + 1;
    }
    std::cout << "Loaded " << patients.size() << " patients.\n";
}

void Hospital::loadDoctors(const std::string &file) {
    auto rows = CSV::readCSV(file);
    doctors.clear();
    nextDoctorId = 1;
    for (auto &r : rows) {
        if (r.size() < 4) continue;
        int id = 0;
        try { id = std::stoi(r[0]); } catch(...) { continue; }
        doctors.emplace_back(id, r[1], r[2], r[3]);
        if (id >= nextDoctorId) nextDoctorId = id + 1;
    }
    std::cout << "Loaded " << doctors.size() << " doctors.\n";
}

void Hospital::loadAppointments(const std::string &file) {
    auto rows = CSV::readCSV(file);
    appointments.clear();
    nextAppointmentId = 1;
    for (auto &r : rows) {
        if (r.size() < 5) continue;
        int id = 0;
        try { id = std::stoi(r[0]); } catch(...) { continue; }
        appointments.emplace_back(id, std::stoi(r[1]), std::stoi(r[2]), r[3], r[4]);
        if (id >= nextAppointmentId) nextAppointmentId = id + 1;
    }
    std::cout << "Loaded " << appointments.size() << " appointments.\n";
}

void Hospital::loadBilling(const std::string &file) {
    auto rows = CSV::readCSV(file);
    bills.clear();
    nextBillId = 1;
    for (auto &r : rows) {
        if (r.size() < 6) continue;
        int id = 0;
        try { id = std::stoi(r[0]); } catch(...) { continue; }
        double amount = 0.0;
        try { amount = std::stod(r[3]); } catch(...) { amount = 0.0; }
        bills.emplace_back(id, std::stoi(r[1]), std::stoi(r[2]), amount, r[4], r[5]);
        if (id >= nextBillId) nextBillId = id + 1;
    }
    std::cout << "Loaded " << bills.size() << " bills.\n";
}

// --------------------------------------------------
// SAVE (CSV)
// --------------------------------------------------

void Hospital::savePatients(const std::string &file) {
    std::vector<std::string> header = {"id","name","age","gender","contact"};
    std::vector<std::vector<std::string>> rows;
    for (const auto &p : patients)
        rows.push_back({std::to_string(p.getId()), p.getName(), std::to_string(p.getAge()), p.getGender(), p.getContact()});
    CSV::writeCSV(file, header, rows);
}

void Hospital::saveDoctors(const std::string &file) {
    std::vector<std::string> header = {"id","name","specialty","contact"};
    std::vector<std::vector<std::string>> rows;
    for (const auto &d : doctors)
        rows.push_back({std::to_string(d.getId()), d.getName(), d.getSpecialty(), d.getContact()});
    CSV::writeCSV(file, header, rows);
}

void Hospital::saveAppointments(const std::string &file) {
    std::vector<std::string> header = {"id","patientId","doctorId","date","time"};
    std::vector<std::vector<std::string>> rows;
    for (const auto &a : appointments)
        rows.push_back({std::to_string(a.getId()), std::to_string(a.getPatientId()), std::to_string(a.getDoctorId()), a.getDate(), a.getTime()});
    CSV::writeCSV(file, header, rows);
}

void Hospital::saveBilling(const std::string &file) {
    std::vector<std::string> header = {"billId","appointmentId","doctorId","amount","description","date"};
    std::vector<std::vector<std::string>> rows;
    for (const auto &b : bills)
        rows.push_back({std::to_string(b.getBillId()), std::to_string(b.getAppointmentId()), std::to_string(b.getDoctorId()), std::to_string(static_cast<long long>(b.getAmount())), b.getDescription(), b.getDate()});
    CSV::writeCSV(file, header, rows);
}
