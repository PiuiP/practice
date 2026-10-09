#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <map>
//PAYMENT SYSTEM
//Сущности: Клиент, Счет, Кредитная карта (КК), Заказ, Администратор.
//Дей-я и инкапсуляция:
// - Клиент: оплатить заказ, сделать перевод на другой счет;
// - Администратор: заблокировать КК;
// - КК: списать деньги, может быть заблокированной;
// - Счет: списать деньги;
// - Заказ: оплатить;
//ОСР: "оплатить Заказ" != "сделать платеж на другой Счет" -> Требуется гибкость при добавлении разныз видов оплат.
class IPaymentSource {
public:
    virtual bool deduct(double amount) = 0;
    virtual ~IPaymentSource() = default;
};

class IPaymentDestination {
public:
    virtual bool receive(double amount) = 0;
    virtual ~IPaymentDestination() = default;
};

class Order : public IPaymentDestination {
private:
    double amount;
    bool isPaid;
public:
    Order(double amt) : amount(amt), isPaid(false) {}
    
    bool receive(double paid) override {
        if (paid >= this->amount) {
            isPaid = true;
            std::cout << "[Order] Order is paid!" << std::endl;
            return true;
        }
        return false;
    }
};

class BankAccount : public IPaymentSource, public IPaymentDestination {
private:
    double balance;
public:
    BankAccount(double bal) : balance(bal) {}

    //реализация источника
    bool deduct(double amount) override {
        if (balance >= amount) {
            balance -= amount;
            std::cout << "[BankAccount] debit: " << amount << ". Remaining: " << balance << std::endl;
            return true;
        }
        std::cout << "[BankAccount] Insufficient funds!" << std::endl;
        return false;
    }
    //реализация получателя
    bool receive(double amount) override {
        balance += amount;
        std::cout << "[BankAccount] Recieved: " << amount << ". Balance: " << balance << std::endl;
        return true;
    }
};

class CreditCard : public IPaymentSource {
private:
    double limit;
    bool isBlocked;
public:
    CreditCard(double lim) : limit(lim), isBlocked(false) {}

    bool deduct(double amount) override {
        if (isBlocked) {
            std::cout << "[CreditCard] Card is banned!" << std::endl;
            return false;
        }
        if (limit >= amount) {
            limit -= amount;
            std::cout << "[CreditCard] is debited from the card. The limit: " << limit << std::endl;
            return true;
        }
        std::cout << "[CreditCard] Credit limit exceeded!" << std::endl;
        return false;
    }

    void block() { isBlocked = true; std::cout << "[CreditCard] The card is banned by administrator." << std::endl; }
};

class Client{
    private:
    std::string name;

    public:
    static int total_clients; //МЕСТО ВЫДЕЛИТЬ НЕ ЗАБУДЬ ПЖ

    Client() : name("Client_"+std::to_string(total_clients)) {
        total_clients++;
    }
    Client(std::string nameClient) : name(nameClient) {
        total_clients++;
    }

    bool transferMoney(IPaymentSource& source, IPaymentDestination& destination, double amount) {
        std::cout << "--- Client " << name << " init a payment " << amount << " ---" << std::endl;

        if (source.deduct(amount)) {
            return destination.receive(amount);
        }
        return false;
    }

    std::string getName() const { 
        return name; 
    }
};

int Client::total_clients = 1; //first client - 1, not 0;

class Administrator {
public:
    void blockCard(CreditCard& card) {
        card.block();
    }
};

int testPaymentSystem() {
    Client client("NAruto");
    Administrator admin;
    Client client2;

    CreditCard card(5000);
    BankAccount myAccount(10000);
    BankAccount friendAccount(0);
    Order myOrder(3000);

    //оплата заказа с Карты
    client.transferMoney(card, myOrder, 3000);

    //перевод со Счета на другой Счет
    client.transferMoney(myAccount, friendAccount, 5000);

    //администратор блокирует карту
    admin.blockCard(card);
    
    //попытка оплаты заблокированной картой -> упадет
    client.transferMoney(card, myOrder, 1000);

    //проверка статического атрибута класса
    std::cout << client2.getName();

    return 0;
}

//OPTIONAL COURSE SYSTEM
//Сущности: Студент, Архив, Преподаватель, Курс, Оценка(?).
//Дей-я и инкапсуляция:
// - Студент: записаться на курс/курсы, получить оценку по курсу;
// - Преподаватель: объявить запись на курс,поставить оценку;
// - Курс: название, продолжительность, список записанных студентов.
// - Оценка: по завершении курса сохраняется в архив;
// - Архив: сохранить курс-оценка-студент.
//ОСР: вероятно как-то связан с тем, как сохраняются данные о студентах, прошедших курсы, и их результаты.

class Course;

class Student {
private:
    std::string name;
public:
    Student(std::string n) : name(n) {}

    bool enrollToCourse(Course& course);  // тело ниже, после Course

    std::string getName() const { return name; }
};

class Course {
private:
    std::string name;
    int duration;
    std::vector<Student> students;
public:
    Course(std::string n, int d) : name(n), duration(d) {}

    bool hasStudent(const Student& s) const {
        return std::any_of(students.begin(), students.end(),
            [&](const Student& x) { return x.getName() == s.getName(); });
    }

    bool addStudent(const Student& s) {
        if (hasStudent(s)) {
            std::cout << s.getName() << " already in this course " << name << std::endl;
            return false;
        }
        students.push_back(s);
        return true;
    }

    std::string getINFO() const {
        return "Course Name: " + name + " Duration: " + std::to_string(duration) +
               " Enrolled students: " + std::to_string(students.size());
    }

    const std::vector<Student>& getListStudents() const { return students; }

    std::string getName() const { return name; }
};

bool Student::enrollToCourse(Course& course) {
    return course.addStudent(*this);
}

class IGradeStorage {
public:
    virtual void saveGrade(const Student& student, const Course& course, double grade) = 0;
    virtual ~IGradeStorage() = default;
};

class LocalArchive : public IGradeStorage {
private:
    std::map<std::string, std::map<std::string, double>> archive;
public:
    void saveGrade(const Student& student, const Course& course, double grade) override {
        archive[course.getName()][student.getName()] = grade;
    }

    void getINFO() const {
        for (const auto& course : archive) {
            std::cout << course.first << ":\n";
            for (const auto& student : course.second) {
                std::cout << "  " << student.first << ": " << student.second << "\n";
            }
        }
    }
};

//зависит от интерфейса хранилища, а не от LocalArchive
class Teacher {
private:
    std::string name;
    IGradeStorage& storage;
public:
    Teacher(std::string n, IGradeStorage& st) : name(n), storage(st) {}

    bool setGrade(const Student& student, const Course& course, double grade) {
        if (!course.hasStudent(student)) {
            std::cout << student.getName() << " not enrolled on this course " << course.getName() << std::endl;
            return false;
        }
        storage.saveGrade(student, course, grade);
        return true;
    }
};

int testOptionalCourseSystem() {
    LocalArchive archive;
    Teacher teacher("Ivanov", archive);

    Course course("C++", 40);
    Student alice("Alice");
    Student bob("Bob");

    alice.enrollToCourse(course);
    alice.enrollToCourse(course);  // повторная запись -> отказ

    teacher.setGrade(alice, course, 5.0);
    teacher.setGrade(bob, course, 4.0);  // Bob не записан -> отказ

    std::cout << course.getINFO() << std::endl;
    archive.getINFO();
    return 0;
}

//HOPITAL SYSTEM
//Сущности: Пациент, Лечащий Врач, Назначение, Другой врач/медсестра, Больница
//Дей-я и инкапсуляция:
// - Больница: Пациенты, Врачи, пациенты выписываются по окончании лечения/при нарушении режима/иное;
// - Назначение: процедуры, лекарства, операции;
// - Пациент: имеет ЛЕЧАЩЕГО врача, может быть забанен, получает назанчение от лечащего врача, получает процедуры от ДРУГОГО врача
// - Лечащий врач: делает назначение;
// - Другой врач: исполняет назанчения пациента.
//ОСР: ????????????????????????????????????????
//потенциально еще что-то может быть добавлено в назанчение или какие-то другие причины для выписки.

//назанчения могут пополняться, делаем через иинетрфейс
class IPrescrecption{
    public:
    virtual std::string describe() const = 0;
    virtual bool requiresDoctor() const = 0; //nurse can't do operations
    virtual ~IPrescrecption() = default;
};

//конкретные реализации назанчений
class Medication : public IPrescrecption{
    private:
    std::string drug;
    public:
    Medication(std::string nameDrug) : drug(nameDrug) {}
    std::string describe() const override {
        return "Medication: " + drug;
    }
    bool requiresDoctor() const override {
        return false;
    }
};

class Procedure : public IPrescrecption{
    private:
    std::string name;
    public:
    Procedure(std::string nameProcedure) : name(nameProcedure) {}
    std::string describe() const override {
        return "Procedure: " + name;
    }
    bool requiresDoctor() const override {
        return false;
    }
};

class Operation : public IPrescrecption{
    private:
    std::string name;
    public:
    Operation(std::string nameOperation) : name(nameOperation) {}
    std::string describe() const override {
        return "OPeration: " + name;
    }
    bool requiresDoctor() const override{
        return true;
    }
};

class Doctor;

class Patient {
    private:
    std::string name;
    Doctor* attending = nullptr;
    std::vector<std::shared_ptr<IPrescrecption>> prescriptions;
    bool discharged = false;
    public:
    Patient(std::string n) : name(n) {}

    void assignDoctor(Doctor* d) { attending = d; }
    Doctor* getDoctor() const { return attending; }

    void addPrescription(std::shared_ptr<IPrescrecption> p) { prescriptions.push_back(p); }
    const std::vector<std::shared_ptr<IPrescrecption>>& getPrescriptions() const { return prescriptions; }

    void discharge() { discharged = true; }
    bool isDischarged() const { return discharged; }
    std::string getName() const { return name; }
};

class Doctor {
    std::string name;
    public:
    Doctor(std::string n) : name(n) {}

    //назначать может только лечащий врач
    bool prescribe(Patient& patient, std::shared_ptr<IPrescrecption> p) {
        if (patient.getDoctor() != this) {
            std::cout << "[Doctor " << name << "] not attending doctor of " << patient.getName() << std::endl;
            return false;
        }
        patient.addPrescription(p);
        std::cout << "[Doctor " << name << "] prescribed to " << patient.getName() << ": " << p->describe() << std::endl;
        return true;
    }

    void execute(const IPrescrecption& p, const Patient& patient) {
        std::cout << "[Doctor " << name << "] " << patient.getName() << " <- " << p.describe() << std::endl;
    }
};

class Nurse {
    std::string name;
public:
    Nurse(std::string n) : name(n) {}

    void execute(const IPrescrecption& p, const Patient& patient) {
        if (p.requiresDoctor()) {
            std::cout << "[Nurse " << name << "] REJECTED: " << patient.getName() 
                      << " <- " << p.describe() << " (Requires Doctor!)" << std::endl;
            return;
        }
        std::cout << "[Nurse " << name << "] " << patient.getName() 
                  << " <- " << p.describe() << std::endl;
    }
};

class Hospital {
    std::vector<Patient*> patients;
public:
    void admit(Patient& patient, Doctor& attending) {
        patient.assignDoctor(&attending);
        patients.push_back(&patient);
        std::cout << "[Hospital] admitted " << patient.getName() << std::endl;
    }

    void discharge(Patient& patient, const std::string& reason) {
        patient.discharge();
        std::cout << "[Hospital] " << patient.getName() << " discharged: " << reason << std::endl;
    }
};

int testHospitalSystem() {
    Hospital hospital;
    Doctor house("House");
    Doctor wilson("Wilson");
    Nurse nurse("Anna");
    Patient patient("Ivan");

    hospital.admit(patient, house);

    house.prescribe(patient, std::make_shared<Medication>("Aspirin"));
    house.prescribe(patient, std::make_shared<Operation>("Appendectomy"));
    wilson.prescribe(patient, std::make_shared<Procedure>("Massage"));  //не лечащий -> отказ

    // исполняют назначения: медсестра и другой врач
    nurse.execute(*patient.getPrescriptions()[0], patient);
    wilson.execute(*patient.getPrescriptions()[1], patient);

    nurse.execute(*patient.getPrescriptions()[1], patient); //медсестра не может делать операции -> отказ

    hospital.discharge(patient, "treatment completed");
    return 0;
}


int main(){
    //testPaymentSystem();
    //testOptionalCourseSystem();
    testHospitalSystem();
}
