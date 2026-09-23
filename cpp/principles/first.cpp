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
void printReport(const Report& report, PrinterType type) {
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

int main(){
    //first_part();
    //using_Report_methods();
}