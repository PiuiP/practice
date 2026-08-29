import pytest
from decimal import Decimal
from math import pi

from classes import (BankAccount, User, Temperature,
                     Rectangle, Vector2D, Shape, 
                     Circle, Square, Duck)

#################################################
@pytest.fixture
def empty_bank_account():
    new_one = BankAccount('Sheilaaa')
    return new_one

@pytest.fixture
def filled_bank_account():
    new_one = BankAccount('OhMy')
    new_one.deposit(Decimal(50)) 
    return new_one

@pytest.mark.parametrize('amount, expected', [
    (0, 50),
    (50, 100),
    ])
def test_valiv_deposit_method(amount, expected, filled_bank_account):
    assert filled_bank_account.deposit(Decimal(amount)) == Decimal(expected)

def test_invalid_deposit_method(filled_bank_account):
    with pytest.raises(ValueError, match="Amount must be non negative value"):
        filled_bank_account.deposit(Decimal(-50)) 

@pytest.mark.parametrize('amount, expected', [
    (0, 50),
    (50, 0),
    ])
def test_valid_withdraw_method(amount, expected, filled_bank_account):
    assert filled_bank_account.withdraw(Decimal(amount)) == Decimal(expected)

@pytest.mark.parametrize('amount, expected_error', [
    (-20, "negative_value"),
    (600, "insufficient_funds"),
    ])
def test_invalid_withdraw_method(amount, expected_error, filled_bank_account):
    if expected_error == "negative_value":
        with pytest.raises(ValueError, match="Amount must be non negative value"):
                filled_bank_account.withdraw(Decimal(amount))

    elif expected_error == "insufficient_funds":
        with pytest.raises(ValueError, match="There are insufficient funds in the account"):
            filled_bank_account.withdraw(Decimal(amount))

def test_get_balance_method(empty_bank_account, filled_bank_account):
    assert empty_bank_account.get_balance() == Decimal(0)
    assert filled_bank_account.get_balance() == Decimal(50)

#################################################

def test_class_attr():
    fisrt = User('fisrt', 'fisrt_email')
    second = User('second', 'second_email')
    third = User('third', 'third_email')

    assert User.total_users == 3

def test_class_method_from_scv_line():
    new_user = User.from_csv_line("alice,alice@mail.com")

    assert new_user.username == "alice"
    assert new_user.email == "alice@mail.com"

#################################################

def test_temperature_validation():
    with pytest.raises(ValueError):
        Temperature(-300)

@pytest.mark.parametrize('temp, expected', [
    (0.0, 32.0),
    (100.0, 212.0),
    ])
def test_convertation(temp, expected):
    assert Temperature(temp).to_fahrenheit() == expected

#################################################

def test_auto_change_area_diff_width():
    new_rec = Rectangle(5.0, 6.0)
    assert new_rec.area == 30.0

    new_rec.weight = 10.0

    assert new_rec.area == 60.0 


@pytest.mark.parametrize("invalid_width", [-5, 0, -0.1])
def test_rectangle_invalid_weigth_raises_value_error(invalid_width):
    with pytest.raises(ValueError):
        Rectangle(invalid_width, 6)
        
    rect = Rectangle(5, 6)
    with pytest.raises(ValueError):
        rect.weight = invalid_width

#################################################

@pytest.fixture
def first_vector():
    return Vector2D(1, 2)
        
@pytest.fixture
def second_vector():
    return Vector2D(3, 4)

def test_magic_method_for_print(capsys, first_vector, second_vector):
    print(first_vector)
    captured = capsys.readouterr()

    assert captured.out == "(1, 2)\n"

    print(repr(second_vector))
    captured = capsys.readouterr()

    assert captured.out == "Vector2D(x=3; y=4)\n"

def test_equel_magic_method(first_vector, second_vector):
    assert (first_vector == second_vector) == False # x1 != x2 && y1 != y2
    assert (first_vector == first_vector) == True # x1 == x2 && y1 == y2
    assert (first_vector == Vector2D(1, 2)) == True # x1 == x2 && y1 == y2
    assert (first_vector == Vector2D(1, 0)) == False # x1 == x2 && y1 != y2
    assert (second_vector == Vector2D(0, 3)) == False # x1 != x2 && y1 == y2

def test_add_magic_method(first_vector, second_vector):
    assert (first_vector + second_vector) == Vector2D(4, 6) #ordinary adding
    assert (first_vector + Vector2D(0, 0)) == first_vector #no changing +0 +0
    assert (second_vector + Vector2D(-2, -2)) == first_vector #add negative

def test_abs_magic_method(second_vector):
    assert abs(second_vector) == 5.0

#################################################

@pytest.mark.parametrize("shape_instance, expected_area", [
    (Square(4), 16.0), 
    (Square(2.5), 6.25),       
    (Circle(1), pi), 
    (Circle(3), pi * 9), 
])
def test_shapes_area(shape_instance, expected_area):
    # pytest.approx для корректного сравнения float
    assert shape_instance.area() == pytest.approx(expected_area)

def test_shapes_describe():
    circle = Circle(5)
    square = Square(3)
    
    assert circle.describe() == "I am a shape. I am a circle!"
    assert square.describe() == "I am a shape. I am a square!"

def test_cannot_instantiate_abstract_shape():
    with pytest.raises(TypeError):
        Shape()

#################################################

def test_duck_has_parent_methods():
    assert hasattr(Duck, "fly")
    assert hasattr(Duck, "swim")
