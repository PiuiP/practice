from bisect import bisect_left, bisect_right


def main():
    n, m = map(int, input().split())
    start = []
    end = []
    for i in range(n):
        starts, ends = (map(int, input().split()))
        start.append(starts)
        end.append(ends)
    start.sort()
    end.sort()
    points = list(map(int, input().split()))
    owning = []
    for point in range(m):
        started = bisect_right(start, points[point])
        ended = bisect_left(end, points[point])
        owning.append(started - ended)
    print(*owning)
        

if __name__ == '__main__':
    main()
