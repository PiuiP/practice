import time
from functools import wraps
import logging
import decimal

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
            print(f"The function {func.__name__}: {type(a).__name__}: {str(a)}")
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
        print(result)
        print(f"The function {func.__name__} is finished")
        return result
    return wrapper

@print_call
def add(a: int | float, b: int | float):
    return a + b

add(5, 6)

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
            return func(*args, **kwargs)
        except Exception as a:
            print(f"The function {func.__name__}: {type(a).__name__}: {str(a)}")
    return wrapper

@safe_execute
def divide(a: int, b: int):
    return a / b

print(divide(2, 1))
print(divide(1, 0))

##############################


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
            logging.error(f"{func.__name__} вызвала {type(a).__name__}: {str(a)}")
            return "ХАХХВ ЛОХ"
    return wrapper

@logged
def risky_divide(a, b):
    return a / b

print(risky_divide(10, 2))
print(risky_divide(10, 0))

##############################
def retry(max_attempts: int, delay: int):
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            for i in range(1, max_attempts+1):
                try:
                    result = func(*args, **kwargs)
                    print(result)
                    return result
                except Exception as e:
                    print(f"Попытка {i}/{max_attempts} запуска функции {func.__name__} провалилась: {e}")
                    if i == max_attempts:
                        raise e 
                time.sleep(delay)
            return result
        return wrapper
    return decorator

glob_count = 0

@retry(max_attempts=5, delay=0.08)
def unstale():
    global glob_count
    glob_count += 1
    if glob_count < 3:
        raise ConnectionError("Net is fallen")
    return "Success"

unstale()

@retry(3, 0.5)
def sucker():
    global glob_count
    glob_count += 1
    if glob_count % 2 == 0:
        raise ConnectionError("Net is fallen")
    return "Success"
    
#sucker()

################################
class RateLimitExceeded(Exception):
    pass

def rate_limit(calls_per_minute: int):
    last_call = []
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            last_call[:] = [t for t in last_call if time.time() - t <= 60]

            if len(last_call) >= calls_per_minute:
                raise RateLimitExceeded("Задудосить хочешь?")

            result = func(*args, **kwargs)

            last_call.append(time.time())

            return result
        return wrapper
    return decorator

@rate_limit(5)
def api_call():
    return "data"

api_call()
api_call()
api_call()
api_call()
api_call()
#api_call()

#############################

def validate_types(**types):
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            for keys, values in kwargs.items():
                if type(values) != types[keys]:
                    raise TypeError(f"{keys} must be {types[keys]}, {values} is recieved")
            return func(*args, **kwargs)
        return wrapper
    return decorator
            

@validate_types(name = str, age = int)
def create_user(name, age):
    return f"User {name} is {age} years old"

print(create_user(name = 'Aline', age = 20))
#print(create_user(name = 'Aline', age = '20'))

#####################################################

def cache_with_ttl(ttl_seconds: int | float):
    cache = dict()
    def decorator(func):
        @wraps(func)
        def wrapper(*args, **kwargs):
            cache_key = (args, tuple(sorted(kwargs.items())))

            if cache_key in cache:
                result, timestamp = cache[cache_key]
                if time.time() - timestamp <= ttl_seconds:
                    return result

            result = func(*args, **kwargs)

            cache[cache_key] = (result, time.time())
            return result
        return wrapper
    return decorator

@timer
@cache_with_ttl(ttl_seconds=60)
def expensive_calculation(x):
    print(f"считаю {x}...")
    time.sleep(1)
    return x * 2

expensive_calculation(5)
expensive_calculation(5)
time.sleep(3)
expensive_calculation(5)         