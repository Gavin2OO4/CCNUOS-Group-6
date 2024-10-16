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
	string name; // 作业的名字
	int Ctime;	   // 作业进入输入井的时间（以分计）
	int Rtime;	   // 作业的运行时间（以分计）
	int Stime;	   // 作业开始运行的时间
	int Etime;	   // 作业运行结束的时间
	int Ttime;	   // 作业周转时间
	float Wtime;   // 作业带权周转时间
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

void FCFS(int batchsize, vector<node>& work)
{
	queue<int> q;
	vector<int> batch(batchsize+1);
	int work_id = 1;
	int work_num = work.size()-1;
	int now_time = 0; // 目前时间
	while (work_id <= work_num || !q.empty())
	{
		now_time++;
		while (work_id <= work_num && now_time >= work[work_id].Ctime)
		{ // 判断该任务是否到达运行的开始时间
			q.push(work_id);
			work_id++;
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
	int now_time = 0; // 目前时间
	while (work_id <= work_num || !q.empty())
	{
		now_time++;
		while (work_id <= work_num && now_time >= work[work_id].Ctime)
		{ // 判断该任务是否到达运行的开始时间
			q.push(work_id);
			work_id++;
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

void HRN(int batchsize, vector<node>& work)
{
	int work_num = work.size()-1;
	vector<int> batch(batchsize+1);
	int now_time = 0; // 目前时间
	auto compare = [&](int x, int y)
	{
		return (now_time - work[x].Ctime) * (work[y].Rtime) < (now_time - work[y].Ctime) * (work[x].Rtime);
	}; // 比较响应比优先运用(作业等待时间 / 作业执行时间)

	priority_queue<int, vector<int>, decltype(compare)> q(compare);
	int work_id = 1;
	while (work_id <= work_num || !q.empty())
	{
		now_time++;
		while (work_id <= work_num && now_time >= work[work_id].Ctime)
		{ // 判断该任务是否到达运行的开始时间
			q.push(work_id);
			work_id++;
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

vector<node> DataDeal()
{
	cout << "请输入要处理的任务数量: ";
	int task_num;
	cin >> task_num; // 读取用户输入的任务数量
	vector<node> work(task_num+1);

	string time;
	cout << "请依次输入名称、进入时间和耗时 \n"; // 提示用户输入任务的详细信息

	for (int i = 1; i <= task_num; ++i)
	{
		cin >> work[i].name;	         // 输入任务名称	 
		cin >> time;					 // 输入任务进入时间
		work[i].Ctime = Timestamp(time); // 将进入时间转换为时间戳格式并存储
		cin >> work[i].Rtime; // 输入任务耗时
	}

	// 对任务数组进行自定义排序，利用lambda函数
	sort(&work[1], &work[task_num+1], [](node a, node b)
		 { return a.Ctime < b.Ctime; });
	return work;
}

void PrintResults(string info, int batchsize, vector<node>work)
{
	int SumTtime = 0;
	float SumWtime = 0;
	int work_num = work.size()-1;
	cout << info << ", 通道数: " << batchsize <<"各指标: "<< endl;
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
	cout<<"平均周转时间: "<<SumTtime*1.0/work_num<<endl;
	cout<<"带权平均周转时间: "<<SumWtime*1.0/work_num<<endl;
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
	cout << "欢迎体验作业调度算法\n";
	cout << "1.先来先服务算法\n";
	cout << "2.最短作业优先法\n";
	cout << "3.最高响应比算法\n";
	cout << "0.退出程序\n\n";
	cout << "请输入你的选择: ";
	int op,batchsize;
	cin >> op;
	cout << "请输入你的通道数量n(n>=1): ";
	cin >> batchsize;
	switch (op)
	{
	case 0:
		exit(0);
	case 1:
		run(FCFS, batchsize, "先来先服务算法");
		break; // 跳转到先来先服务算法
	case 2:
		run(SJF, batchsize, "最短作业优先法");
		break; // 跳转到最短优先服务算法
	case 3:
		run(HRN, batchsize, "最高响应比算法");
		break; // 跳转到最高响应比算法
	default:
		cout << "\n选择无效\n\n";
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
