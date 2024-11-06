#include <bits/stdc++.h>
#include <stdlib.h>
using namespace std;

// 内存
struct mem_range
{ // 左闭右开
    int start;
    int end;
};

struct mem_manager
{
    int start;
    int end;
    vector<mem_range> recycled;
};
mem_manager MEM_MANAGER;

void init_mem()
{
    MEM_MANAGER.start = 0;
    MEM_MANAGER.end = 4096;
    MEM_MANAGER.recycled.clear();
}

// 在res里分配大小至少是range的地址块。成功返回0，失败返回1；
bool alloc_mem(int range, mem_range &res)
{
    res.start = -1;
    for (auto iter = MEM_MANAGER.recycled.begin(); iter != MEM_MANAGER.recycled.end(); ++iter)
    {
        auto [l, r] = *iter;
        if (r - l >= range)
        {
            res.start = l;
            res.end = r;
            MEM_MANAGER.recycled.erase(iter);
            break;
        }
    }
    if (res.start != -1)
        return 0;
    if (MEM_MANAGER.end - MEM_MANAGER.start >= range)
    {
        res.start = MEM_MANAGER.start;
        res.end = MEM_MANAGER.start + range;
        MEM_MANAGER.start += range;
    }
    if (res.start == -1)
        return 1;
    return 0;
}

bool dealloc_mem(mem_range &res)
{
    int l = res.start, r = res.end;
    //  cout << "word" << l << " " << r << endl;
    if ((l >= MEM_MANAGER.start && l < MEM_MANAGER.end) || (r > MEM_MANAGER.start && r <= MEM_MANAGER.end))
    {
        // cout << "r1\n";
        return 1; // 出现bug:内存已经空闲了
    }
    for (auto [start, end] : MEM_MANAGER.recycled)
    {
        if ((l >= start && l < end) || (r > start && r <= end))
        {
            // cout << "r2\n";
            return 1; // 出现bug:内存已经空闲了
        }
    }

    if (l == MEM_MANAGER.end)
    {
        MEM_MANAGER.end = r;
        res.start = res.end = -1;
        return 0;
    }

    MEM_MANAGER.recycled.emplace_back(res);

    res.start = res.end = -1; // 表示无效
    return 0;
}

void merge_mem()
{
    if (MEM_MANAGER.recycled.size() == 0)
        return;

    sort(MEM_MANAGER.recycled.begin(), MEM_MANAGER.recycled.end(), [](mem_range a, mem_range b)
         { return a.start < b.start; });

    int len = MEM_MANAGER.recycled.size();

    for (auto iter = MEM_MANAGER.recycled.begin(); iter != MEM_MANAGER.recycled.end() && next(iter) != MEM_MANAGER.recycled.end();)
    {

        if ((*iter).end == (*next(iter)).start)
        {
            (*iter).end = (*next(iter)).end;
            iter = MEM_MANAGER.recycled.erase(next(iter));
        }
        else
        {
            iter = next(iter);
        }
    }
    // cerr << "111111111\n";

    // sort(MEM_MANAGER.recycled.begin(), MEM_MANAGER.recycled.end(), [](mem_range a, mem_range b)
    //      { return a.start < b.start; });

    for (auto iter = MEM_MANAGER.recycled.begin(); iter != MEM_MANAGER.recycled.end();)
    {
        auto [l, r] = *iter;
        if (MEM_MANAGER.end == l)
        {
            MEM_MANAGER.end = r;
            iter = MEM_MANAGER.recycled.erase(iter);
        }
        else
        {
            iter = next(iter);
        }
    }

    // cout << "22222222\n";

    reverse(MEM_MANAGER.recycled.begin(), MEM_MANAGER.recycled.end());

    for (auto iter = MEM_MANAGER.recycled.begin(); iter != MEM_MANAGER.recycled.end();)
    {
        auto [l, r] = *iter;
        if (MEM_MANAGER.start == r)
        {
            MEM_MANAGER.start = l;
            iter = MEM_MANAGER.recycled.erase(iter);
        }
        else
        {
            iter = next(iter);
        }
    }
    reverse(MEM_MANAGER.recycled.begin(), MEM_MANAGER.recycled.end());
}

void show_mem()
{
    cout << MEM_MANAGER.start << " " << MEM_MANAGER.end << endl;
    cout << "---\n";
    for (auto [l, r] : MEM_MANAGER.recycled)
    {
        cout << l << " " << r << endl;
    }
}

// 定义 PCB 结构体
typedef struct PCB
{
    int pid;
    int status; // 0: 空闲, 1: 就绪, 2: 运行中, 3: 等待中
    mem_range mem;
    struct PCB *next;
} PCB;

PCB *newPCB()
{
    PCB *pcb = new PCB;
    pcb->status = 0;
    pcb->pid = -1;
    pcb->mem.start = pcb->mem.end = -1;
    return pcb;
}

void cancelPCB(PCB *pcb)
{
    pcb->status = 0;
    pcb->pid = -1;
    dealloc_mem(pcb->mem); // 归还内存
    merge_mem();           // 合并内存块
}

int activatePCB(PCB *pcb, int pid, int mem_range)
{
    pcb->pid = pid;
    pcb->status = 1; // 就绪态
    if (alloc_mem(mem_range, pcb->mem))
    {
        return 1;
    }

    return 0;
}

// 全局队列
PCB *runningQueue = NULL;
PCB *readyQueue = NULL;
PCB *waitingQueue = NULL;
PCB *pcbPool = NULL;

// 函数声明
void initializeSystem();
void createProcess();
void destroyProcess();
void timeSliceExpired();
void suspendProcess();
void activateProcess();
void displayQueues();
void enqueue(PCB **queue, PCB *process);
PCB *dequeue(PCB **queue);
void mergePCBs();

int main()
{
    // init_mem();
    // show_mem();
    // cout << "ok" << endl;
    // mem_range a, b, c, d, e;
    // alloc_mem(10, a);
    // show_mem();
    // alloc_mem(100, b);
    // alloc_mem(100, c);
    // cout << dealloc_mem(b) << endl;
    // cout << dealloc_mem(a) << endl;

    // cout << dealloc_mem(c) << endl;

    // merge_mem();
    // cout << "ok" << endl;
    // show_mem();
    int choice;
    initializeSystem();

    do
    {
        printf("\n1. Create Process\n");
        printf("2. Destroy Process\n");
        printf("3. Time Slice Expired\n");
        printf("4. Suspend Process\n");
        printf("5. Activate Process\n");
        printf("6. Display Queues\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            createProcess();
            break;
        case 2:
            destroyProcess();
            break;
        case 3:
            timeSliceExpired();
            break;
        case 4:
            suspendProcess();
            break;
        case 5:
            activateProcess();
            break;
        case 6:
            displayQueues();
            break;
        case 0:
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}

// 初始化系统
void initializeSystem()
{
    // 初始化内存空间
    init_mem();

    // 初始化 PCB 池
    for (int i = 0; i < 10; i++)
    {
        PCB *newPcb = newPCB();
        newPcb->next = pcbPool;
        pcbPool = newPcb;
    }
}

int cnt = 0;
// 创建进程
//请注意这里，我认为pid和进程需要的内存mem_range都是需要根据进程信息生成的
//但没有完全实现
void createProcess()
{
    if (pcbPool == NULL)
    {
        printf("No available PCB in the pool.\n");
        return;
    }

    PCB *newProcess = dequeue(&pcbPool);
    activatePCB(newProcess,++cnt,10);
    enqueue(&readyQueue, newProcess);
    printf("Process with PID %d created and added to ready queue.\n", newProcess->pid);
}

// 销毁进程
void destroyProcess()
{
    if (runningQueue == NULL)
    {
        printf("No running process to destroy.\n");
        return;
    }

    PCB *processToDestroy = runningQueue;
    runningQueue = runningQueue->next;
    processToDestroy->status = 0; // 将状态设置为空闲
    enqueue(&pcbPool, processToDestroy);
    mergePCBs();
    printf("Process with PID %d destroyed and returned to PCB pool.\n", processToDestroy->pid);
}

// 时间片到
void timeSliceExpired()
{
    if (runningQueue == NULL)
    {
        printf("No running process.\n");
        return;
    }

    PCB *expiredProcess = runningQueue;
    runningQueue = runningQueue->next;
    expiredProcess->status = 1; // 变为就绪状态
    enqueue(&readyQueue, expiredProcess);
    printf("Process with PID %d time slice expired, moved to ready queue.\n", expiredProcess->pid);

    // 调度下一个就绪进程
    if (readyQueue != NULL)
    {
        PCB *nextProcess = dequeue(&readyQueue);
        nextProcess->status = 2; // 设置为运行中状态
        enqueue(&runningQueue, nextProcess);
        printf("Process with PID %d is now running.\n", nextProcess->pid);
    }
}

// 挂起进程
void suspendProcess()
{
    if (runningQueue == NULL)
    {
        printf("No running process to suspend.\n");
        return;
    }

    PCB *suspendedProcess = runningQueue;
    runningQueue = runningQueue->next;
    suspendedProcess->status = 3; // 等待中状态
    enqueue(&waitingQueue, suspendedProcess);
    printf("Process with PID %d suspended and moved to waiting queue.\n", suspendedProcess->pid);
}

// 激活进程
void activateProcess()
{
    if (waitingQueue == NULL)
    {
        printf("No waiting process to activate.\n");
        return;
    }

    PCB *activatedProcess = dequeue(&waitingQueue);
    activatedProcess->status = 1; // 变为就绪状态
    enqueue(&readyQueue, activatedProcess);
    printf("Process with PID %d activated and moved to ready queue.\n", activatedProcess->pid);
}

// 显示队列
void displayQueues()
{
    printf("\nRunning Queue:\n");
    PCB *current = runningQueue;
    while (current != NULL)
    {
        printf("PID: %d, Status: %d\n", current->pid, current->status);
        current = current->next;
    }

    printf("\nReady Queue:\n");
    current = readyQueue;
    while (current != NULL)
    {
        printf("PID: %d, Status: %d\n", current->pid, current->status);
        current = current->next;
    }

    printf("\nWaiting Queue:\n");
    current = waitingQueue;
    while (current != NULL)
    {
        printf("PID: %d, Status: %d\n", current->pid, current->status);
        current = current->next;
    }

    printf("\nPCB Pool:\n");
    current = pcbPool;
    while (current != NULL)
    {
        printf("PID: %d, Status: %d\n", current->pid, current->status);
        current = current->next;
    }
}

// 将进程加入队列
void enqueue(PCB **queue, PCB *process)
{
    if (*queue == NULL)
    {
        *queue = process;
        process->next = NULL;
    }
    else
    {
        PCB *current = *queue;
        while (current->next != NULL)
        {
            current = current->next;
        }
        current->next = process;
        process->next = NULL;
    }
}

// 从队列中取出进程
PCB *dequeue(PCB **queue)
{
    if (*queue == NULL)
    {
        return NULL;
    }
    else
    {
        PCB *process = *queue;
        *queue = (*queue)->next;
        process->next = NULL;
        return process;
    }
}

// 合并空闲的PCB块
void mergePCBs()
{
    PCB *current = pcbPool;
    while (current != NULL && current->next != NULL)
    {
        if (current->status == 0 && current->next->status == 0)
        {
            PCB *tmp = current->next;
            current->next = tmp->next;
            free(tmp);
            printf("Merged two consecutive empty PCBs.\n");
        }
        else
        {
            current = current->next;
        }
    }
}