#include <iostream>
#include <bits/stdc++.h>
using namespace std;
/*This fucntion prints the status of all 4 lights for every time step*/
void print_result(pair<int, int> p, vector<int> queue)
{
    vector<char> lights{'A', 'B', 'C', 'D'};
    vector<string> msg = {"off", "go straight", "go right"};
    int i;

    /*In the for loop various conditions are checked 
    to find the status of lights and to ensure proper formatting 
    of printed result for each time step.*/
    for (i = 0; i < 4; i++)
    {
        if (i == p.first / 2 || i == p.second / 2)
        {
            if (i == p.first / 2 && i != p.second / 2)
            {
                if (queue[p.first] <= 0)
                    cout << lights[i] << ": " << msg[0];
                else
                    cout << lights[p.first / 2] << ": " << msg[1 + p.first % 2];
            }
            else if (i == p.second / 2 && i != p.first / 2)
            {
                if (queue[p.second] <= 0)
                    cout << lights[i] << ": " << msg[0];
                else
                    cout << lights[i] << ": " << msg[1 + p.second % 2];
            }
            else if (p.first / 2 == p.second / 2)
            {
                if (queue[p.first] > 0)
                    cout << lights[i] << ": " << msg[1] << " ";

                if (queue[p.first] <= 0 && queue[p.second] > 0)
                    cout << lights[i] << ": " << msg[2] << " ";
                else if (queue[p.first] > 0 && queue[p.second] > 0)
                    cout << msg[2] << " ";
            }
        }
        else
            cout << lights[i] << ": " << msg[0];

        cout << "\n";
    }
}
int main()
{
    vector<pair<int, int>> states = {{0, 1}, {0, 2}, {0, 7}, {1, 3}, {1, 4}, {2, 3}, {2, 5}, {3, 6}, {4, 5}, {4, 6}, {5, 7}, {6, 7}};
    /*states is a vector of pairs containing all possible combinations of valid traffic lights.
    The number indicates lights at which position should be turned on while all other are off.*/
    int i, j, t, step = 0, k, cnt;
    /*initial_queue contains the traffic data present at each time step after traffic flow.*/
    /*traffic_flow contains data about incoming traffic at each step*/
    vector<int> initial_queue(8, 0);
    vector<int> traffic_flow(8, 0);
    cout << "Enter time steps till which traffic will flow in\n";
    cin >> t;
    for (i = 0; i < 8; i++)
        cin >> traffic_flow[i];

    for (i = 0; i < 8; i++)
        initial_queue[i] += traffic_flow[i];

    t--;
    while (true)
    {
        /*The infinite loop breaks when all enteries in
        the initial_queue are zero no other traffic will flow in*/
        for (j = 0; j < 8; j++)
        {
            if (initial_queue[j] != 0)
                break;
        }
        if (j == 8 && t <= 0)
            break;
        /*traffic_on_spot is a vector of pairs that contains the traffic 
        present on the valid possible route(valid traffic lights combination)
        and to ensure that traffic flow is done through 2 possible routes
        at each step a minimum of the summands is also added to ensure
        no favour to no traffic roads(where traffic at any road is 0, that 
        road  is given less preference)*/
        vector<pair<int, int>> traffic_on_spot;
        traffic_on_spot.assign(12, {0, 0});

        for (i = 0; i < 12; i++)
        {
            traffic_on_spot[i].first += initial_queue[states[i].first] + initial_queue[states[i].second];
            traffic_on_spot[i].second += min(initial_queue[states[i].first], initial_queue[states[i].second]);
        }
        /*The roads with maximum traffic is selected and traffic at that roads are cleared first*/
        vector<pair<int, int>>::iterator it = max_element(traffic_on_spot.begin(), traffic_on_spot.end());

        pair<int, int> p;
        p.first = states[it - traffic_on_spot.begin()].first;
        p.second = states[it - traffic_on_spot.begin()].second;

        ++step;

        cout << "Time step " << step << ": \n";
        /*The status of traffic lights are printed using the print_result function*/
        print_result(p, initial_queue);
        cout << "Initial queue : ";
        for (cnt = 0; cnt < 8; cnt++)
            cout << initial_queue[cnt] << " ";
        cout << "\n";
        /*After clearing traffic the initial queue is modified accordingly*/
        initial_queue[p.second] = max(0, initial_queue[p.second] - 1);
        initial_queue[p.first] = max(0, initial_queue[p.first] - 1);

        cout << "Final queue : ";
        for (cnt = 0; cnt < 8; cnt++)
            cout << initial_queue[cnt] << " ";
        cout << "\n";

        if (t > 0)
        {
            for (i = 0; i < 8; i++)
                cin >> traffic_flow[i];

            for (i = 0; i < 8; i++)
                initial_queue[i] += traffic_flow[i];

            t--;
        }
    }

    return 0;
}
/*
Valid combinations of traffic lights
1. A - straight,right{0,1}
2. A - straight, B- straight{0,2}
3. A - straight D - right{0,7}
4. A - right B -right{1,3}
5. A -right C - straight{1,4}
6. B - straight,right{2,3}
7. B - straight C - right{2,5}
8. B - right D - straight{3,6}
9. C - straight C - right{4,5}
10. C - straight D - straight{4,6}
11. C - right D - right{5,7}
12. D - straight,right{6,7}
*/