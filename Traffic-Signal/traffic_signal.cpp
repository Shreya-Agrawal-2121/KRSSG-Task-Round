#include <iostream>
#include <bits/stdc++.h>
using namespace std;
vector<int> waiting_queue(8, 0);
//FSM  class stores possible states of traffic lights as pair of ints and transitions as per input
//Input is expressed as an integer in the range of 0 to 255 each bit indicating
//presence or absence of traffic at that crossing.
class FSM
{

public:
    vector<pair<int, int>> states;
    int trans[13][256];
    //To ensure optimality for any input a state is formed such that traffic exists on both crossings
    //If it is not so then a state is found such that traffic is present at least in one of the crossings
    //otherwise the state doesn't changes or goes to state 0 (when all lights are off).
    FSM(vector<pair<int, int>> state)
    {
        bool possible2, possible1;

        states = state;
        for (int i = 0; i < 13; i++)
            trans[i][0] = 0;
        for (int i = 0; i < 13; i++)
        {
            for (int j = 1; j < 256; j++)
            {
                possible2 = possible1 = false;
                for (int k = 1; k < 13; k++)
                {
                    if (i == k)
                        continue;
                    else
                    {
                        int p1 = states[k].first;
                        int p2 = states[k].second;
                        if ((j & (1 << p1)) && (j & (1 << p2)))
                        {
                            possible2 = true;
                            trans[i][j] = k;
                            break;
                        }
                    }
                }
                if (possible2 == false)
                {
                    for (int k = 1; k < 13; k++)
                    {
                        if (i == k)
                            continue;
                        else
                        {
                            int p1 = states[k].first;
                            int p2 = states[k].second;
                            if ((j & (1 << p1)) || (j & (1 << p2)))
                            {
                                possible1 = true;
                                trans[i][j] = k;
                                break;
                            }
                        }
                    }
                    if (possible1 == false)
                    {
                        if (i == 0)
                            trans[i][j] = 0;
                        else
                        {
                            int p1 = states[i].first;
                            int p2 = states[i].second;
                            if ((j & (1 << p1)) || (j & (1 << p2)))
                                trans[i][j] = i;
                            else
                                trans[i][j] = 0;
                        }
                    }
                }
            }
        }
    }
};
//Function to convert the traffic input to a number in range of 0 to 255
int get_input()
{
    int sum = 0;
    for (int i = 0; i < 8; i++)
    {
        if (waiting_queue[i])
        {
            sum += ((1 << i));
        }
    }
    return (sum);
}
void print_queue()
{
    for (int i = 0; i < 8; i++)
        cout << waiting_queue[i] << " ";
    cout << "\n";
}
//Function to print status of traffic lights
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

//Function to modify list of waiting cars
void modify_queue(pair<int, int> p)
{
    waiting_queue[p.second] = max(0, waiting_queue[p.second] - 1);
    waiting_queue[p.first] = max(0, waiting_queue[p.first] - 1);
}
//Function to get the actual status of lights at each time step
pair<int,int> set_status(pair<int,int> p){
    pair<int,int>s;
    s.first = -2;
    s.second = -2;
    if(waiting_queue[p.first] > 0){
        s.first = p.first;
    }
    if(waiting_queue[p.second] > 0){
        s.second = p.second;
    }
    return(s);
}
int main()
{
    vector<int> traffic_flow(8, 0);
    int prev_state, next_state, t, i, j;
    //States is a vector of pairs  pair of  valid transitions
    vector<pair<int, int>> states = {{-1, -1}, {0, 2}, {4, 6}, {1, 3}, {5, 7}, {0, 1}, {2, 3}, {4, 5}, {6, 7}, {0, 7}, {1, 4}, {2, 5}, {3, 6}};
    prev_state = 0;
    
    cin >> t;
    FSM F(states);
    int k = 1;
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
        int input = get_input();
        next_state = F.trans[prev_state][input];
        if (next_state != 0)
        {
            cout<<"Time step : "<<k<<"\n";
            pair<int,int> s =set_status(states[next_state]);

            print_result(s);
            cout<<"Initial queue :";
            print_queue();
            modify_queue(states[next_state]);
            cout<<"Final queue :";
            print_queue();
            k++;
        }
        else
            break;

        prev_state = next_state;
    }
    return 0;
}