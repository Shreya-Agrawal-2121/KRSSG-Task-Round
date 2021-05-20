#include <iostream>
#include <bits/stdc++.h>
using namespace std;
vector<int> waiting_queue(8, 0);
int traffic_on_spot(pair<int, int> p)
{
    return (waiting_queue[p.first] + waiting_queue[p.second]);
}
void print_queue()
{
    for (int i = 0; i < 8; i++)
        cout << waiting_queue[i] << " ";
    cout << "\n";
}
void print_result(pair<int, int> p)
{
    vector<char> lights{'A', 'B', 'C', 'D'};
    vector<string> msg = {"off", "go straight", "go right"};
    int i, status_first, status_second;

    for (i = 0; i < 4; i++)
    {
        cout << lights[i] << " ";
        if (i != p.first / 2 && i != p.second / 2)
        {
            cout << msg[0] << "\n";
        }
        else
        {
            if (2 * i == p.first || 2 * i == p.second)
                cout << msg[1] << " ";

            if (2 * i + 1 == p.first || 2 * i + 1 == p.second)
            {
                cout << msg[2] << " ";
            }
            cout << "\n";
        }
    }
}
pair<int, int> set_status(pair<int, int> p)
{
    pair<int, int> status = {-2, -2};
    if (waiting_queue[p.first] > 0)
        status.first = p.first;
    if (waiting_queue[p.second] > 0)
        status.second = p.second;
    return (status);
}
void modify_queue(pair<int, int> p)
{
    waiting_queue[p.second] = max(0, waiting_queue[p.second] - 1);
    waiting_queue[p.first] = max(0, waiting_queue[p.first] - 1);
}
void generate_output(pair<int, int> p)
{
    pair<int, int> status;

    status = set_status(p);
    print_result(status);
    cout << "Initial queue :";
    print_queue();
    modify_queue(p);
    cout << "Final queue :";
    print_queue();
}
int check_availability(vector<pair<int, int>> states)
{
    int i;
    pair<int, int> available = {-1, -1};

    for (i = 0; i < 12; i++)
    {
        if (waiting_queue[states[i].first] > 0 && waiting_queue[states[i].second] > 0)
        {
            available = states[i];
            break;
        }
    }
    return (i);
}
int main()
{
    vector<int> traffic_flow(8, 0);
    int prev_state, next_state, t, i, j, time_step = 0,available;
    vector<pair<int, int>> states = {{0, 2}, {4, 6}, {1, 3}, {5, 7}, {0, 1}, {2, 3}, {4, 5}, {6, 7}, {0,7}, {1,4}, {2,5}, {3,6}};
    prev_state = 0;
    pair<int, int> p, new_p, status;
    cin >> t;

    while (true)
    {
        if (t > 0)
        {
            for (i = 0; i < 8; i++)
            {
                cin >> traffic_flow[i];
                waiting_queue[i] += traffic_flow[i];
            }
            t--;
        }

        for (j = 0; j < 8; j++)
        {
            if (waiting_queue[j] != 0)
                break;
        }
        if (j == 8 && t <= 0)
            break;
        p = states[prev_state];
        new_p = states[prev_state + 1];
        
        switch (prev_state)
        {
        default:
            if (traffic_on_spot(p) > 0)
            {
                ++time_step;
                cout << "Time step " << time_step << ":\n";
                generate_output(p);
                //Turn lights on and modify waiting queue
            }
            if (traffic_on_spot(p) > 0 && traffic_on_spot(new_p) <= 0)
            {
                next_state = prev_state;
            }
            else
            {
                available = check_availability(states);
                if((waiting_queue[new_p.first] == 0 || waiting_queue[new_p.second] == 0) && available != 12)
                {
                    next_state = available;
                }
                else
                    next_state = prev_state + 1;
            }
            break;

        case 11:
            if (traffic_on_spot(p) > 0)
            {
                ++time_step;
                cout << "Time step " << time_step << ":\n";
                generate_output(p);
                //Turn lights on and modify waiting queue
            }
            if (traffic_on_spot(p) > 0 && traffic_on_spot(new_p) <= 0)
            {
                next_state = prev_state;
            }
            else
            {
                available = check_availability(states);
                if((waiting_queue[new_p.first] == 0 || waiting_queue[new_p.second] == 0) && available != 12)
                {
                    next_state = available;
                }
                else
                    next_state = 0;
            }
            break;
        }
        prev_state = next_state;
    }

    return 0;
}