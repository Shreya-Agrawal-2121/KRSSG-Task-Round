import math
import copy
import numpy as np
import sys
import cv2 as cv
import random
BLACK = [0,0,0]
class Node:
    def __init__(self,n):
        self.x = int(n[0])
        self.y = int(n[1])
        self.parent = None

image = cv.imread("/home/shreya/path.jpg",1)
dim = image.shape
window_name = 'Image'
image_copy = image.copy()
start = input("Enter starting point(coordinates seperated by ,)")
start_list = start.split(",")
end = input("Enter goal point(coordinates seperated by ,)")
end_list = end.split(",")
start_x = int(start_list[0])
start_y = int(start_list[1])
node_start = Node((start_x,start_y))
end_x = int(end_list[0])
end_y = int(end_list[1])
node_end = Node((end_x,end_y))
Graph_start = []
Graph_end = []
Graph_start.append(node_start)
Graph_end.append(node_end)
max_iterations = 5000
step_size = 10
reached = 0
iterate = 0
delta = 5
rad = 5
#Function to check if two nodes are same
def is_node_same(node_new_prim, node_new):
        if node_new_prim.x == node_new.x and \
                node_new_prim.y == node_new.y:
            return True

        return False
#Function to generate random node
def generate_random():
    global dim
    x = random.randint(0 + delta,dim[0]-1 - delta)
    y = random.randint(0 + delta,dim[1]-1 - delta)
    n = Node((x,y))

    return(n)
#Function to find distance between 2 nodes
def dist(node_start,node_end):
    dx = node_end.x - node_start.x
    dy = node_end.y - node_start.y
    return math.hypot(dx,dy)
#Function to find cost to a certain node
def cost(node_p):
        node = node_p
        cost = 0.0

        while node.parent:
            cost += math.hypot(node.x - node.parent.x, node.y - node.parent.y)
            node = node.parent

        return cost
#Function to get updated cost
def get_new_cost(node_new,node_start):
    distance = dist(node_new,node_start)
    return(cost(node_start) + distance)
#Function to find nearest node to a particular node in a graph
def nearest_node(n,Graph):
    list_dist = [dist(Graph[i],n) for i in range(len(Graph))]
    indx = int(np.argmin(list_dist))
    return(Graph[indx])
#Function to find most optimal path to goal node
def search_goal_parent(Graph,node_goal):
        global step_size
        dist_list = [math.hypot(n.x - node_goal.x, n.y - node_goal.y) for n in Graph]
        node_index = [i for i in range(len(dist_list)) if dist_list[i] <= step_size]

        if len(node_index) > 0:
            cost_list = [dist_list[i] + cost(Graph[i]) for i in node_index
                         if is_obstacle_free(node_goal,Graph[i]) == 1]
            return node_index[int(np.argmin(cost_list))]

        return len(Graph) - 1

#Function to change data of a node
def change_node(node_new_prim, node_new_prim2):
        node_new = Node((node_new_prim2.x, node_new_prim2.y))
        node_new.parent = node_new_prim

        return node_new
#Function to choose least cost parent
def choose_parent(Graph,node_new, neighbor_index):
        cost = [get_new_cost(node_new,Graph[i]) for i in neighbor_index]

        cost_min_index = neighbor_index[int(np.argmin(cost))]
        node_new.parent = Graph[cost_min_index]
        return(node_new)
#Function to rewire the path
def rewire(Graph, node_new, neighbor_index):
        for i in neighbor_index:
            node_neighbor = Graph[i]

            if cost(node_neighbor) > get_new_cost(node_new, node_neighbor):
                node_neighbor.parent = node_new
        return(Graph)
#Functions to check if path is obstacle free
def is_obstacle_free_one(new_node,node_start):
    if(new_node.x < node_start.x):
        small = new_node
        large = node_start
    else:
        small = node_start
        large = new_node

    if(large.x == small.x):
        return(0)
    slope = ((large.y - small.y)*1.000)/(large.x - small.x)
    for i in range(int(small.x+1),int(large.x)):
        j1 = int(slope*(i - small.x) + small.y)
        j2 = j1 + 1
        if any(image[j1,i]) or any(image[j2,i]):
            return(0)
    return(1)
def is_obstacle_free_two(new_node,node_start):
    if(new_node.y < node_start.y):
        small = new_node
        large = node_start
    else:
        small = node_start
        large = new_node

    if(large.x == small.x):
        return(0)
    slope = ((large.y - small.y)*1.000)/(large.x - small.x)
    for i in range(int(small.y+1),int(large.y)):
        j1 = int((i-small.y)/slope + small.x)
        j2 = j1 + 1
        if any(image[j1,i]) or any(image[j2,i]):
            return(0)
    return(1)

def is_obstacle_free(new_node,node_start):
    if is_obstacle_free_one(new_node,node_start) == 1 and is_obstacle_free_two(new_node,node_start) == 1:
        return(1)
    return(0)
#Function to find a near neighbor to node obtained after stepping
def find_near_neighbor(Graph, node_new):
        n = len(Graph) + 1
        global rad,step_size
        r = min(rad * math.sqrt((math.log(n) / n)), step_size)

        dist_table = [math.hypot(nd.x - node_new.x, nd.y - node_new.y) for nd in Graph]
        dist_table_index = [ind for ind in range(len(dist_table)) if dist_table[ind] <= r and
                            is_obstacle_free(node_new,Graph[ind])]

        return dist_table_index
#Function to generate path
def draw_path(node_goal,node_goal_parent):
    down_node = node_goal
    up_node = node_goal_parent
    down = (down_node.x,down_node.y)
    up = (up_node.x,up_node.y)

    while up_node.parent is not None:
        cv.line(image,up,down,(255,255,255),1)
        down_node = down_node.parent
        up_node = up_node.parent
        down = (down_node.x,down_node.y)
        up = (up_node.x,up_node.y)
#Function to find node at stepsize distance from near_node
def stepping_node(new_node,near_node):
    global step_size
    distance = dist(near_node,new_node)
    dx = new_node.x - near_node.x
    dy = new_node.y - near_node.y
    theta = math.atan2(dy,dx)
    x_new = int(near_node.x + (min(step_size,distance))*math.cos(theta))
    y_new = int(near_node.y + (min(step_size,distance))*math.sin(theta))
    
    step_node = Node((x_new,y_new))
    step_node.parent = near_node
    
    return(step_node)
#Function to extend a graph to the goal node for it
def extend(Graph,node_goal,node_new):
    
        Graph.append(node_new)
        neighbor_index = find_near_neighbor(Graph,node_new)
        if neighbor_index:
            node_new = choose_parent(Graph,node_new,neighbor_index)
            Graph = rewire(Graph,node_new,neighbor_index)
        return((Graph,1,node_new))
#Function to connect two graph by extending one towards a particular node of other              
def Connect(Graph,node_goal):
    
    node_near = nearest_node(node_goal,Graph)
    node_new = stepping_node(node_goal,node_near)
    if is_obstacle_free(node_new,node_near) == 1:
        status = extend(Graph,node_goal,node_new)
        while(True):
            node_new_2 = stepping_node(node_goal,node_new)
            if is_obstacle_free(node_new_2,node_new) == 1:
                status = extend(Graph,node_goal,node_new_2)
                Graph = status[0]
                node_new = change_node(node_new,node_new_2)
            else:
                break
            if is_node_same(node_goal,node_new):
                break
        return((Graph,1,node_new))
    return((Graph,0,node_goal))

        
#Main RRT*_Connect planning        
def rrt_planning():
    
    global Graph_start,Graph_end
    global max_iterations
    global node_end
    
    for i in range(max_iterations):
        node_rand = generate_random()
        node_near = nearest_node(node_rand,Graph_start)
        node_new = stepping_node(node_rand,node_near)
        status = extend(Graph_start,node_rand,node_new)
        Graph_start = status[0]
        if status[1] == 1:
            status_new = Connect(Graph_end,status[2])
            Graph_end = status_new[0]
        if len(Graph_end) < len(Graph_start):
            list_mid = Graph_start
            Graph_start = Graph_end
            Graph_end = list_mid
    ind = search_goal_parent(Graph_start,node_end)
    node_end.parent = Graph_start[ind]
    draw_path(node_end,Graph_start[ind])
            
        
    
def main():
    
    rrt_planning()
    cv.imshow(window_name,image)
    cv.waitKey(0)
    cv.destroyAllWindows()


if __name__ == '__main__':
    main()
