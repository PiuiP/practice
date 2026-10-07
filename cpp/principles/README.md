# Принципы SOLID и паттерны проектирования

## Теория

### Что такое SOLID?
SOLID — это аббревиатура из 5 принципов объектно-ориентированного программирования, которые делают код более гибким, поддерживаемым и расширяемым.

---

## 1 SRP — Single Responsibility Principle
**Принцип единственной ответственности**

> У класса должна быть только **одна причина для изменения**

### Основная идея
- Каждый объект должен иметь **одну ответственность**
- Эта ответственность должна быть полностью инкапсулирована в класс
- Все поведения класса направлены исключительно на обеспечение этой ответственности

### Пример нарушения (файл `first.cpp`, класс `Report`)
```cpp
class Report {
private:
    std::string title;
    std::vector<std::string> data;

public:
    Report(const std::string& t) : title(t) {}

    void addData(const std::string& item) { data.push_back(item); }

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
```
**Проблема:** класс меняется по **трём разным причинам** (изменился формат данных / способ вывода / место сохранения).

### Как исправить
Каждая ответственность выносится в свой класс:
- `Report` — меняется, только если меняется структура данных
- `ReportPrinter` — меняется, только если меняется способ вывода
- `ReportFormatter` — меняется, только если меняется формат

---

## 2 OCP — Open/Closed Principle
**Принцип открытости/закрытости**

> Программные сущности должны быть **открыты для расширения**, но **закрыты для изменения**

### Основная идея
- **Открыты для расширения**: можно добавлять новое поведение через создание новых типов
- **Закрыты для изменения**: при расширении не нужно править существующий код

### Пример нарушения (файл `first.cpp`, функция `printReport`)
```cpp
enum class PrinterType {
    CONSOLE,
    FILE,
    EMAIL
};

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
        // Добавили PDF? Придётся менять эту функцию
    }
}
```
**Проблема:** каждый новый тип печати требует правки уже работающей функции.

### Пример соблюдения (файл `first.cpp`, интерфейс `IPrinter`)
```cpp
// АБСТРАКЦИЯ (интерфейс)
class IPrinter {
public:
    virtual void print(std::string text) = 0;
    virtual ~IPrinter() = default;
};

// Конкретные реализации — РАСШИРЯЕМ, не меняя старое
class ConsolePrinter : public IPrinter {
public:
    void print(std::string text) override {
        std::cout << text << std::endl;
    }
};

class PdfPrinter : public IPrinter {
public:
    void print(std::string text) override { /* печать в PDF */ }
};

// Клиентский код ЗАКРЫТ для изменений
void generateReport(IPrinter* printer) {
    printer->print("Report is ready");  // Не важно, кто это!
}
```

**Результат:** чтобы добавить новый тип печати, просто создаём новый класс, **не трогая** существующий код.

---

## Ключевые концепции в коде

### Интерфейс (контракт)
```cpp
class IPrinter {
public:
    virtual void print(std::string text) = 0;  // Чистая виртуальная функция
    virtual ~IPrinter() = default;
};
```
- `= 0` по сути работает, как абстрактный метод в питоне;
- Нельзя создать объект класса с чистыми виртуальными функциями;
- Это и есть **интерфейс** в C++.

### Виртуальный деструктор
```cpp
virtual ~IPrinter() = default;  // ОБЯЗАТЕЛЬНО!
```
Если удалить объект через указатель базового класса без `virtual` деструктора, вызовется только деструктор базового класса (утечка памяти).

Демонстрация проблемы в `first.cpp` (у `Base` деструктор намеренно **не** виртуальный):
```cpp
class Base {
public:
    virtual void show() { std::cout << "Base" << std::endl; }
    ~Base() { std::cout << "Destructor Base" << std::endl; }  // НЕ virtual
};

class Derived : public Base {
public:
    void show() override { std::cout << "Derived" << std::endl; }
    ~Derived() { std::cout << "Destructor Derived" << std::endl; }
};
```

### Полиморфизм
```cpp
Base* ptr = new Derived();  // Указатель базового класса на объект наследника
ptr->show();                // "Derived" — вызовется Derived::show() благодаря virtual
delete ptr;                 // "Destructor Base" — деструктор Derived НЕ вызвался!
```

Полиморфизм через интерфейс (функция `first_part()`):
```cpp
ConsolePrinter cp;
PdfPrinter pp;

generateReport(&cp);  // Вызовет ConsolePrinter::print()
generateReport(&pp);  // Вызовет PdfPrinter::print()
```

---

## Остальные принципы SOLID

## 3 LSP — Liskov Substitution Principle
**Принцип подстановки Барбары Лисков**

> Функции, которые используют базовый тип, должны иметь возможность использовать подтипы базового типа, не зная об этом

**Пример:** если функция работает с `IPrinter*`, она должна корректно работать с **любым** наследником (`ConsolePrinter`, `PdfPrinter` и т.д.).

### Пример нарушения (файл `first.cpp`, классы `Rectangle` и `Square`)
Классическая ошибка: `Square` наследуется от `Rectangle`, но меняет поведение методов базового класса, ломая ожидания клиента.
```cpp
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
}
```
Проверка (функция `test_LSP()`):
```cpp
Rectangle rect(0, 0);
testLSPViolation(rect);  // корректно, площадь 20

Square sq(0);
testLSPViolation(sq);    // площадь 16 вместо 20 — нарушение LSP
```
**Проблема:** `Square` нельзя безопасно подставить вместо `Rectangle`, значит наследование здесь выбрано неверно.

---

## 4 ISP — Interface Segregation Principle
**Принцип разделения интерфейса**

> Много специализированных интерфейсов лучше, чем один универсальный

**Пример:** лучше сделать `IPrintable`, `ISavable`, `IEditable` отдельно, чем один огромный интерфейс со всеми методами.

### Пример нарушения (файл `first.cpp`, интерфейс `IWorkerFat`)
«Жирный» интерфейс заставляет классы реализовывать ненужные им методы.
```cpp
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
```

### Как исправить (файл `first.cpp`, интерфейсы `IWorkable` и `IEatable`)
Разделяем на специализированные интерфейсы.
```cpp
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

// Человек реализует оба интерфейса
class HumanWorkerGood : public IWorkable, public IEatable {
public:
    void work() override { std::cout << "Человек работает\n"; }
    void eat() override { std::cout << "Человек ест\n"; }
};

// Робот реализует только то, что ему нужно
class RobotWorkerGood : public IWorkable {
public:
    void work() override { std::cout << "Робот работает эффективно\n"; }
};
```
Проверка (функция `test_ISP()`):
```cpp
HumanWorkerGood human;
RobotWorkerGood robot;

human.work();
human.eat();

robot.work();
// robot.eat();  // Ошибка компиляции — и это хорошо: интерфейс не заставляет робота есть
```

---

## 5 DIP — Dependency Inversion Principle
**Принцип инверсии зависимостей**

> Зависимость на абстракциях, а не на конкретных классах

**Пример:**
```cpp
// Плохо: зависимость от конкретного класса
class Report {
    ConsolePrinter printer;  // Жёсткая привязка
};

// Хорошо: зависимость от абстракции
class Report {
    IPrinter* printer;  // Можно подставить любую реализацию
};
```

### Пример соблюдения (файл `first.cpp`, класс `NotificationService`)
Высокоуровневый модуль (`NotificationService`) зависит от абстракции (`IMessageSender`), а не от конкретных реализаций (`EmailSender`, `SMSSender`).
```cpp
// Абстракция
class IMessageSender {
public:
    virtual void send(const std::string& message) = 0;
    virtual ~IMessageSender() = default;
};

// Конкретные реализации
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

// Высокоуровневый модуль зависит от интерфейса
class NotificationService {
private:
    IMessageSender* sender;  // Зависимость от абстракции!
public:
    // Внедрение зависимости через конструктор (Dependency Injection)
    NotificationService(IMessageSender* s) : sender(s) {}

    void notify(const std::string& msg) {
        // Высокоуровневая логика не знает, как именно отправляется сообщение
        sender->send(msg);
    }
};
```
Проверка (функция `test_DIP()`):
```cpp
EmailSender email;
SMSSender sms;

NotificationService serviceByEmail(&email);
serviceByEmail.notify("Срочное сообщение!");

NotificationService serviceBySMS(&sms);
serviceBySMS.notify("Срочное сообщение!");
```
**Результат:** чтобы отправлять уведомления по-другому (Telegram, Push), достаточно написать новый класс-наследник `IMessageSender`, а `NotificationService` менять не нужно.

---

## Связь с паттернами GoF

Принципы SOLID реализуются через паттерны:

| Принцип | Паттерны |
|---------|----------|
| **SRP** | Strategy, Observer |
| **OCP** | Strategy, Factory Method, Decorator |
| **LSP** | Все паттерны, использующие наследование |
| **ISP** | Adapter, Facade |
| **DIP** | Dependency Injection, Factory |

---

## Структура файлов

- `first.cpp` — примеры кода:
  - `first_part()` — демонстрация полиморфизма и виртуальных функций (`IPrinter`, `ConsolePrinter`, `PdfPrinter`, `Base`, `Derived`)
  - `Report`, `using_Report_methods()` — пример нарушения SRP
  - `PrinterType`, `Report1`, `printReport()` — пример нарушения OCP
  - `IPrinter` и реализации — пример соблюдения OCP
  - `Rectangle`, `Square`, `testLSPViolation()`, `test_LSP()` — пример нарушения LSP
  - `IWorkerFat`, `RobotWorkerBad` — пример нарушения ISP
  - `IWorkable`, `IEatable`, `HumanWorkerGood`, `RobotWorkerGood`, `test_ISP()` — пример соблюдения ISP
  - `IMessageSender`, `EmailSender`, `SMSSender`, `NotificationService`, `test_DIP()` — пример соблюдения DIP

---