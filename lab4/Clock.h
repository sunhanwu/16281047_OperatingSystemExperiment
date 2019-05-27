//
// Created by sun on 2019-05-27.
//

#ifndef LAB4_CLOCK_H
#define LAB4_CLOCK_H

#include "test.h"
#include <stdio.h>
#include <time.h>

int Clock(MemSchedule s1)
{
    int miss = 0;
    clock_t start,finish;
    start = clock();
    printf("***************改进Clock算法*****************\n");
    printf("seq\t");
    for (int i=0;i<s1.e;i++)
    {
        printf("[%d]UM\t",i+1);
    }
    printf("\n");
    for (int i=0;i<s1.length;i++)
    {
        //pointer指向下一个将要访问的页号
        int pointer=s1.VisitSeq[i];
        printf("%d:\t",pointer);
        //判断当前是否还有空闲物理块
        if (s1.work_len < s1.e)
        {
            //表示还有空闲物理块
            int flag = 0;//表示下一个要访问的页号是否在内存块中
            int flag1 = 0;//标记与下一个页号相同的内存块号
            for (int m=0;m<s1.work_len;m++)
            {
                if (s1.WorkSpace[m].seq==pointer)
                    //若序列在内存中则将flag置为1
                {
                    flag=1;
                    flag1 = m;
                    break;

                }
            }
            if (flag==1)
            {
                //若下一个访问的页号在内存中，则没有发生缺页，不需要发生替换,change值表示这一次没有
                //缺页
                s1.change = -1;
                s1.WorkSpace[flag1].Modified = 'Y';
            }
            if(flag==0)
            {
                //如下一个要访问的页号不在内存中，则发生缺页，需要直接将新的页号放入下一个空闲块中
                //首先更新全部优先级
                //直接将下一个空闲快替换
                s1.WorkSpace[s1.work_len].seq = pointer;
                //将刚刚调入内存的物理块号的使用位设置为1
                s1.WorkSpace[s1.work_len].Used = 'Y';
                s1.WorkSpace[s1.work_len].Modified = 'N';
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
                    printf("[%d]%c%c@\t",s1.WorkSpace[k].seq,s1.WorkSpace[k].Used,s1.WorkSpace[k].Modified);
                    //缺页率加一
                    miss++;
                }
                else
                    printf("[%d]%c%c\t",s1.WorkSpace[k].seq,s1.WorkSpace[k].Used,s1.WorkSpace[k].Modified);
            }
            //剩下空闲的页面全部打印*
            for (int j=0;j<s1.e-s1.work_len;j++)
            {
                printf("*\t");
            }



        }
        else
        {
            //此处表示没有空闲页面的情况
            //还是使用flag表示新调入的页面是否在内存中已经有了
            int flag = 0;
            int flag1 = 0;//flag1记录下一个要访问的页面在内存中的块号
            for (int m=0;m<s1.work_len;m++)
            {
                if (s1.WorkSpace[m].seq == pointer)
                {
                    flag = 1;
                    flag1  = m;
                    break;
                }
            }
            if (flag == 1)
            {
                //表示内存块已经出现过
                s1.WorkSpace[flag1].Modified = 'Y';
                s1.change = -1;
            }
            if (flag == 0)
            {
                //表示内存块中没有出现过
                for(int t=0;t<2;t++)
                {
                    int flag = 0;
                    for (int m=0;m<s1.work_len;m++)
                    {
                        if (s1.WorkSpace[m].Used =='N' && s1.WorkSpace[m].Modified =='N')
                        {
                            s1.change =m;
                            flag =1;
                            break;
                        }
                    }
                    if (flag == 0)
                    {
                        for (int m=0;m<s1.work_len;m++)
                        {
                            if(s1.WorkSpace[m].Used =='N' && s1.WorkSpace[m].Modified =='Y')
                            {
                                s1.change = m;
                                flag = 1;
                                break;
                            }
                            s1.WorkSpace[m].Used = 'N';
                        }
                    }
                    if(flag!=0)
                        break;

                }
                s1.WorkSpace[s1.change].seq = pointer;
                s1.WorkSpace[s1.change].Used = 'Y';
                s1.WorkSpace[s1.change].Modified = 'N';

            }
            //打印信息
            for (int k=0;k<s1.work_len;k++)
            {
                //如果发生了页面置换，则打印带@的信息
                if(k==s1.change)
                {
                    printf("[%d]%c%c@\t",s1.WorkSpace[k].seq,s1.WorkSpace[k].Used,s1.WorkSpace[k].Modified);
                    //缺页率加一
                    miss++;
                }
                else
                    printf("[%d]%c%c\t",s1.WorkSpace[k].seq,s1.WorkSpace[k].Used,s1.WorkSpace[k].Modified);
            }
        }
        printf("\n");
    }
    finish = clock();
    printf("******************算法评估**************\n");
    printf("缺页率:\t%.2f %%\n",(miss*1.0)/s1.length*100);
    printf("时间开销:%.2f ms\n",(double)(finish-start));
    free(s1.WorkSpace);
    free(s1.VisitSeq);
    return 0;
}
#endif //LAB4_CLOCK_H
