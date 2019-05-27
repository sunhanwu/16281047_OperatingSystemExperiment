//
// Created by sun on 2019-05-23.
//

#ifndef LAB4_OPTIMAL_H
#define LAB4_OPTIMAL_H

#include <stdio.h>
#include "test.h"
#include <time.h>

int Optimal(MemSchedule s1)
{
    //start和finish用于记录时间
    clock_t start,finish;
    //num表示优先级，每访问一个指令加一，所以优先级最低的就是最久没有被访问的到的，选择其替换
    int miss=0;
    start = clock();
    printf("***************最佳置换算法*****************\n");
    printf("seq\t");
    for (int i=0;i<s1.e;i++)
    {
        printf("%d\t",i+1);
    }
    printf("\n");
    for (int i=0;i<s1.length;i++)
    {
        int pointer=s1.VisitSeq[i];
        printf("%d:\t",pointer);
        //判断当前是否还有空闲物理块
        if (s1.work_len<s1.e)
        {
            //此处的flag用于判断当前访问的序列是已经在内存中
            int flag=0;
            //扫描一遍当前的内存块，进行更新优先级操作和查询下一个访问的页号是否在内存中
            for (int m=0;m<s1.work_len;m++)
            {
                if (s1.WorkSpace[m].seq==pointer)
                    //若序列在内存中则将flag置为1
                    flag=1;
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
        else
        {
            //表示没有空闲内存块
            //还是使用flag表示当前物理块在内存中是否出现
            int flag=0;
            for (int m=0;m<s1.work_len;m++)
            {
               if (s1.WorkSpace[m].seq == pointer)
                   flag = 1;
            }
            if (flag == 1)
            {
                //表示内存块已经出现过
                s1.change = -1;
            }
            if (flag == 0)
            {
                //min表示最小优先级的那个内存块号
                int min_pri=0xffffff;
                for ( int m=0;m<s1.work_len;m++)
                {
                    int n = SearchVisitSqe(s1.VisitSeq,i-1,s1.WorkSpace[m].seq,0,s1.length-s1.work_len+m);
                    s1.WorkSpace[m].priority = n;
                    if (n < min_pri)
                    {
                        s1.change = m;
                        min_pri = n;
                    }
                }
                //表示发生替换变化的块号s1.change
                s1.WorkSpace[s1.change].seq = pointer;
            }
            for (int m=0;m<s1.e;m++)
            {
                if (m == s1.change)
                {
                    printf("%d@\t", s1.WorkSpace[m].seq);
                    miss++;
                }
                else
                    printf("%d\t", s1.WorkSpace[m].seq);
            }
        }
        printf("\n");
        finish = clock();
    }

    printf("******************算法评估**************\n");
    printf("缺页率:\t%.2f %%\n",(miss*1.0)/s1.length*100);
    printf("时间开销:\t%.2f ms\n",(double)(finish-start));
    free(s1.WorkSpace);
    free(s1.VisitSeq);
    return 0;
}

#endif //LAB4_OPTIMAL_H
