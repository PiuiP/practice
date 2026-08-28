import pytest
import io
import sys
from unittest.mock import patch, MagicMock

from for_test import (apply_discount, validate_password, get_element_by_index, 
                      TaskManager, apply_discounts_to_cart, ask_user_for_age,
                      print_user_report, get_api_key, ask_user_to_continue,
                      get_user_data, check_and_notify, retry, fetch_data
                      )

#############################################################

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

#############################################################

@pytest.mark.parametrize("password, expected", [
    ("Secret123", True), 
    ("Ab1", False), #len < 8
    ("SecretPassword", False), #no numeric
    ("", False) #empty
])
def test_validate_password(password, expected):
    assert validate_password(password) == expected

#############################################################

@pytest.mark.parametrize("line, index", [
    ([], 0), 
    ([1, 2], 2),
    ([1, 2], -3),
])
def test_get_element_index_invalid_indexerror(line, index):
    with pytest.raises(IndexError):
        get_element_by_index(line, index)


@pytest.mark.parametrize("line, index", [
    ([], ''), 
    ([1, 2], '1'),
    ([1, 2], True),
])
def test_get_element_index_invalid_indextype(line, index):
    with pytest.raises(TypeError):
        get_element_by_index(line, index)

#############################################################

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

def test_add_task(empty_manager):
    empty_manager.add_task("New task")
    assert len(empty_manager.get_all_tasks()) == 1
    assert "New task" in empty_manager.get_all_tasks()

def test_add_multiple_task(empty_manager):
    empty_manager.add_task('A')
    empty_manager.add_task(1)
    empty_manager.add_task(True)

    assert len(empty_manager.get_all_tasks()) == 3
    assert empty_manager.get_all_tasks()[2]

def test_remove_existing_tadk(manager_with_tasks):
    manager_with_tasks.remove_task("Купить хлеб")

    assert len(manager_with_tasks.get_all_tasks()) == 1
    assert manager_with_tasks.get_all_tasks()[0] == "Позвонить маме"

def test_remove_nonexisting_task(manager_with_tasks):
    manager_with_tasks.remove_task("None task")

    assert len(manager_with_tasks.get_all_tasks()) == 2

def test_get_all_tasks_without_mutation(manager_with_tasks):
    tasks = manager_with_tasks.get_all_tasks()
    tasks.append("Try to break manager")

    assert len(manager_with_tasks.get_all_tasks()) == 2
    assert len(tasks) == 3 #ordinary list[str]

#############################################################

@pytest.fixture
def prepared_cart():
    """Change it"""
    return {'First':50.0, 'Second':100.50, "Third":30.33}

def test_apply_discounts_to_cart_mutation(prepared_cart):
    apply_discounts_to_cart(prepared_cart, 50.0)
    assert prepared_cart['First'] == 25.0

#############################################################

def test_ask_user_for_age_imitate_valid_input(monkeypatch):
    monkeypatch.setattr(sys, 'stdin', io.StringIO("25\n")) #(obj, name, value, raising=True)

    result = ask_user_for_age()
    assert result == 25

def test_ask_user_for_age_imitate_invalid_input(monkeypatch):
    monkeypatch.setattr("builtins.input", lambda: "not_a_number")

    with pytest.raises(ValueError):
        ask_user_for_age()

#############################################################

@pytest.mark.parametrize('name, multiple, expected', [
    ("Lily", 3, "LilyLilyLily\n"),
    ("", 2, "\n"),
    ("-", 0, "\n"),
    ("  ", 2, "    \n")
    ])
def test_print_user_report(name, multiple, expected, capsys):
    print_user_report(name, multiple)
    captured = capsys.readouterr()

    assert captured.out == expected

#############################################################

@pytest.mark.parametrize('env_var, key, expected', [
    ('API_KEY', 'GoodJob', 'GoodJob'), 
    ('NO_API_KEY', 'No It Is Not Me', 'default_key')
])
def test_get_api_key_fake_env_variablee(env_var, key, expected, monkeypatch):
    monkeypatch.setenv(env_var, key)

    result = get_api_key()
    assert result == expected

#############################################################

@pytest.mark.parametrize('answer, expected', [
    ('y', True),
    ('n', False),
    ('', False),
    ])
def test_ask_user_to_continue(answer, expected, monkeypatch):
    monkeypatch.setattr('builtins.input', lambda _: answer)

    assert ask_user_to_continue() == expected

#############################################################

@patch('for_test.requests.get') #патчим там, где используется/импортирован, а не там, где определен
def test_get_user_data_fake_dependencies(mock_get):
    fake_response = {"id" : 1, "name" : 'Alex'}

    mock_get.return_value.json.return_value = fake_response

    assert get_user_data(1).json() == fake_response

#############################################################

@pytest.mark.parametrize('balance, should_call', [
    (-100, True),  #вызов ДОЛЖЕН быть
    (500, False)   #вызова НЕ должно быть
])
def test_check_and_notify_with_magic_mock(balance, should_call):
    mock_service = MagicMock()
    
    check_and_notify(balance, mock_service)

    #теперь в засиимости от того должен ли был быть вызов (True/False) проверяем был ли реально вызов 
    if should_call:
        mock_service.send_alert.assert_called_once_with("Low balance!")
    else:
        mock_service.send_alert.assert_not_called()

#############################################################

def test_retry_decorator():
    mock_func = MagicMock()
    mock_func.side_effect = [ValueError("First fail"), ValueError("Second fail"), "OK"]

    decorated_func = retry(3)(mock_func)

    result = decorated_func()
    assert result == 'OK'
    assert mock_func.call_count == 3
             
#############################################################

@pytest.mark.asyncio
async def test_fetch_data():
    result = await fetch_data("I will be a Hokage")
    assert result == "I will be a Hokage"
