class Node:
    def __init__(self, value):
        self.value = value
        self.left = None
        self.right = None

def insert(root, value):
    if root is None:
        return Node(value)
    if value > root.value:
        root.right = insert(root.right, value)
    elif value < root.value:
        root.left = insert(root.left, value)
    return root

def heigth(root):
    if root is None:
        return 0
    return 1 + max(heigth(root.left), heigth(root.right))

def main():
    numbers = list(map(int, input().split()))
    root = None
    for i in numbers:
        if i != 0:
            root = insert(root, i)
    print(heigth(root))

if __name__ == '__main__':
    main()