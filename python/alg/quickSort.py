def quickSort(numbers):
    if len(numbers) <= 1:
        return numbers
    
    left = []
    right = []
    pivotInd = len(numbers) // 2
    pivot = numbers[pivotInd]

    for i in range(len(numbers)):
        if i == pivotInd:
            continue
        elif numbers[i] < pivot:
            left.append(numbers[i])
        else:
            right.append(numbers[i])
    
    return quickSort(left) + [pivot] + quickSort(right)

def partition(numbers, left, right):
    pivotInd = (right+left)//2
    pivot = numbers[pivotInd]
    
    while (right > left):
        while(numbers[left] < pivot):
            left+=1
        while(right >= left and numbers[right] > pivot):
            right-=1
        if left < right:
            numbers[left], numbers[right] = numbers[right], numbers[left]
            left += 1
            right -= 1
    return right

def quickSortHoar(numbers, left, right):
    if left >= right:
        return 
    pivotind = partition(numbers, left, right)
    quickSortHoar(numbers, left, pivotind)
    quickSortHoar(numbers, pivotind+1, right)

line = [-1, -100, 0, -1, 50]
quickSortHoar(line, 0, len(line)-1)
print(line)