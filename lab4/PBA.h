//
// Created by sun on 2019-05-27.
//

#ifndef LAB4_PBA_H
#define LAB4_PBA_H

#include "test.h"
#include <stdio.h>
#include <time.h>
//using namespace std;

int PBA(MemSchedule s1)
{
    int miss = 0;
    clock_t start,finish;
    start = clock();
    printf("***************PBA算法*****************\n");
    printf("seq\t");
    for (int i=0;i<s1.e;i++)
    {
        printf("%d\t",i+1);
    }
    printf("\n");
    for (int i=0;i<s1.length;i++)
    {
        //pointer指向下一个将要访问的页号
        int pointer=s1.VisitSeq[i];
        printf("%d:\t",pointer);
        //判断当前是否还有空闲物理块
        if (s1.work_len<s1.e)
        {
            //表示还有剩余的空闲物理块
            int flag = 0;//flag用于标记下一个页号是否在物理块中
            //扫描一遍工作集，查看下一个页号是否存在
            for (int m=0;m<s1.work_len;m++)
            {
                s1.WorkSpace[m].priority--;
                if (s1.WorkSpace[m].seq==pointer)
                    //若序列在内存中则将flag置为1
                {
                    flag=1;
                    s1.WorkSpace[m].priority = s1.e;
                }
            }
            if (flag==1)
            {
                //若下一个访问的页号在内存中，则没有发生缺页，不需要发生替换,change值表示这一次没有
                //缺页
                s1.change = -1;
            }
            if(flag==0)
            {
                //如下一个要访问的页号不在内存中，则发生缺页，需要直接将新的页号放入下一个空闲块中
                //首先更新全部优先级
                //直接将下一个空闲快替换
                s1.WorkSpace[s1.work_len].seq = pointer;
                s1.WorkSpace[s1.work_len].priority = s1.e;
                //将替换变化了的内存块号写入
                s1.change = s1.work_len;
                //将已使用的内存数量加一
                s1.work_len++;
            }
            //打印信息
            for (int k=0;k<s1.work_len;k++)
            {
                //如果发生了页面置换，则打印带@的信息
                if(k==s1.change)
                {
                    printf("%d@\t",s1.WorkSpace[k].seq);
                    //缺页率加一
                    miss++;
                }
                else
                    printf("%d\t",s1.WorkSpace[k].seq);
            }
            //剩下空闲的页面全部打印*
            for (int j=0;j<s1.e-s1.work_len;j++)
            {
                printf("*\t");
            }


        }
        printf("\n");
    }

}
#endif //LAB4_PBA_H
