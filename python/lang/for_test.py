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


if __name__ == '__main__':
    print(apply_discount(10, 10))
    print(validate_password("Ru12345678"))