from collections import deque

def bfs(graph, startpoint):
    visited = []
    visited.append(startpoint)
    current = deque()
    current.append(startpoint)

    while (len(current) > 0):
        nowVertex = current.popleft()

        for i in graph[nowVertex]:
            if i not in visited:
                visited.append(i)
                current.append(i)

    return visited

def dfs(graf, startpoint):
    visited = []
    visited.append(startpoint)
    current = deque()
    current.append(startpoint)

    while(len(current)>0):
        nowVertex = current.pop()

        for i in graf[nowVertex]:
            if i not in visited:
                visited.append(i)
                current.append(i)

    return visited

def dfsRec(graf, node, visited=None):
    if visited is None:
        visited = []
    visited.append(node)
    for neighbor in graf[node]:
        if neighbor not in visited:
            dfsRec(graf, neighbor, visited)
    return list(visited)

def Djikstra(graph, startnode):
    distances = dict()
    visited = set()
    sequenceVisit = []

    for key, value in graph.items():
        distances[key] = float('inf')
    distances[startnode] = 0

    while (len(visited) < len(distances)):
        closestVertex = None
        SmallestDist = float('inf')

        for vertex, distance in graph.items():
            if(vertex not in visited and distances[vertex] < SmallestDist):
                SmallestDist = distances[vertex]
                closestVertex = vertex

        if closestVertex == None: break

        visited.add(closestVertex)
        sequenceVisit.append(closestVertex)

        for i in graph[closestVertex]:
            weight = graph[closestVertex][i]
            newWeight = distances[closestVertex] + weight
            if newWeight < distances[i]:
                distances[i] = newWeight

    return distances, sequenceVisit

        


def main():
    graph = {
    'A': ['B', 'C'],
    'B': ['A', 'D', 'E'],
    'C': ['A', 'F'],
    'D': ['B'],
    'E': ['B', 'F'],
    'F': ['C', 'E']
    }
    print(bfs(graph, 'A'))
    print(dfs(graph, 'A'))
    print(dfsRec(graph, 'A'))

    graph1 = {
    'A': {'B': 7, 'C': 9, 'F': 14},
    'B': {'A': 7, 'C': 10, 'D': 15},
    'C': {'A': 9, 'B': 10, 'D': 11, 'F': 2},
    'D': {'B': 15, 'C': 11, 'E': 6},
    'E': {'D': 6, 'F': 9},
    'F': {'A': 14, 'C': 2, 'E': 9}
    }
    start_node = 'A'
    print(Djikstra(graph1, start_node))
    
if __name__ == '__main__' :
    main()