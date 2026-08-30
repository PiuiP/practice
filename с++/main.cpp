#include <iostream>
#include <string>

//stuct
struct Player {
    std::string name;
    int health;

    // constructor default without params
    Player() : name("no_name"), health(0) {} //списочная инициалиизация, чтобы не создавать пустую переменную и ттолько потом класть в нее значение – делаем сразу
    // constructor with params
    Player(std::string nameVal, int healthVal) : name(nameVal), health(healthVal) {} //the same but with params
};

//pointers vs reference vs value
int healthByValue(Player p){
    p.health += 10;
    return p.health;
}

int healthByPointer(Player* p){
    p->health += 10;
    return p->health;
}

int healthByPointerDepoint(Player* p){
    (*p).health += 10;  //try why not
    return (*p).health;
}

int healthByReference(Player& p){
    p.health += 10;
    return p.health;
}

//class
class Animal{
    public:

    static int total_animals; //атрибут КЛАССА, как в питоне, но через static + вне класса надо выделить память пот статический атрибут

    std::string name; //атрибут ЭКЗЕМПЛЯРА

    Animal(std::string nameVal) : name(nameVal) {
        std::cout << "Animal create..." << std::endl;
        total_animals++;
    }
    
    virtual void speak(){
        std::cout << name << " makes a noise" << std::endl;
    }

    static int getTotalAnimals(){ // class method (like @classmethod in python но через static и можно вызвать без создания объектов
        // return name; // ОШИБКА! статический метод не знает, чьё имя брать
        return total_animals; //works only with static
    }

    virtual ~Animal(){ //у базового класса всегда виртуальный дестроктор, иначе вызывается только дестркутор родителя, а деструктор наследника игнорируется -> уттечка памяти
        std::cout << "Animal is destroyed..." << std::endl;
        total_animals--;
    }
};

int Animal::total_animals = 0; //NECCECERY!!!

class Dog : virtual public Animal { //create: constructor Animal -> constructor Dog; kill: destructor Dog -> destructor Animal;
    public:
    Dog(std::string nameVal) : Animal(nameVal){
        std::cout << "Dog create..." << std::endl;
    }

    void speak() override{ //override show that we rewrite parent's method. Mistake in the name -> error, not create a new method
        std::cout << name << " says: Woof-Woof!" << std::endl;
    }

    ~Dog() {
        std::cout << "Dog is destroyed..." << std::endl;
    }
};

int main() {
    Player play{"Oleg", 50}; 

    std::cout << "Original Oleg health: " << play.health << std::endl;
    
    std::cout << "By Value: " << healthByValue(play) << " Orig change? - " << play.health << std::endl;
    
    //для указателя передаем адрес объекта
    std::cout << "By Pointer: " << healthByPointer(&play) << " Orig change? - " << play.health << std::endl;
    std::cout << "By Pointer Depoint: " << healthByPointerDepoint(&play) << " Orig change? - " << play.health << std::endl;
    
    //для ссылки передаем объект как обычно, C++ сам сделает ссылку под капотом
    std::cout << "By Reference: " << healthByReference(play) << " Orig change? - " << play.health << std::endl;
    
    std::cout << "protect data -> const ref | change orig -> ref or pointer" << std::endl;

    /////////////////////////////////////////

    //new/delete dynamic array
    int* dynamicArray = new int[5];

    for (int i = 1; i <= 5; ++i){
        dynamicArray[i] = i;
        std::cout << i << ' ';
        if (i == 5){
        std::cout << std::endl;
        }
    }

    delete[] dynamicArray;

    ////////////////////////////////////////
    std::cout << "Animals count at start: " << Animal::getTotalAnimals() << std::endl;

    Animal ordinary_cat("Barsik");

    std::cout << ordinary_cat.name << std::endl; //access to ordinary atrr
    ordinary_cat.speak();


    Animal* a = new Dog("Sharik");
    std::cout << a->name << std::endl;
    a->speak(); //указатель типа Animal*, но вызовется метод класса Dog ебать приколы
    a->Animal::speak(); //хуяк и родительский метод
    std::cout << "Animals count now: " << Animal::getTotalAnimals() << std::endl;

    delete a;

    std::cout << "Animals count now: " << Animal::getTotalAnimals() << std::endl;

}