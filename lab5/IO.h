//
// Created by sun on 2019-06-05.
//

#ifndef LAB5_IO_H
#define LAB5_IO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "FS.h"
#define L 1000 //逻辑块数量
#define C 10 // 柱面号
#define H 10 //磁头号
#define B 10 //扇区号

typedef struct BLOCK
{
    char Content[512]; //逻辑块存储的内容
    int BlockNnum; //逻辑块号
    int c; // 柱面号
    int h; //磁头号
    int b; //扇区号
}BLOCK;

BLOCK ldisk[C][H][B];//磁盘模型

int DiskNumToBlock(int c,int h,int b)
{
    return b+b*h+b*h*c;//返回
}

void InitDisk(void)
{
    for(int i=0;i<C;i++)
        for(int j=0;j<H;j++)
            for(int k=0;k<B;k++)
            {
                ldisk[i][j][k].c=i;
                ldisk[i][j][k].h=j;
                ldisk[i][j][k].b=k;
                ldisk[i][j][k].BlockNnum=DiskNumToBlock(i,j,k);//计算对应的逻辑块号
            }
}

void ReadBlock(int i,char *p)
{
    int c,h,b;//磁盘的柱面 磁道 扇区
    c = i % (H*B);//
    h = (i -c*H*B) % B;//
    b = i-c*H*B - h*B;//
    memcpy(p,ldisk[c][h][b].Content,512);
}

void WriteBlock(int i,char *p)
{
    int c,h,b;
    b = i % B;
    h = ((i - b) / B) % H;
    c = (i -b -h*B) / (H*B);
    b = i -c*H*B -h*B ;
    memcpy(ldisk[c][h][b].Content,p,512);
}
//将磁盘写入文件中
void DiskToFile(char filename[])
{
//    FileDescriptorToDisk();
    FILE *fp;
    fp = fopen(filename,"wb");
    //判断fp打开成功
    if (fp ==NULL)
    {
        printf("File Open Fail");
        exit(1);
    }
    // 循环遍历，将磁盘块内容写入二进制文件中去
    for(int i=0;i<C;i++)
        for(int j=0;j<H;j++)
            for(int k=0;k<B;k++)
                //以二进制的形式写入二进制文件中
                fwrite(ldisk[i][j][k].Content,512,1,fp);
    fclose(fp);
}
//从文件中读取数据，恢复磁盘系统
void FileToDisk(char filename[])
{
     FILE *fp;
     fp = fopen(filename,"rb");
     if(fp == NULL)
     {
         printf("File Open Fail");
         exit(1);
     }
     int index = 0;
     while(!feof(fp))
     {
         int c,h,b;
         b = index % B;
         h = ((index - b) / B) % H;
         c = (index -b -h*B) / (H*B);
         b = index -c*H*B -h*B ;
         fread(ldisk[c][h][b].Content,512,1,fp);
         ldisk[c][h][b].c = c;
         ldisk[c][h][b].h = h;
         ldisk[c][h][b].b = b;
         index++;
     }
     fclose(fp);
}

//用于修改磁盘位图
void ChangeBitMap(int i,char p)
{
    if(i <512)
        ldisk[0][0][0].Content[i] = p;
    else
        ldisk[0][0][1].Content[i-512] = p;
}

//初始化磁盘位图
void InitBitMap(void)
{
    //第0，1号磁盘已经被占用
    ChangeBitMap(0,'Y');
    ChangeBitMap(1,'Y');
    for (int i=2;i<C*H*B;i++)
    {
        ChangeBitMap(i,'N');
    }
}

//搜索位图，找到空闲磁盘块号
int SearchBitMap(void)
{
    for(int i=14;i<C*H*B;i++)
    {
        if(i<512)
        {
            if(ldisk[0][0][0].Content[i]=='N')
                return i;
        }
        else
        {
            if(ldisk[0][0][1].Content[i-512]=='N')
                return i;
        }
    }
}

#endif //LAB5_IO_H
