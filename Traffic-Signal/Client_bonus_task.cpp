#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

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
int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server;
    pair<int, int> p;
    vector<int> initial_queue(8, 0);
    vector<int> traffic_flow(8, 0);
    int t, i, j, k, step = 0, cnt;
    cout << "Enter number of time steps till which traffic will flow in:\n";
    cin >> t;
    int num;
    // Client is connected to the server
    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8880);
    connect(sock, (struct sockaddr *)&server, sizeof(server));
    send(sock, &t, sizeof(int), 0);
    if (t > 0)
    {
        for (i = 0; i < 8; i++)
            cin >> traffic_flow[i];

        for (i = 0; i < 8; i++)
            initial_queue[i] += traffic_flow[i];
        //At each step the data of traffic flow is sent to server
        for (i = 0; i < 8; i++)
        {
            num = traffic_flow[i];
            send(sock, &num, sizeof(int), 0);
        }

        t--;
    }

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
        //Server returns the valid routes to be taken for a particular step as positions in the array
        int pos[2];
        recv(sock, &pos, 2 * sizeof(int), 0);
        p.first = pos[0];
        p.second = pos[1];

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

            for (i = 0; i < 8; i++)
            {
                num = traffic_flow[i];
                send(sock, &num, sizeof(int), 0);
            }

            t--;
        }
    }
}