from decimal import Decimal
from abc import ABC, abstractmethod
from math import pi

class BankAccount:
    def __init__(self, owner: str):
        self.owner = owner
        self.balance = Decimal(0)

    def deposit(self, amount: Decimal) -> Decimal:
        if amount < 0:
            raise ValueError("Amount must be non negative value")
        self.balance += amount

        return self.balance

    def withdraw(self, amount: Decimal) -> Decimal:
        if amount < 0:
            raise ValueError("Amount must be non negative value")
        if amount > self.balance:
            raise ValueError("There are insufficient funds in the account")
        self.balance -= amount

        return self.balance

    def get_balance(self) -> Decimal:
        return self.balance

#################################################

class User:
    total_users = 0 #атрибут этого класса, ощбий для всех экспеляров класса
    def __init__(self, username: str, email: str):
        self.username = username  #атрибут эксепляра, уникальный для каждого экземпляра
        self.email = email
        
        User.total_users += 1

    @classmethod #marked class method, which have access only to class attr
    def get_total_users(cls) -> int: #first arg for class method - cls (like self but for clss attr)
        return cls.total_users

    @classmethod
    def from_csv_line(cls, line: str):
        """создаёт объект из строки типа: alice,alice@mail.com"""
        name, email = line.split(',')
        return cls(name, email) #use cls (not User - class name) for flexible nheritance 

#################################################

class Temperature:
    def __init__(self, degrees: float):
        self.celsius = degrees #при создании объекта всегда будет вызываться сеттер


    @property #getter: to read temperature as t.celsius (without () )
    def celsius(self) -> float:
        return self/self._celsius

    @celsius.setter
    def celsius(self, value: float):
        if value < -273.15:
            raise ValueError("This temperature is lower than absolute zero")
        self._celsius = value

    def to_fahrenheit(self) -> float:
        return self._celsius * (9/5) + 32

    def to_kelvin(self) -> float:
        return self._celsius + 273.15

    
#################################################

class Rectangle:
    def __init__(self, width_val: float, height_val: float):
        self.weight = width_val
        self.height = height_val

    @property
    def weight(self) -> float:
        return self._weight

    @property
    def height(self) -> float:
        return self._height

    @weight.setter
    def weight(self, value: float):
        if value <= 0:
            raise ValueError
        self._weight = value

    @height.setter
    def height(self, value: float):
        if value <= 0:
            raise ValueError
        self._height = value

    @property
    def area(self) -> float:
        return self.height * self.weight

    @property
    def perimetr(self) -> float:
        return 2 * (self.height + self.weight)

    
new_rec = Rectangle(5, 6)
print(new_rec.weight)
print(new_rec.height)
print(new_rec.area)
print(new_rec.perimetr)
#################################################

class Vector2D:
    def __init__(self, x_val: float, y_val: float):
        self.x = x_val
        self.y = y_val

    def __repr__(self):
        return f"{self.__class__.__name__}(x={self.x}; y={self.y})"

    def __str__(self):
        return f"({self.x}, {self.y})"

    def __eq__(self, other):
        return (self.x == other.x) and (self.y == other.y)

    def __add__(self, other):
        return Vector2D(self.x + other.x, self.y + other.y)

    def __sub__(self, other):
        return Vector2D(self.x - other.x, self.y - other.y)

    def __abs__(self):
        return (self.x**2 + self.y**2)**0.5

#################################################

class Shape(ABC):
    @abstractmethod
    def area(self) -> float:
        pass

    def describe(self) -> str:
        return "I am a shape"

class Circle(Shape):
    def __init__(self, radius_val: float):
        self.radius = radius_val

    @property
    def radius(self):
        return self._radius

    @radius.setter
    def radius(self, value: float):
        if value <= 0:
            raise ValueError("Radius must be greater than 0")
        self._radius = value

    #переопределяем родительские методы

    def area(self) -> float:
        return pi * (self.radius ** 2)

    def describe(self):
        return f"{super().describe()}. I am a circle!" #parents super() + itself text

class Square(Shape):
    def __init__(self, side_val: float):
            self.side = side_val

    @property
    def side(self):
        return self._side

    @side.setter
    def side(self, value: float):
        if value <= 0:
            raise ValueError("Side must be greater than 0")
        self._side = value

    def area(self) -> float:
        return self.side ** 2

    def describe(self):
        return f"{super().describe()}. I am a square!"

#################################################

class Flyable:
    def fly() -> str:
        return "Flying"

class Swimmable:
    def swim() -> str:
        return "Swimming"

class Duck(Flyable, Swimmable):
    def quack() -> str:
        return "QUACK QUACK"
