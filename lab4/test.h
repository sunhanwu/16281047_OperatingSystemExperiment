//
// Created by sun on 2019-05-23.
//

#ifndef LAB4_TEST_H
#define LAB4_TEST_H

#include <stdio.h>
#include <stdlib.h>
#include <queue>
using namespace std;
//
typedef struct WorkItem
{
    int seq;
    int priority;
    char Used;
    char Modified;
}WorkItem;

typedef struct MemSchedule
{
    WorkItem* WorkSpace=NULL;
    int *VisitSeq=NULL;
    int N=64;
    int p=0;
    //工作集大小
    int e=5;
    int m=1;
    //定义访问序列长度o
    int length;
    //当前已使用物理块数
    int work_len=0;
    //发生替换的物理块号
    int change=0;
    float t,r;
//   队列长度
    int queue_free_len = 0;
    int queue_modified_len = 0;
    WorkItem free[2];
    WorkItem Modified[2];


}MemSchedule;


MemSchedule SetMemSchedule(MemSchedule s1)
{
    printf("请输入虚拟内存尺寸N:");
    scanf("%d",&s1.N);
    printf("请输入工作集起始位置p:");
    scanf("%d",&s1.p);
    printf("请输入工作集包含页数e:");
    scanf("%d",&s1.e);
    printf("请输入工作集移动效率m:");
    scanf("%d",&s1.m);
    printf("**************参数***************\n");
    printf("虚拟内存:%d\t",s1.N);
    printf("起始位置p:%d\n",s1.p);
    printf("包含页数e:%d\t",s1.e);
    printf("移动效率m:%d\n",s1.m);
    printf("********************************\n");
    return s1;

}

MemSchedule GeneVisitSeq(int l)
{
    MemSchedule schedule1;
    printf("是否设定调度算法参数(Y/N):");
    char flag;
    scanf("%c",&flag);
    if (flag=='Y')
    {
        schedule1=SetMemSchedule(schedule1);
    }
	schedule1.t=rand()%1000/1000.0;
    schedule1.VisitSeq=(int *)malloc(l*sizeof(schedule1.m));
    printf("生成随机内存访问序列：\n");
    for (int i=0;i<l;i++)
    {
        for (int j=0;j<schedule1.m;j++)
        {
            schedule1.VisitSeq[i*schedule1.m+j] = rand()%schedule1.e+schedule1.p;
            printf("%d ",schedule1.VisitSeq[i*schedule1.m+j]);
        }
        schedule1.r=rand()%1000/1000.0;
        if(schedule1.r<schedule1.t)
        {
            schedule1.p=(schedule1.p+1)%schedule1.N;
        }
    }
    printf("\n");
    schedule1.length=l*schedule1.m;
    schedule1.WorkSpace = (WorkItem *)malloc(schedule1.e *sizeof(WorkItem));
    return schedule1;
}

//此函数用于向前或者向后查询访问序列中某个页号出现的位置
// seq 参数是访问序列，work_now是当前在序列中的位置，target是目标页号，direct是查询方向 0表示正向搜索，1表示反向搜索，
//length表示序列长度
//返回值：若未找到则返回-1，找到返回距离
int SearchVisitSqe(int seq[],int work_now,int target,int direct,int length)
{
    if(direct == 0)
    {
        //当指定方向是从前往后搜索的时候
        for (int i=0;i<length-work_now;i++)
        {
            if (seq[work_now+i+1] == target)
                return i+1;
        }
        return -1;
    }
    else
    {
        //当指定方向是从后往前搜索的时候
        for (int i=0;i<work_now;i++)
        {
            if (seq[work_now-i] == target)
                return i;
        }
        return -1;
    }
}

//此函数用于打印空闲队列和修改队列
WorkItem* Insert(WorkItem* q,int len,WorkItem item,int priority)
{
    if(len==0)
    {
        q[0] = item;
        return q;
    }
    else if(len==1)
    {
        q[1] = item;
        return q;
    }
    else
    {
        if(q[0].priority > q[1].priority)
        {
            q[0] = item;
            return q;
        }
        else
        {
            q[1] = item;
            return q;
        }
    }

}

#endif //LAB4_TEST_H
