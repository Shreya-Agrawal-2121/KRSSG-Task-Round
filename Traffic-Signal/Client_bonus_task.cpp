#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

vector<int> waiting_queue(8, 0);
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

int main(int argc, char *argv[])
{
    int sock;
    struct sockaddr_in server;
    pair<int, int> p;

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
            waiting_queue[i] += traffic_flow[i];
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
            if (waiting_queue[j] != 0)
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
        generate_output(p);

        if (t > 0)
        {
            for (i = 0; i < 8; i++)
                cin >> traffic_flow[i];

            for (i = 0; i < 8; i++)
                waiting_queue[i] += traffic_flow[i];

            for (i = 0; i < 8; i++)
            {
                num = traffic_flow[i];
                send(sock, &num, sizeof(int), 0);
            }

            t--;
        }
    }
    close(sock);
    return 0;
}