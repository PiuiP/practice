from os import getenv
import requests
from functools import wraps
import asyncio

def apply_discount(price: float, discount_perecent: float) -> float:
    if (discount_perecent >= 0 and discount_perecent <= 100) and price >= 0:
        return price - (price * discount_perecent / 100)
    else:
        raise ValueError

def validate_password(password: str) -> bool:
    if len(password) < 8:
        return False
    line = list(password)
    numeric = False
    capital_letter = False
    for i in line:
        if numeric and capital_letter:
            return True
        elif i.isupper():
            capital_letter = True
        elif i.isnumeric():
            numeric = True
    return False

def get_element_by_index(lst: list, index: int):
    if type(index) != int:
        raise TypeError
    # [0, 1, ..., len-1]
    # [-len, 1-len, ..., -1]
    if (index >= 0 and len(lst)-1 < index) or (index < 0 and len(lst) < abs(index)):
        raise IndexError

class TaskManager:
    def __init__(self):
        self.tasks = []

    def add_task(self, title: str) -> None:
        self.tasks.append(title)
        return

    def remove_task(self, title: str) -> None:
        if title in self.tasks:
            self.tasks.remove(title)
        return

    def get_all_tasks(self) -> list[str]:
        return self.tasks.copy()

def apply_discounts_to_cart(cart: dict[str, float], discount: float):
    for key, val in cart.items():
        cart[key] = val - (val * discount / 100)
    return cart

def ask_user_for_age() -> int:
    return int(input())
    
def print_user_report(user_name: str, multiple: int) -> None:
    print(user_name * multiple)

def get_api_key() -> str:
    return getenv('API_KEY', 'default_key') #default_key will be returned if API_KEY is missing

def ask_user_to_continue() -> bool:
    answer = input("Continue& (y/n): ")
    if answer == 'y':
        return True
    return False

def get_user_data(user_id: int) -> dict:
    return requests.get(f"https://api.example.com/users/{user_id}") #тип http-request к внешенму апи

def check_and_notify(user_balance: float, email_service: object):
    if user_balance < 0:
        email_service.send_alert("Low balance!")

def retry(max_attempts: int):
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            for i in range(1, max_attempts+1):
                try:
                    result = func(*args, **kwargs)
                    return result
                except Exception as e:
                    if i == max_attempts:
                        raise e
                    continue
        return wrapper
    return decorator

async def fetch_data(url: str):
    await asyncio.sleep(0.1)
    return url

if __name__ == '__main__':
    print(apply_discount(10, 10))
    print(validate_password("Ru12345678"))
    Sunday = TaskManager()
    Sunday.add_task('New Task')
    Sunday.add_task('Next Task')
    print(Sunday.get_all_tasks())
    Sunday.remove_task('New Task')
    print(Sunday.get_all_tasks())
    print(get_api_key())