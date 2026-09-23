from collections import deque

graph = {'A': ['B', 'C'], 'B': ['D'], 'C': [], 'D': []}
visited = set()

def dfs_rec(node, graph, visited):
    if node not in visited:
        visited.add(node)
        print(node, end=" ")

        for neidhor in graph[node]:
            dfs_rec(neidhor, graph, visited)

dfs_rec('A', graph, visited)

def bfs(start, graph):
    current = deque()
    visited = []
    visited.append(start)
    current.append(start)

    while current:
        nowVertex = current.popleft()

        for i in graph[nowVertex]:
            if i not in visited:
                visited.append(i)
                current.append(i)
    return visited

print()
print(bfs('A', graph))

cyclic_graph = {'A': ['B'], 'B': ['C'], 'C': ['A']}

def has_cicle(graph):
    white, gray, black = 0, 1, 2
    color = {node: white for node in graph}

    def dfs(node):
        color[node] = gray
        for neibgor in graph.get(node, []):
            if color[neibgor] == gray:
                return True

