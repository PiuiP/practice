#include <iostream>
#include <string>
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

int Client::total_clients = 0;

class Administrator {
public:
    void blockCard(CreditCard& card) {
        card.block();
    }
};

int testPaymentSystem() {
    Client client("NAruto");
    Administrator admin;

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

    return 0;
}

//OPTIONAL COURSE SYSTEM
//Сущности: Студент, Архив, Преподаватель, Курс, Оценка(?).
//Дей-я и инкапсуляция:
// - Студент: записаться на курс/курсы, получить оценку по курсу;
// - Преподаватель: создать курс/объявить запись на курс,поставить оценку;
// - Курс: название, продолжительность, список записанных студентов.
// - Оценка: по завершении курса сохраняется в архив;
// - Архив: сохранить курс-оценка-студент.
//ОСР: вероятно как-то связан с тем, как сохраняются данные о студентах, прошедших курсы, и их результаты.

class Student {
private:
    std::string name;
    int id;
public:
    Student(std::string n, int i) : name(n), id(i) {}
    std::string getName() const { return name; }
    int getId() const { return id; }
};

//как-то хранит студентов
class Course {
private:
    std::string name;
public:
    Course(std::string n) : name(n) {}
    std::string getName() const { return name; }
};

class IGradeStorage {
public:
    virtual void saveGrade(const Student& student, const Course& course, double grade) = 0;
    virtual ~IGradeStorage() = default;
};

//вариант реализации хранения оценок студентов - архив (по заданию)
class LocalArchive : public IGradeStorage {
private:
public:
};

//сюда передадим потом интерфейс хранилища, а не конркетную реализацию для принциипа открытости/закртости
class Teacher {
private:
    std::string name;
public:
};

int testOptionalCourseSystem() {
}



















int main(){
    //testPaymentSystem();
    testOptionalCourseSystem();
}
