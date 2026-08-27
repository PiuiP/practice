import pytest

from for_test import apply_discount, validate_password, TaskManager

@pytest.mark.parametrize("price, percent, expected", [
    (1000, 10, 900.0), 
    (100, 0, 100.0), #second is minimum
    (100, 100, 0.0), #second is maximum
    (0, 10, 0.0), #price is free
    (50.5, 20, 40.4), #float values
])
def test_apply_discount_valid(price, percent, expected):
    assert apply_discount(price, percent) == expected

@pytest.mark.parametrize("price, percent", [
    (100, -5), #second is negative
    (100, 105), #second is more than 100% 
    (-10, 10), #first is negative
])
def test_apply_discount_invalid(price, percent):
    with pytest.raises(ValueError):
        apply_discount(price, percent)

@pytest.mark.parametrize("password, expected", [
    ("Secret123", True), 
    ("Ab1", False), #len < 8
    ("SecretPassword", False), #no numeric
    ("", False) #empty
])
def test_validate_password(password, expected):
    assert validate_password(password) == expected

#####################################################

@pytest.fixture
def empty_manager():
    """Пустой менеджер"""
    return TaskManager()

@pytest.fixture
def manager_with_tasks():
    """Менеджер с двумя предзагруженными задачами"""
    manager = TaskManager()
    manager.add_task("Купить хлеб")
    manager.add_task("Позвонить маме")
    return manager

#TODO: tests for all class methods