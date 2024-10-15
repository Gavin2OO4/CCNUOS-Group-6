#include <iostream>
#include <cstdio>
#include <cstring>
#include <queue>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
using namespace std;

const int N = 10000;

int task;     // �����������
int now_time; // Ŀǰʱ��

struct node
{
    char name[30]; // ��ҵ������
    int Ctime;     // ��ҵ�������뾮��ʱ�䣨�Էּƣ�
    int Rtime;     // ��ҵ������ʱ�䣨�Էּƣ�
    int Stime;     // ��ҵ��ʼ���е�ʱ��
    int Etime;     // ��ҵ���н�����ʱ��
    int Ttime;     // ��ҵ��תʱ��
    float Wtime;   // ��ҵ��Ȩ��תʱ��
} work[N];

int batch[N]; // ģ����

bool cmp(node a, node b)
{
    return a.Ctime < b.Ctime;
};

int Timestamp(string time) // ʱ���ת��
{
    int pos = time.find_last_of(':'); // �ҵ����ð�ţ��Էָ�Сʱ�ͷ���

    string s1 = time.substr(0, pos); // ��ȡСʱ�����Ӳ���
    string s2 = time.substr(pos + 1);

    int n1 = stoi(s1); // ת��Ϊ����
    int n2 = stoi(s2);

    return n1 * 60 + n2; // ���ط��Ӹ�ʽʱ���
}

void p_Timestamp(int time)
{
    cout << setw(2) << setfill('0') << time / 60 << ":"
         << setw(2) << setfill('0') << time % 60 << "\t";
}

void DataDeal();                          // ���ݴ���
void PrintResults(int op, int batchsize); // ��ӡ���

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
        { // �жϸ������Ƿ񵽴����еĿ�ʼʱ��
            q.push(task_id);
            task_id++;
        }

        for (int i = 1; i <= batchsize; i++)
        { // �ж������Ƿ��п��е���������
            if (now_time >= batch[i] && !q.empty())
            { // ����п���������ϵͳ��������в�Ϊ�գ���ȡ���ȼ���ߵģ�����ϵͳ
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
        { // �жϸ������Ƿ񵽴����еĿ�ʼʱ��
            q.push(task_id);
            task_id++;
        }

        for (int i = 1; i <= batchsize; i++)
        { // �ж������Ƿ��п��е���������
            if (now_time >= batch[i] && !q.empty())
            { // ����п���������ϵͳ��������в�Ϊ�գ���ȡ���ȼ���ߵģ�����ϵͳ
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
    }; // �Ƚ���Ӧ����������(��ҵ�ȴ�ʱ�� / ��ҵִ��ʱ��)

    priority_queue<int, vector<int>, decltype(compare)> q(compare);
    int task_id = 1;
    while (task_id <= task || !q.empty())
    {
        now_time++;
        while (task_id <= task && now_time >= work[task_id].Ctime)
        { // �жϸ������Ƿ񵽴����еĿ�ʼʱ��
            q.push(task_id);
            task_id++;
        }

        for (int i = 1; i <= batchsize; i++)
        { // �ж������Ƿ��п��е���������
            if (now_time >= batch[i] && !q.empty())
            { // ����п���������ϵͳ��������в�Ϊ�գ���ȡ���ȼ���ߵģ�����ϵͳ
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
        cout << "    ��ӭ������ҵ�����㷨\n\n";
        cout << "      1.�����ȷ���\n\n";
        cout << "      2.�����ҵ���ȷ�\n\n";
        cout << "      3.�����Ӧ���㷨\n\n";
        cout << "      0.�˳�\n\n";
        cout << "      ���������ѡ�� ";
        cin >> op;
        switch (op)
        {
        case 0:
            exit(0);
        case 1:
            menu("�����ȷ����㷨", FCFS);
            break; // ��ת�������ȷ����㷨
        case 2:
            menu("������ȷ����㷨", SJF);
            break; // ��ת��������ȷ����㷨
        case 3:
            menu("�����Ӧ���㷨", HRN);
            break; // ��ת�������Ӧ���㷨
        default:
            cout << "\n     ѡ����Ч\n\n";
        }
    }
    return 0;
}

void DataDeal()
{
    cout << "      ������Ҫ�������������: ";
    cin >> task; // ��ȡ�û��������������

    string time;
    cout << "      ���������� ����ʱ�� ��ʱ \n"; // ��ʾ�û������������ϸ��Ϣ

    for (int i = 1; i <= task; ++i)
    {
        string s;
        cin >> s;                        // ������������
        strcpy(work[i].name, s.c_str()); // ���������Ƹ��Ƶ��ṹ����

        cin >> time;                     // �����������ʱ��
        work[i].Ctime = Timestamp(time); // ������ʱ��ת��Ϊʱ�����ʽ���洢

        cin >> work[i].Rtime; // ���������ʱ
    }

    // �����������������ʹ�� cmp �������бȽ�
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
        cout << "      1.��������(1)\n\n";
        cout << "      2.�������(>=2)\n\n";
        cout << "      0.�˳�\n\n";
        cout << "      ���������ѡ�� ";
        cin >> op;
        switch (op)
        {
        case 0:
            return;
        case 1:
            DataDeal();
            func(1); // ��������
            PrintResults(name, 1);
            batch[1] = 0;
            break;
        case 2:
            cout << "      �������������� "; // �������(>=2)
            int batchsize;
            cin >> batchsize;

            DataDeal();
            func(batchsize);
            PrintResults(name, batchsize);
            for (int i = 1; i <= batchsize; ++i)
                batch[i] = 0;
            break;
        default:
            cout << "\n     ѡ����Ч\n\n";
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
