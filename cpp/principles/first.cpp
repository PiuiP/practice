#include <iostream>
#include <string>
#include <vector>

//interface(contract)
class IPrinter {
public:
    virtual void print(std::string text) = 0;  // pure virtual func: must be override, cannot create an obj of this classsss
    virtual ~IPrinter() = default;  // !виртуальный! деструктор
};

// РЕАЛИЗАЦИЯ 1
class ConsolePrinter : public IPrinter {
public:
    void print(std::string text) override {
        std::cout << text << std::endl;
    }
};

// РЕАЛИЗАЦИЯ №2
class PdfPrinter : public IPrinter {
public:
    void print(std::string text) override {
        //huinya
    }
};

//polymorphism
void generateReport(IPrinter* printer) { //принимает любой объект класса
    printer->print("Report is ready");  // dont know who is it.
}

class Base {
public:
    virtual void show() { std::cout << "Base" << std::endl; }
    // !НЕТ виртуального деструктора!
    ~Base() { std::cout << "Destructor Base" << std::endl; }
};

class Derived : public Base {
public:
    void show() override { std::cout << "Derived" << std::endl; }
    ~Derived() { std::cout << "Destructor Derived" << std::endl; }
};

int first_part() {
    ConsolePrinter cp;
    PdfPrinter pp;

    generateReport(&cp);  //Вызовет ConsolePrinter::print()
    generateReport(&pp);  //Вызовет PdfPrinter::print()

    Base* ptr = new Derived(); //указатель базового класса на объект наследника
    ptr->show();   // "Derived"(полиморфизм работает)
    delete ptr;    // "Деструктор Base" (Деструктор Derived НЕ вызвался!)

    return 0;
}

//SRP (Принцип единственной обязанности)
//НАРУШЕНИЕ ПРИНЦИИПА: Класс Report делает две вещи:
// формирует отчёт И печатает его. Две причины для изменения.
class Report {
private:
    std::string title;
    std::vector<std::string> data;

public:
    Report(const std::string& t) : title(t) {}

    void addData(const std::string& item) {
        data.push_back(item);
    }

    // Ответственность 1: ФОРМИРОВАНИЕ отчёта
    std::string generate() {
        std::string result = "=== " + title + " ===\n";
        for (const auto& item : data) {
            result += "- " + item + "\n";
        }
        return result;
    }

    // Ответственность 2: ПЕЧАТЬ на консоль
    void printToConsole() {
        std::cout << generate() << std::endl;
    }

    // Ответственность 3: СОХРАНЕНИЕ в файл
    void saveToFile(const std::string& filename) {
        std::cout << "Saved to " << filename << std::endl;
    }
};

int using_Report_methods() {
    Report report("Sells for month");
    report.addData("Goods A: 100");
    report.addData("Goods B: 50");
    report.printToConsole();
    return 0;
}

//соблюдение SRP: Каждая ответственность - в своём классе
//Так у каждого класса одна причина изменений: Report - меняется, если меняется структура данных
//ReportPrinter - меняется, если меняется способ вывода
//ReportFormatter - меняется, если меняется формат

//OCP (Принцип открытости/закрытости)
//НАРУШЕНИЕ ПРИНЦИИПА: Каждый новый тип печати = правка существующего кода.
enum class PrinterType {
    CONSOLE,
    FILE,
    EMAIL
};

class Report1 {
private:
    std::string content;
public:
    Report1(const std::string& c) : content(c) {}
    const std::string& getContent() const { return content; }
};

// чтобы добавить новый тип печати,
// нужно МЕНЯТЬ эту функцию (добавлять новый case) -> значит, хуйню сделали
void printReport(const Report1& report, PrinterType type) {
    switch (type) {
        case PrinterType::CONSOLE:
            std::cout << report.getContent() << std::endl;
            break;
        case PrinterType::FILE:
            std::cout << "[FILE] " << report.getContent() << std::endl;
            break;
        case PrinterType::EMAIL:
            std::cout << "[EMAIL] " << report.getContent() << std::endl;
            break;
        //Добавили PDF? Придётся менять эту функцию
    }
}

//соблюдение OCP: использовать абстракцию (интерфейс) и полиморфизм


// LSP (Принцип подстановки Лисков)
// НАРУШЕНИЕ LSP: на примере квадрат != прмоугольник.
class Rectangle {
protected:
    int width;
    int height;
public:
    Rectangle(int w, int h) : width(w), height(h) {}
    virtual void setWidth(int w) { width = w; }
    virtual void setHeight(int h) { height = h; }
    virtual int getArea() { return width * height; }
};

class Square : public Rectangle {
public:
    Square(int side) : Rectangle(side, side) {}
    
    // Нарушение: меняем и ширину, и высоту, ломая ожидаемое поведение Rectangle
    void setWidth(int w) override { width = w; height = w; }
    void setHeight(int h) override { width = h; height = h; }
};

// Функция, которая ожидает поведение Rectangle
void testLSPViolation(Rectangle& r) {
    r.setWidth(5);
    r.setHeight(4);
    // Ожидаем площадь 20. Но если передадим Square, площадь будет 16! 
    // Подтип (Square) сломал логику базового типа (Rectangle).
}

void test_LSP() {
    Rectangle rect(0, 0);
    testLSPViolation(rect); //корректно
    
    Square sq(0);
    testLSPViolation(sq);   //площадь будет 16 вместо 20. Нарушение LSP!
}


// ISP (принцип разделения интерфейса)
// НАРУШЕНИЕ ISP: интерфейс, заставляющий реализовывать ненужное
class IWorkerFat {
public:
    virtual void work() = 0;
    virtual void eat() = 0;  // Роботу это не нужно!
    virtual ~IWorkerFat() = default;
};

class HumanWorker : public IWorkerFat {
public:
    void work() override { std::cout << "Человек работает\n"; }
    void eat() override { std::cout << "Человек ест\n"; }
};

class RobotWorkerBad : public IWorkerFat {
public:
    void work() override { std::cout << "Робот работает\n"; }
    void eat() override { 
        // Нарушение: вынуждены реализовывать метод, который нам не нужен
        throw std::runtime_error("Роботы не едят! Ошибка интерфейса."); 
    }
};


// СОБЛЮДЕНИЕ ISP: Разделяем на специализированные интерфейсы
class IWorkable {
public:
    virtual void work() = 0;
    virtual ~IWorkable() = default;
};

class IEatable {
public:
    virtual void eat() = 0;
    virtual ~IEatable() = default;
};

class HumanWorkerGood : public IWorkable, public IEatable {
public:
    void work() override { std::cout << "Человек работает\n"; }
    void eat() override { std::cout << "Человек ест\n"; }
};

class RobotWorkerGood : public IWorkable { 
    // Робот реализует только то, что ему нужно. Интерфейс IEatable его не касается.
public:
    void work() override { std::cout << "Робот работает эффективно\n"; }
};

void test_ISP() {
    HumanWorkerGood human;
    RobotWorkerGood robot;
    
    human.work();
    human.eat();
    
    robot.work();
    // robot.eat(); // Ошибка компиляции! И это гуд, интерфейс не заставляет робота есть.
}

// DIP (Принцип инверсии зависимостей)
// АБСТРАКЦИЯ (низкоуровневый модуль зависит от абстракции)
class IMessageSender {
public:
    virtual void send(const std::string& message) = 0;
    virtual ~IMessageSender() = default;
};

// КОНКРЕТНЫЕ РЕАЛИЗАЦИИ
class EmailSender : public IMessageSender {
public:
    void send(const std::string& message) override {
        std::cout << "[EMAIL] Отправка: " << message << std::endl;
    }
};

class SMSSender : public IMessageSender {
public:
    void send(const std::string& message) override {
        std::cout << "[SMS] Отправка: " << message << std::endl;
    }
};

// ВЫСОКОУРОВНЕВЫЙ МОДУЛЬ зависит от АБСТРАКЦИИ, а не от конкретики
class NotificationService {
private:
    IMessageSender* sender; // Зависимость от интерфейса (абстракции)!
public:
    // Внедрение зависимости через конструктор (Dependency Injection)
    NotificationService(IMessageSender* s) : sender(s) {}
    
    void notify(const std::string& msg) {
        // Высокоуровневая логика не знает, как именно отправляется сообщение
        sender->send(msg);
    }
};


void test_DIP() {
    EmailSender email;
    SMSSender sms;
    
    NotificationService serviceByEmail(&email);
    serviceByEmail.notify("Срочное сообщение!");
    
    NotificationService serviceBySMS(&sms);
    serviceBySMS.notify("Срочное сообщение!");
}



int main(){
    //first_part();
    //using_Report_methods();
    //test_LSP();
    //test_ISP();
    test_DIP();
}