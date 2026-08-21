import time
from functools import wraps


def timer(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        start = time.time()
        try:
            result = func(*args, **kwargs)
            end = time.time()
            print(f'Execution time of {func.__name__} is {end - start} seconds')
            return result
        except Exception as a:
            end = time.time()
            print(f"The function {func.__name__}: {type(a).__name__}: str(a)")
            print(f'Execution time of {func.__name__} is {end - start} seconds')     
    return wrapper

@timer
def morning(name):
    print(f'Good Morning, {name}!')

@timer
def slow_func():
    time.sleep(1)
    raise ValueError("OOOOOOps!")

morning('Jhon')
slow_func()

#################################


def print_call(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        print(f"Calling the function {func.__name__}")
        result = func(*args, **kwargs)
        print(f"The function {func.__name__} is finished")
        return result
    return wrapper

@print_call
def add(a: int | float, b: int | float):
    return a + b

print(add(5, 6))

################################

def uppercase_result(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        result = func(*args, **kwargs)
        if isinstance(result, str):
            result = result.upper()
        return result
    return wrapper

@uppercase_result
def greet(name: str):
    return f"Gamardjoba, {name}!"

@uppercase_result
def multiple(a: int | float, b: int | float):
    return a * b

print(greet("bruder"))
print(multiple(10, 10.0))

#################################

def call_counter(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        wrapper.call_count += 1
        return func(*args, **kwargs)

    wrapper.call_count = 0 #иницлизаяция атрбута ОДИН раз при декорровании ток

    return wrapper

@call_counter
def function_for_count():
    return

function_for_count()
function_for_count()
function_for_count()

print(function_for_count.call_count)

##################################

def safe_execute(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        try:
            func(*args, **kwargs)
        except Exception as a:
            print(f"The function {func.__name__}: {type(a).__name__}: str(a)")
    return wrapper

@safe_execute
def divide(a: int, b: int):
    return a / b

print(divide(2, 1))
print(divide(1, 0))

##############################

import logging

logging.basicConfig(
    filename='app.log',
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

def logged(func):
    @wraps(func)
    def wrapper(*args, **kwargs):
        logging.info(f"Вызов {func.__name__} с args={args}, kwargs={kwargs}")
        try:
            result = func(*args, **kwargs)
            logging.info(f"{func.__name__} вернула {result}")
            return result
        except Exception as a:
            logging.error(f"{func.__name__} вызвала {type(a).__name__}: str(a)")
            return "ХАХХВ ЛОХ"
    return wrapper

@logged
def risky_divide(a, b):
    return a / b

print(risky_divide(10, 2))
print(risky_divide(10, 0))
