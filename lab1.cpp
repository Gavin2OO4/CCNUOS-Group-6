#include <iostream>
#include <cstdio>
#include <cstring>
#include <queue>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
using namespace std;

struct node
{
	string name; // ��ҵ������
	int Ctime;	   // ��ҵ�������뾮��ʱ�䣨�Էּƣ�
	int Rtime;	   // ��ҵ������ʱ�䣨�Էּƣ�
	int Stime;	   // ��ҵ��ʼ���е�ʱ��
	int Etime;	   // ��ҵ���н�����ʱ��
	int Ttime;	   // ��ҵ��תʱ��
	float Wtime;   // ��ҵ��Ȩ��תʱ��
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

void FCFS(int batchsize, vector<node>& work)
{
	queue<int> q;
	vector<int> batch(batchsize+1);
	int work_id = 1;
	int work_num = work.size()-1;
	int now_time = 0; // Ŀǰʱ��
	while (work_id <= work_num || !q.empty())
	{
		now_time++;
		while (work_id <= work_num && now_time >= work[work_id].Ctime)
		{ // �жϸ������Ƿ񵽴����еĿ�ʼʱ��
			q.push(work_id);
			work_id++;
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

void SJF(int batchsize, vector<node>& work)
{
	int work_num = work.size()-1;
	vector<int> batch(batchsize+1);
	auto compareshort = [&](int x, int y)
	{
		return work[x].Rtime > work[y].Rtime;
	};
	priority_queue<int, vector<int>, decltype(compareshort)> q(compareshort);
	int work_id = 1;
	int now_time = 0; // Ŀǰʱ��
	while (work_id <= work_num || !q.empty())
	{
		now_time++;
		while (work_id <= work_num && now_time >= work[work_id].Ctime)
		{ // �жϸ������Ƿ񵽴����еĿ�ʼʱ��
			q.push(work_id);
			work_id++;
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

void HRN(int batchsize, vector<node>& work)
{
	int work_num = work.size()-1;
	vector<int> batch(batchsize+1);
	int now_time = 0; // Ŀǰʱ��
	auto compare = [&](int x, int y)
	{
		return (now_time - work[x].Ctime) * (work[y].Rtime) < (now_time - work[y].Ctime) * (work[x].Rtime);
	}; // �Ƚ���Ӧ����������(��ҵ�ȴ�ʱ�� / ��ҵִ��ʱ��)

	priority_queue<int, vector<int>, decltype(compare)> q(compare);
	int work_id = 1;
	while (work_id <= work_num || !q.empty())
	{
		now_time++;
		while (work_id <= work_num && now_time >= work[work_id].Ctime)
		{ // �жϸ������Ƿ񵽴����еĿ�ʼʱ��
			q.push(work_id);
			work_id++;
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

vector<node> DataDeal()
{
	cout << "������Ҫ�������������: ";
	int task_num;
	cin >> task_num; // ��ȡ�û��������������
	vector<node> work(task_num+1);

	string time;
	cout << "�������������ơ�����ʱ��ͺ�ʱ \n"; // ��ʾ�û������������ϸ��Ϣ

	for (int i = 1; i <= task_num; ++i)
	{
		cin >> work[i].name;	         // ������������	 
		cin >> time;					 // �����������ʱ��
		work[i].Ctime = Timestamp(time); // ������ʱ��ת��Ϊʱ�����ʽ���洢
		cin >> work[i].Rtime; // ���������ʱ
	}

	// ��������������Զ�����������lambda����
	sort(&work[1], &work[task_num+1], [](node a, node b)
		 { return a.Ctime < b.Ctime; });
	return work;
}

void PrintResults(string info, int batchsize, vector<node>work)
{
	int SumTtime = 0;
	float SumWtime = 0;
	int work_num = work.size()-1;
	cout << info << ", ͨ����: " << batchsize <<"��ָ��: "<< endl;
	cout << "name\tCtime\tRtime\tStime\tEtime\tTtime\tWtime" << endl;
	for (int i = 1; i <= work_num; i++)
	{
		cout << work[i].name << "\t";
		p_Timestamp(work[i].Ctime);
		cout << work[i].Rtime << "\t";
		p_Timestamp(work[i].Stime);
		p_Timestamp(work[i].Etime);
		cout << work[i].Ttime << "\t";
		cout << fixed << setprecision(3) << work[i].Wtime << endl;
		SumTtime += work[i].Ttime;
		SumWtime += work[i].Wtime;
	}
	cout << endl;
	cout<<"ƽ����תʱ��: "<<SumTtime*1.0/work_num<<endl;
	cout<<"��Ȩƽ����תʱ��: "<<SumWtime*1.0/work_num<<endl;
	cout << endl;

}

void run(void (*func)(int,vector<node>&), int batchsize, string info)
{
	vector<node> work = DataDeal();
	func(batchsize, work);
	PrintResults(info, batchsize,work);
}

void menu()
{
	cout << "********************\n";
	cout << "��ӭ������ҵ�����㷨\n";
	cout << "1.�����ȷ����㷨\n";
	cout << "2.�����ҵ���ȷ�\n";
	cout << "3.�����Ӧ���㷨\n";
	cout << "0.�˳�����\n\n";
	cout << "���������ѡ��: ";
	int op,batchsize;
	cin >> op;
	cout << "���������ͨ������n(n>=1): ";
	cin >> batchsize;
	switch (op)
	{
	case 0:
		exit(0);
	case 1:
		run(FCFS, batchsize, "�����ȷ����㷨");
		break; // ��ת�������ȷ����㷨
	case 2:
		run(SJF, batchsize, "�����ҵ���ȷ�");
		break; // ��ת��������ȷ����㷨
	case 3:
		run(HRN, batchsize, "�����Ӧ���㷨");
		break; // ��ת�������Ӧ���㷨
	default:
		cout << "\nѡ����Ч\n\n";
	}
}

int main()
{
	while (1)
	{
		menu();
	}
	return 0;
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
