#include <iostream>
#include <string>
#include <limits>
#include "Hospital.h"

static void pause() {
    std::cout << "Press Enter to continue...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int readInt(const std::string &prompt) {
    int x;
    while (true) {
        std::cout << prompt;
        if (std::cin >> x) {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return x;
        } else {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Try again.\n";
        }
    }
}

static std::string readLine(const std::string &prompt) {
    std::string s;
    std::cout << prompt;
    std::getline(std::cin, s);
    return s;
}

int main() {
    Hospital hosp;
    try {
        hosp.loadPatients("data/patients.csv");
        hosp.loadDoctors("data/doctors.csv");
        hosp.loadAppointments("data/appointments.csv");
        hosp.loadBilling("data/billing.csv");
        std::cout << "\n--- DATABASE LOADED SUCCESSFULLY ---\n\n";
    } catch (const std::exception &ex) {
        std::cerr << "Error loading data: " << ex.what() << std::endl;
        return 1;
    }

    while (true) {
        std::cout << "\n--- HOSPITAL MANAGEMENT ---\n";
        std::cout << "1. List Patients\n2. Add Patient\n3. Edit Patient\n4. Delete Patient\n5. Search Patients by name\n";
        std::cout << "6. List Doctors\n7. Add Doctor\n8. Edit Doctor\n9. Delete Doctor\n10. Search Doctors by name\n";
        std::cout << "11. List Appointments\n12. Book Appointment\n13. Generate Bill for Appointment\n14. List Bills\n15. Save & Exit\n";
        int choice = readInt("Choose option: ");

        try {
            if (choice == 1) {
                auto &apts = hosp.getAllAppointments(); // just to ensure compiled; patients listing next
                std::cout << "\nPatients:\n";
                // We don't have a getter for patients; list indirectly via CSV or reflection. To keep simple, load then search by id 1..nextId-1
                // But Hospital doesn't expose nextPatientId publicly; so we will search by printing by searching name wildcard
                std::string q = "";
                auto results = hosp.searchPatientsByName(q); // q="" returns everyone because we check contains; but implementation lower-cases and looks for q in name -> empty matches all
                for (const auto &p : results) std::cout << p << "\n";
                pause();
            }
            else if (choice == 2) {
                std::string name = readLine("Name: ");
                int age = readInt("Age: ");
                std::string gender = readLine("Gender: ");
                std::string contact = readLine("Contact: ");
                auto p = hosp.addPatient(name, age, gender, contact);
                std::cout << "Added: " << p << "\n";
                pause();
            }
            else if (choice == 3) {
                int id = readInt("Patient ID to edit: ");
                std::string name = readLine("New Name: ");
                int age = readInt("New Age: ");
                std::string gender = readLine("New Gender: ");
                std::string contact = readLine("New Contact: ");
                if (hosp.editPatient(id, name, age, gender, contact)) std::cout << "Patient edited.\n";
                else std::cout << "Patient not found.\n";
                pause();
            }
            else if (choice == 4) {
                int id = readInt("Patient ID to delete: ");
                if (hosp.deletePatient(id)) std::cout << "Patient deleted.\n";
                else std::cout << "Patient not found.\n";
                pause();
            }
            else if (choice == 5) {
                std::string q = readLine("Search name: ");
                auto res = hosp.searchPatientsByName(q);
                if (res.empty()) std::cout << "No patients found.\n";
                else for (auto &p : res) std::cout << p << "\n";
                pause();
            }
            else if (choice == 6) {
                std::cout << "\nDoctors:\n";
                auto res = hosp.searchDoctorsByName(""); // empty -> list all
                for (auto &d : res) std::cout << d << "\n";
                pause();
            }
            else if (choice == 7) {
                std::string name = readLine("Name: ");
                std::string spec = readLine("Specialty: ");
                std::string contact = readLine("Contact: ");
                auto d = hosp.addDoctor(name, spec, contact);
                std::cout << "Added: " << d << "\n";
                pause();
            }
            else if (choice == 8) {
                int id = readInt("Doctor ID to edit: ");
                std::string name = readLine("New Name: ");
                std::string spec = readLine("New Specialty: ");
                std::string contact = readLine("New Contact: ");
                if (hosp.editDoctor(id, name, spec, contact)) std::cout << "Doctor edited.\n";
                else std::cout << "Doctor not found.\n";
                pause();
            }
            else if (choice == 9) {
                int id = readInt("Doctor ID to delete: ");
                if (hosp.deleteDoctor(id)) std::cout << "Doctor deleted.\n";
                else std::cout << "Doctor not found.\n";
                pause();
            }
            else if (choice == 10) {
                std::string q = readLine("Search name: ");
                auto res = hosp.searchDoctorsByName(q);
                if (res.empty()) std::cout << "No doctors found.\n";
                else for (auto &d : res) std::cout << d << "\n";
                pause();
            }
            else if (choice == 11) {
                std::cout << "\nAppointments:\n";
                for (const auto &a : hosp.getAllAppointments()) std::cout << a << "\n";
                pause();
            }
            else if (choice == 12) {
                int pid = readInt("Patient ID: ");
                int did = readInt("Doctor ID: ");
                std::string date = readLine("Date (YYYY-MM-DD): ");
                std::string time = readLine("Time (HH:MM): ");
                try {
                    auto ap = hosp.bookAppointment(pid, did, date, time);
                    std::cout << "Booked: " << ap << "\n";
                } catch (const std::exception &ex) {
                    std::cout << "Failed to book appointment: " << ex.what() << "\n";
                }
                pause();
            }
            else if (choice == 13) {
                int aid = readInt("Appointment ID to bill: ");
                try {
                    auto b = hosp.generateBill(aid);
                    std::cout << "Generated bill: " << b << "\n";
                } catch (const std::exception &ex) {
                    std::cout << "Failed to generate bill: " << ex.what() << "\n";
                }
                pause();
            }
            else if (choice == 14) {
                std::cout << "\nBills:\n";
                for (const auto &b : hosp.getAllBills()) std::cout << b << "\n";
                pause();
            }
            else if (choice == 15) {
                hosp.savePatients("data/patients.csv");
                hosp.saveDoctors("data/doctors.csv");
                hosp.saveAppointments("data/appointments.csv");
                hosp.saveBilling("data/billing.csv");
                std::cout << "Saved. Exiting.\n";
                break;
            }
            else {
                std::cout << "Unknown option.\n";
            }
        } catch (const std::exception &ex) {
            std::cout << "Error: " << ex.what() << "\n";
            pause();
        }
    }

    return 0;
}
