import time

def timer(func):
    def wrapper(*args, **kwargs):
        start = time.time()
        func(*args, **kwargs)
        end = time.time()
        print(f'Execution time {end - start} seconds')
    return wrapper

def greeting(func):
    def wrapper(*args, **kwargs):
        print('Hello,')
        func(*args, **kwargs)
    return wrapper

@greeting
@timer
def morning(name):
    print(f'Good Morning, {name}!')

morning('Jhon')