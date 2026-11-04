#include <iostream>
#include <memory>
#include <string>
#include <vector>

// Двигатель, бензиновый двигатель, дизельный двигатель, реактивный двигатель.
//LSP: любой наследник Engine можно подставить вместо Engine, базовый контракт не ломается:
//      - start(): false, если двигатель уже запущен или нет топлива; иначе true;
//      - stop(): двигатель всегда останавливается;
// что-то еще хз вообще
//
// ISP: возможности разнесены по маленьким интерфейсам.
//
// DIP: высокоуровневые классы зависят от абстракций

//ISP: интерфейсы возможностей

// двигатель, который вращает вал (бензиновый, дизельный)
class IShaftEngine {
public:
    virtual double getTorque() const = 0;   // Н*м
    virtual double getRpm() const = 0;      // об/мин
    virtual ~IShaftEngine() = default;
};

// двигатель с реактивной тягой
class IThrustEngine {
public:
    virtual double getThrust() const = 0;   // кН типа
    virtual ~IThrustEngine() = default;
};

// обслуживание свечей зажигания (только бензиновый)
class ISparkPlugService {
public:
    virtual bool replaceSparkPlugs() = 0;
    virtual ~ISparkPlugService() = default;
};

// обслуживание свечей накаливания (только дизельный)
class IGlowPlugService {
public:
    virtual bool replaceGlowPlugs() = 0;
    virtual ~IGlowPlugService() = default;
};

// Общая логика (запуск, остановка, топливо) лежит здесь, наследники её не переопределяют.
class Engine {
protected:
    std::string name;
    double fuelLevel;
    bool running;


    virtual void onStart() {}

public:
    Engine(const std::string& n, double fuel) : name(n), fuelLevel(fuel), running(false) {}

    bool start() {
        if (running) {
            std::cout << "[" << name << "] already running" << std::endl;
            return false;
        }
        if (fuelLevel <= 0) {
            std::cout << "[" << name << "] no fuel, cannot start" << std::endl;
            return false;
        }
        onStart();
        running = true;
        std::cout << "[" << name << "] started" << std::endl;
        return true;
    }

    void stop() {
        running = false;
        std::cout << "[" << name << "] stopped" << std::endl;
    }

    bool refuel(double liters) {
        if (liters <= 0) {
            std::cout << "[" << name << "] wrong amount of fuel" << std::endl;
            return false;
        }
        fuelLevel += liters;
        std::cout << "[" << name << "] refueled: " << liters << ". Fuel: " << fuelLevel << std::endl;
        return true;
    }

    bool isRunning() const { return running; }
    double getFuelLevel() const { return fuelLevel; }
    const std::string& getName() const { return name; }

    virtual std::string getFuelType() const = 0;   // делает Engine абстрактным
    virtual ~Engine() = default;
};

//Конкретные двигатели

class PetrolEngine : public Engine, public IShaftEngine, public ISparkPlugService {
};

class DieselEngine : public Engine, public IShaftEngine, public IGlowPlugService {
};

class JetEngine : public Engine, public IThrustEngine {
};

//DIP: высокоуровневые классы. Хз пока, что с движками можно вообще придумать.


int main() {
}