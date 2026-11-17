#include <iostream>
#include <memory>
#include <string>
#include <type_traits>
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
// двигатель с вращающимся валом (бензиновый, дизельный)
class IShaftEngine {
public:
    virtual double getTorque() const = 0;   // Н*м
    virtual double getRpm() const = 0;      // об/мин
    virtual ~IShaftEngine() = default;
};

// двигатель с реактивной тягой
class IThrustEngine {
public:
    virtual double getThrust() const = 0;   // кН
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

//LSP: базовый класс

class Engine {
    std::string name;
    double fuelLevel;
    bool running = false;
public:
    Engine(const std::string& n, double fuel) : name(n), fuelLevel(fuel) {}
    virtual ~Engine() = default;

    bool start() {
        if (running) {
            std::cout << "[" << name << "] already running" << std::endl;
            return false;
        }
        if (fuelLevel <= 0) {
            std::cout << "[" << name << "] no fuel, cannot start" << std::endl;
            return false;
        }
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
};

//Конкретные двигатели

class PetrolEngine : public Engine, public IShaftEngine, public ISparkPlugService {
public:
    using Engine::Engine;

    std::string getFuelType() const override { return "petrol"; }
    double getTorque() const override { return isRunning() ? 150.0 : 0.0; }
    double getRpm() const override { return isRunning() ? 3000.0 : 0.0; }

    bool replaceSparkPlugs() override {
        if (isRunning()) {
            std::cout << "[" << getName() << "] stop the engine before replacing spark plugs" << std::endl;
            return false;
        }
        std::cout << "[" << getName() << "] spark plugs replaced" << std::endl;
        return true;
    }
};

class DieselEngine : public Engine, public IShaftEngine, public IGlowPlugService {
public:
    using Engine::Engine;

    std::string getFuelType() const override { return "diesel"; }
    double getTorque() const override { return isRunning() ? 400.0 : 0.0; }
    double getRpm() const override { return isRunning() ? 2500.0 : 0.0; }

    bool replaceGlowPlugs() override {
        if (isRunning()) {
            std::cout << "[" << getName() << "] stop the engine before replacing glow plugs" << std::endl;
            return false;
        }
        std::cout << "[" << getName() << "] glow plugs replaced" << std::endl;
        return true;
    }
};

class JetEngine : public Engine, public IThrustEngine {
public:
    using Engine::Engine;

    std::string getFuelType() const override { return "kerosene"; }
    double getThrust() const override { return isRunning() ? 120.0 : 0.0; }
};

//DIP: высокоуровневый класс

class Workshop {
public:
    // работает с любым двигателем через базовый класс (LSP)
    bool test(Engine& engine) {
        std::cout << "--- Workshop: test " << engine.getName()
                  << " (" << engine.getFuelType() << ") ---" << std::endl;
        if (!engine.start()) {
            return false;
        }
        engine.stop();
        return true;
    }

    // клиенту нужен только тот интерфейс, который он использует (ISP)
    void measure(const IShaftEngine& engine) {
        std::cout << "[Workshop] torque: " << engine.getTorque()
                  << " N*m, rpm: " << engine.getRpm() << std::endl;
    }
    void measure(const IThrustEngine& engine) {
        std::cout << "[Workshop] thrust: " << engine.getThrust() << " kN" << std::endl;
    }

    bool service(ISparkPlugService& engine) { return engine.replaceSparkPlugs(); }
    bool service(IGlowPlugService& engine) { return engine.replaceGlowPlugs(); }
};

// ======================= Тестирование =======================

// ISP: интерфейсы не навязывают лишнего (проверка при компиляции)
static_assert(!std::is_base_of<IShaftEngine, JetEngine>::value, "jet engine has no shaft");
static_assert(!std::is_base_of<IThrustEngine, PetrolEngine>::value, "petrol engine has no thrust");
static_assert(!std::is_base_of<IGlowPlugService, PetrolEngine>::value, "petrol engine has no glow plugs");
static_assert(!std::is_base_of<ISparkPlugService, DieselEngine>::value, "diesel engine has no spark plugs");

int passed = 0, failed = 0;

void check(bool condition, const std::string& name) {
    if (condition) { passed++; std::cout << "  [OK]   " << name << std::endl; }
    else           { failed++; std::cout << "  [FAIL] " << name << std::endl; }
}

void runTests() {
    std::cout << "\n===== Testing =====" << std::endl;

    // LSP: одни и те же проверки для любого наследника Engine
    std::vector<std::unique_ptr<Engine>> engines;
    engines.push_back(std::make_unique<PetrolEngine>("Petrol", 50));
    engines.push_back(std::make_unique<DieselEngine>("Diesel", 50));
    engines.push_back(std::make_unique<JetEngine>("Jet", 500));
    for (auto& e : engines) {
        const std::string n = e->getName();
        check(e->start() && e->isRunning(), n + ": starts");
        check(!e->start(), n + ": cannot start twice");
        e->stop();
        check(!e->isRunning(), n + ": stops");
        check(!e->getFuelType().empty(), n + ": has fuel type");
    }

    // топливо
    PetrolEngine empty("Empty", 0);
    check(!empty.start(), "no fuel: cannot start");
    check(!empty.refuel(-5), "refuel: wrong amount is rejected");
    check(empty.refuel(10) && empty.getFuelLevel() == 10, "refuel: fuel level grows");
    check(empty.start(), "after refuel: starts");

    // ISP: каждый двигатель умеет только своё
    PetrolEngine petrol("Petrol", 50);
    DieselEngine diesel("Diesel", 50);
    JetEngine jet("Jet", 500);
    check(petrol.getTorque() == 0 && jet.getThrust() == 0, "stopped engines give no torque/thrust");
    petrol.start();
    jet.start();
    check(petrol.getTorque() > 0 && petrol.getRpm() > 0, "petrol: torque and rpm when running");
    check(jet.getThrust() > 0, "jet: thrust when running");

    // DIP: Workshop работает через абстракции
    Workshop workshop;
    check(workshop.test(diesel), "workshop tests diesel as Engine");
    check(!workshop.test(petrol), "workshop: running petrol engine cannot be started again");
    check(!workshop.service(petrol), "spark plugs: cannot replace while running");
    petrol.stop();
    check(workshop.service(petrol), "spark plugs: replaced when stopped");
    check(workshop.service(diesel), "glow plugs: replaced when stopped");

    std::cout << "Passed: " << passed << ", failed: " << failed << std::endl;
}

// ======================= Демонстрация =======================

void demo() {
    PetrolEngine petrol("Petrol", 50);
    DieselEngine diesel("Diesel", 80);
    JetEngine jet("Jet", 500);
    Workshop workshop;

    workshop.test(petrol);
    workshop.test(diesel);
    workshop.test(jet);

    petrol.start();
    workshop.measure(petrol);
    workshop.service(petrol);   // нельзя: двигатель работает
    petrol.stop();
    workshop.service(petrol);   // теперь можно
    workshop.service(diesel);

    jet.start();
    workshop.measure(jet);
    jet.stop();
}

int main() {
    demo();
    runTests();
    return failed == 0 ? 0 : 1;
}