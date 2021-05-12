#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

int main(int argc, char *argv[])
{
    vector<pair<int, int>> states = {{0, 1}, {0, 2}, {0, 7}, {1, 3}, {1, 4}, {2, 3}, {2, 5}, {3, 6}, {4, 5}, {4, 6}, {5, 7}, {6, 7}};
    /*states is a vector of pairs containing all possible combinations of valid traffic lights.
    The number indicates lights at which position should be turned on while all other are off.*/
    vector<int> initial_queue(8, 0);
    int num;

    int i, j, t, step = 0, k, cnt;
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
    //Client sends to server the information of timesteps till which traffic will flow in
    recv(client_sock, &t, sizeof(int), 0);
    while (true)
    {
        if (t > 0)
        {

            t--;
            for (i = 0; i < 8; i++)
            {
                //Server receives information of traffic flow
                //and modifies the queue of waiting cars accordingly.
                recv(client_sock, &num, sizeof(int), 0);
                initial_queue[i] += num;
            }
        }
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

        initial_queue[p.second] = max(0, initial_queue[p.second] - 1);
        initial_queue[p.first] = max(0, initial_queue[p.first] - 1);

        int pos[2];
        pos[0] = p.first;
        pos[1] = p.second;
        /*The information of traffic lights is sent to client 
        where it calculates the traffic light as well as displays the
        status of lights.*/
        send(client_sock, &pos, 2 * sizeof(int), 0);

        for (j = 0; j < 8; j++)
        {
            if (initial_queue[j] != 0)
                break;
        }
        if (j == 8 && t <= 0)
            break;
    }

    return 0;
}
