#pragma once
#include <string>
#include <iostream>

class Doctor {
private:
    int id;
    std::string name;
    std::string specialty;
    std::string contact;

public:
    Doctor() : id(0), name(), specialty(), contact() {}

    Doctor(int id, const std::string &name, const std::string &specialty, const std::string &contact)
        : id(id), name(name), specialty(specialty), contact(contact) {}

    int getId() const noexcept { return id; }
    const std::string &getName() const noexcept { return name; }
    const std::string &getSpecialty() const noexcept { return specialty; }
    const std::string &getContact() const noexcept { return contact; }

    void setName(const std::string &n) { name = n; }
    void setSpecialty(const std::string &s) { specialty = s; }
    void setContact(const std::string &c) { contact = c; }

    inline friend std::ostream &operator<<(std::ostream &os, const Doctor &d) {
        os << "Doctor[ID=" << d.id
           << ", Name=" << d.name
           << ", Specialty=" << d.specialty
           << ", Contact=" << d.contact << "]";
        return os;
    }
};
