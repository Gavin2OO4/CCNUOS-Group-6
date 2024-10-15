#include <iostream>
#include <cstdio>
#include <cstring>
#include <queue>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
using namespace std;

const int N = 10000;

int task;     // 任务的总数量
int now_time; // 目前时间

struct node
{
    char name[30]; // 作业的名字
    int Ctime;     // 作业进入输入井的时间（以分计）
    int Rtime;     // 作业的运行时间（以分计）
    int Stime;     // 作业开始运行的时间
    int Etime;     // 作业运行结束的时间
    int Ttime;     // 作业周转时间
    float Wtime;   // 作业带权周转时间
} work[N];

int batch[N]; // 模拟多道

bool cmp(node a, node b)
{
    return a.Ctime < b.Ctime;
};

int Timestamp(string time) // 时间戳转换
{
    int pos = time.find_last_of(':'); // 找到最后冒号，以分隔小时和分钟

    string s1 = time.substr(0, pos); // 提取小时、分钟部分
    string s2 = time.substr(pos + 1);

    int n1 = stoi(s1); // 转换为整数
    int n2 = stoi(s2);

    return n1 * 60 + n2; // 返回分钟格式时间戳
}

void p_Timestamp(int time)
{
    cout << setw(2) << setfill('0') << time / 60 << ":"
         << setw(2) << setfill('0') << time % 60 << "\t";
}

void DataDeal();                          // 数据处理
void PrintResults(int op, int batchsize); // 打印结果

void menu(string name, function<void(int)> func);

function<void(int)> FCFS = [](int batchsize)
{
    queue<int> q;
    int task_id = 1;
    now_time = 0;
    while (task_id <= task || !q.empty())
    {
        now_time++;
        while (task_id <= task && now_time >= work[task_id].Ctime)
        { // 判断该任务是否到达运行的开始时间
            q.push(task_id);
            task_id++;
        }

        for (int i = 1; i <= batchsize; i++)
        { // 判断现在是否有空闲的批处理弹道
            if (now_time >= batch[i] && !q.empty())
            { // 如果有空闲批处理系统且任务队列不为空，则取优先级最高的，加入系统
                int idx = q.front();
                q.pop();
                work[idx].Stime = now_time;
                work[idx].Etime = now_time + work[idx].Rtime;
                work[idx].Ttime = work[idx].Etime - work[idx].Ctime;
                work[idx].Wtime = work[idx].Ttime * 1.0 / work[idx].Rtime;
                batch[i] = work[idx].Etime;
            }
        }
    }
};

function<void(int)> SJF = [](int batchsize)
{
    auto compareshort = [](int x, int y)
    {
        return work[x].Rtime > work[y].Rtime;
    };
    priority_queue<int, vector<int>, decltype(compareshort)> q(compareshort);
    int task_id = 1;
    now_time = 0;
    while (task_id <= task || !q.empty())
    {
        now_time++;
        while (task_id <= task && now_time >= work[task_id].Ctime)
        { // 判断该任务是否到达运行的开始时间
            q.push(task_id);
            task_id++;
        }

        for (int i = 1; i <= batchsize; i++)
        { // 判断现在是否有空闲的批处理弹道
            if (now_time >= batch[i] && !q.empty())
            { // 如果有空闲批处理系统且任务队列不为空，则取优先级最高的，加入系统
                int idx = q.top();
                q.pop();
                work[idx].Stime = now_time;
                work[idx].Etime = now_time + work[idx].Rtime;
                work[idx].Ttime = work[idx].Etime - work[idx].Ctime;
                work[idx].Wtime = work[idx].Ttime * 1.0 / work[idx].Rtime;
                batch[i] = work[idx].Etime;
            }
        }
    }
};

function<void(int)> HRN = [](int batchsize)
{
    now_time = 0;
    auto compare = [](int x, int y)
    {
        return (now_time - work[x].Ctime) * (work[y].Rtime) < (now_time - work[y].Ctime) * (work[x].Rtime);
    }; // 比较响应比优先运用(作业等待时间 / 作业执行时间)

    priority_queue<int, vector<int>, decltype(compare)> q(compare);
    int task_id = 1;
    while (task_id <= task || !q.empty())
    {
        now_time++;
        while (task_id <= task && now_time >= work[task_id].Ctime)
        { // 判断该任务是否到达运行的开始时间
            q.push(task_id);
            task_id++;
        }

        for (int i = 1; i <= batchsize; i++)
        { // 判断现在是否有空闲的批处理弹道
            if (now_time >= batch[i] && !q.empty())
            { // 如果有空闲批处理系统且任务队列不为空，则取优先级最高的，加入系统
                int idx = q.top();
                q.pop();
                work[idx].Stime = now_time;
                work[idx].Etime = now_time + work[idx].Rtime;
                work[idx].Ttime = work[idx].Etime - work[idx].Ctime;
                work[idx].Wtime = work[idx].Ttime * 1.0 / work[idx].Rtime;
                batch[i] = work[idx].Etime;
            }
        }
    }
};

int main()
{
    int op;
    while (1)
    {
        cout << "*****************************\n\n";
        cout << "    欢迎体验作业调度算法\n\n";
        cout << "      1.先来先服务\n\n";
        cout << "      2.最短作业优先法\n\n";
        cout << "      3.最高响应比算法\n\n";
        cout << "      0.退出\n\n";
        cout << "      请输入你的选择： ";
        cin >> op;
        switch (op)
        {
        case 0:
            exit(0);
        case 1:
            menu("先来先服务算法", FCFS);
            break; // 跳转到先来先服务算法
        case 2:
            menu("最短优先服务算法", SJF);
            break; // 跳转到最短优先服务算法
        case 3:
            menu("最高响应比算法", HRN);
            break; // 跳转到最高响应比算法
        default:
            cout << "\n     选择无效\n\n";
        }
    }
    return 0;
}

void DataDeal()
{
    cout << "      请输入要处理的任务数量: ";
    cin >> task; // 读取用户输入的任务数量

    string time;
    cout << "      请输入名称 进入时间 耗时 \n"; // 提示用户输入任务的详细信息

    for (int i = 1; i <= task; ++i)
    {
        string s;
        cin >> s;                        // 输入任务名称
        strcpy(work[i].name, s.c_str()); // 将任务名称复制到结构体中

        cin >> time;                     // 输入任务进入时间
        work[i].Ctime = Timestamp(time); // 将进入时间转换为时间戳格式并存储

        cin >> work[i].Rtime; // 输入任务耗时
    }

    // 对任务数组进行排序，使用 cmp 函数进行比较
    sort(work + 1, work + 1 + task, cmp);
}

void PrintResults(string name, int batchsize)
{
    cout << name << " " << batchsize << endl;
    cout << "name\tCtime\tRtime\tStime\tEtime\tTtime\tWtime" << endl;
    for (int i = 1; i <= task; i++)
    {
        cout << work[i].name << "\t";
        p_Timestamp(work[i].Ctime);
        cout << work[i].Rtime << "\t";
        p_Timestamp(work[i].Stime);
        p_Timestamp(work[i].Etime);
        cout << work[i].Ttime << "\t";
        cout << fixed << setprecision(3) << work[i].Wtime << endl;
    }
    cout << endl
         << endl;
    now_time = 0;
}

void menu(string name, function<void(int)> func)
{
    int op;
    while (1)
    {
        cout << "*****************************\n\n";
        cout << "    " << name << "\n\n";
        cout << "      1.单道运行(1)\n\n";
        cout << "      2.多道运行(>=2)\n\n";
        cout << "      0.退出\n\n";
        cout << "      请输入你的选择： ";
        cin >> op;
        switch (op)
        {
        case 0:
            return;
        case 1:
            DataDeal();
            func(1); // 单道运行
            PrintResults(name, 1);
            batch[1] = 0;
            break;
        case 2:
            cout << "      请输入多道数量： "; // 多道运行(>=2)
            int batchsize;
            cin >> batchsize;

            DataDeal();
            func(batchsize);
            PrintResults(name, batchsize);
            for (int i = 1; i <= batchsize; ++i)
                batch[i] = 0;
            break;
        default:
            cout << "\n     选择无效\n\n";
        }
    }
}

/*
1 8:00 50
2 8:15 30
3 8:30 25
4 8:35 20
5 8:45 15
6 9:00 10
7 9:20 5
*/
