#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

vector<int> waiting_queue(8, 0);
int traffic_on_spot(pair<int, int> p)
{
    return (waiting_queue[p.first] + waiting_queue[p.second]);
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
void modify_queue(pair<int, int> p)
{
    waiting_queue[p.second] = max(0, waiting_queue[p.second] - 1);
    waiting_queue[p.first] = max(0, waiting_queue[p.first] - 1);
}
int main(int argc, char *argv[])
{
    vector<pair<int, int>> states = {{0, 2}, {4, 6}, {1, 3}, {5, 7}, {0, 1}, {2, 3}, {4, 5}, {6, 7}, {0, 7}, {1, 4}, {2, 5}, {3, 6}};
    int num;

    int prev_state, next_state, t, i, j, time_step = 0, available;
    prev_state = 0;
    pair<int, int> p, new_p, status;
    //  Server socket is created and bound to a port
    //  Connection is established with a client and server is put in listening mode
    int socket_desc, client_sock, c, read_size;
    struct sockaddr_in server, client;
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8880);

    bind(socket_desc, (struct sockaddr *)&server, sizeof(server));
    listen(socket_desc, 3);
    c = sizeof(struct sockaddr_in);
    client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);

    recv(client_sock, &t, sizeof(int), 0);
    while (true)
    {
        int pos[2];
        if (t > 0)
        {

            t--;
            for (i = 0; i < 8; i++)
            {
                //Server receives information of traffic flow
                //and modifies the queue of waiting cars accordingly.
                recv(client_sock, &num, sizeof(int), 0);
                waiting_queue[i] += num;
            }
        }
        p = states[prev_state];
        new_p = states[prev_state + 1];
        switch (prev_state)
        {
        default:
            if (traffic_on_spot(p) > 0)
            {
                pos[0] = p.first;
                pos[1] = p.second;
                send(client_sock, &pos, 2 * sizeof(int), 0);
                modify_queue(p);
                //Turn lights on and modify waiting queue
            }
            if (traffic_on_spot(p) > 0 && traffic_on_spot(new_p) <= 0)
            {
                next_state = prev_state;
            }
            else
            {
                available = check_availability(states);
                if ((waiting_queue[new_p.first] == 0 || waiting_queue[new_p.second] == 0) && available != 12)
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
                pos[0] = p.first;
                pos[1] = p.second;
                send(client_sock, &pos, 2 * sizeof(int), 0);
                modify_queue(p);
            }
            if (traffic_on_spot(p) > 0 && traffic_on_spot(new_p) <= 0)
            {
                next_state = prev_state;
            }
            else
            {
                available = check_availability(states);
                if ((waiting_queue[new_p.first] == 0 || waiting_queue[new_p.second] == 0) && available != 12)
                {
                    next_state = available;
                }
                else
                    next_state = 0;
            }
            break;
        }
        prev_state = next_state;

        for (j = 0; j < 8; j++)
        {
            if (waiting_queue[j] != 0)
                break;
        }
        if (j == 8 && t <= 0)
        {
            cout<<"Traffic cleared !! \n";
                break;
        }
    }

    close(client_sock);
    close(socket_desc);

    return 0;
}