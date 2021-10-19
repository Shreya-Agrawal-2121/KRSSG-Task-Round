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

// Function to print status of traffic lights

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

// Function to decide the status of traffic lights

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

    vector<pair<int, int>> states = {{-1, -1}, {0, 2}, {4, 6}, {1, 3}, {5, 7}, {0, 1}, {2, 3}, {4, 5}, {6, 7}, {0, 7}, {1, 4}, {2, 5}, {3, 6}};

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

    int prev_state, next_state;

    while (true)

    {

        if (t > 0)

        {

            for (i = 0; i < 8; i++)

                cin >> traffic_flow[i];

            for (i = 0; i < 8; i++)

                waiting_queue[i] += traffic_flow[i];

            // At each step the data of traffic flow is sent to server

            for (i = 0; i < 8; i++)

            {

                num = traffic_flow[i];

                send(sock, &num, sizeof(int), 0);
            }
        }

        // Server returns the valid routes to be taken for a particular step as positions in the array

        t--;

        recv(sock, &next_state, sizeof(int), 0);

        ++step;

        if (next_state != 0)
        {

            cout << "Time Step :" << step << "\n";

            generate_output(states[next_state]);
        }

        else

            break;
    }

    close(sock);

    return 0;
}