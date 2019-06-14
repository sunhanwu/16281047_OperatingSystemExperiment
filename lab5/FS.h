//
// Created by sun on 2019-06-06.
//

#ifndef LAB5_FS_H
#define LAB5_FS_H

#include "IO.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define DiskNumLen 3


void FileDescriptorToDisk(void);
void MenuToFileDescriptor(void);
//磁盘的第0、1块是存储位图
//磁盘的第2-9块用于存储文件描述符
//对齐后这个文件描述符结构体占据24字节
#pragma pack(4)
typedef struct FileDescriptor //此文件描述符总共占据磁盘16字节
{
    int Length;//文件长度
    int DiskNum[DiskNumLen]; //第二个3只是表示每个磁盘块好最大长度是3位
    int Num; //文件描述符号
    char IsFree; //表示此文件描述符是否空闲
}FileDescriptor;
#pragma pack(pop)

FileDescriptor filedescriptor[256];
#pragma pack(4)
typedef struct MenuItem //目录对应0号文件描述符,一个目录项占据16字节，所以一个文件描述符可以存储96个文件
{
    char FileName[12]; //目录项中文件名的最大长度为16字节
    int FileDescriptorNum;//文件描述符号
}MenuItem;
MenuItem menuitem[31];//目录项数组
#pragma pack(pop)
//用于修改文件描述符
void ChangeFileDescriptor(FileDescriptor *f,int length,int DiskNum[],int Num,char p)
{
    f->Length = length;
    for(int i=0;i<DiskNumLen;i++)

    {
        f->DiskNum[i] = DiskNum[i];
    }
    f->Num = Num;
    f->IsFree = p;
}
//初始化所有文件描述符
void InitFileDescriptor(void)
{
    for(int i=0;i<256;i++)
    {
      int  DiskNum[3]; //磁盘号数组
      DiskNum[0] = i;
      DiskNum[1] = -1;
      DiskNum[2] = -1;
      int FileDescriptorNum = i; //文件描述符号
      ChangeFileDescriptor(&filedescriptor[i],0,DiskNum,FileDescriptorNum,'Y');
    }
    filedescriptor[0].IsFree = 'N';
    FileDescriptorToDisk();

}

//将文件描述符写入磁盘中
void FileDescriptorToDisk(void)
{
    char temp_block[512];
    int index = 0;
    int DiskNumIndex = 2;
    for(int i=0;i<256;i++)
    {
        char temp_descriptor[24];
        memcpy(temp_descriptor,&filedescriptor[i], sizeof(FileDescriptor));
        memcpy(&temp_block[index*24],temp_descriptor,24);
        index++;
        int t = index % 21;
        if(t == 0)
        {
            index = 0;
            if(DiskNumIndex<10)
            {
                memcpy(ldisk[0][0][DiskNumIndex].Content,temp_block,512);
                ChangeBitMap(DiskNumIndex,'Y');
            }
            else
            {
                memcpy(ldisk[0][1][DiskNumIndex-B].Content,temp_block,512);
                ChangeBitMap(DiskNumIndex,'Y');//修改位图
            }
            DiskNumIndex++;
        }

    }
}

//将磁盘读取的信息恢复
void DiskToFileDescriptor(void)
{
    for(int i=2;i<15;i++)
    {
        char temp[512];
        if (i<B)
            memcpy(temp,ldisk[0][0][i].Content,512);
        else
            memcpy(temp,ldisk[0][1][i-B].Content,512);
        for (int j=0;j<21;j++)
        {
            if(((i-2)*21+j)>256)
                break;
            char temp_FileDescriptor[24];
            memcpy(temp_FileDescriptor,&temp[j*24],24);
            FileDescriptor *f;
            f = (FileDescriptor*)temp_FileDescriptor;
            int num = (i-2)*21+j;
            filedescriptor[num].IsFree = f->IsFree;
            filedescriptor[num].DiskNum[0] = f->DiskNum[0];
            filedescriptor[num].DiskNum[1] = f->DiskNum[1];
            filedescriptor[num].DiskNum[2] = f->DiskNum[2];
            filedescriptor[num].Length = f->Length;
            filedescriptor[num].Num = f->Num;
        }
    }
}

//初始化目录
void InitMenu(void)
{
    for(int i=0;i<32;i++)
        menuitem[i].FileDescriptorNum = i;
}

//将目录内容写入文件描述中
void MenuToFileDescriptor(void)
{
    char temp_FileDescriptor[512];
    for(int i=0;i<32;i++)
    {
        char temp_menuitem[16];
        memcpy(temp_menuitem,&menuitem[i],16);
        memcpy(&temp_FileDescriptor[i*16],temp_menuitem,16);
    }
    filedescriptor[0].IsFree = 'N';
    filedescriptor[0].DiskNum[0] = SearchBitMap();
    filedescriptor[0].Length = 512;
    WriteBlock(filedescriptor[0].DiskNum[0],temp_FileDescriptor);
}

void FileDescriptorToMenu(void)
{
    char MenuContent[512];
    ReadBlock(filedescriptor[0].DiskNum[0],MenuContent);
    for(int i=0;i<32;i++)
    {
        char temp_menuitem[16];
        memcpy(temp_menuitem,&MenuContent[i*16],16);
        MenuItem *t;
        t = (MenuItem *)temp_menuitem;
        strcpy(menuitem[i].FileName,t->FileName);
        menuitem[i].FileDescriptorNum = t->FileDescriptorNum;
    }
}

int SearchFileDescriptor()
{
    for(int i=0;i<256;i++)
    {
        if(filedescriptor[i].IsFree == 'Y')
            return i;
    }
    return -1;
}

int SearchMenuItem()
{
    for(int i=0;i<32;i++)
    {
        if(strlen(menuitem[i].FileName)==0)
            return i;
    }
    return -1;
}

//下面是文件系统与用户直接的接口
void create(char filename[])
{
    int FileDescriptorNum,MenuItemNum,DiskNum;
    //寻找空闲目录项
    MenuItemNum = SearchMenuItem();
    strcpy(menuitem[MenuItemNum].FileName,filename);
    //寻找空闲文件描述符
    FileDescriptorNum = SearchFileDescriptor();
    menuitem[MenuItemNum].FileDescriptorNum = FileDescriptorNum;
    //寻找空闲磁盘块
    DiskNum = SearchBitMap();
    filedescriptor[FileDescriptorNum].DiskNum[0] = DiskNum;
    filedescriptor[FileDescriptorNum].IsFree = 'N';
    //修改磁盘位图
    ChangeBitMap(DiskNum,'Y');
}

void destroy(char filename[])
{
    int MenuItemNum=-1;
    for(int i=0;i<32;i++)
        if(strcmp(menuitem[i].FileName,filename)==0)
            MenuItemNum = i;
    if(MenuItemNum==-1)
    {
        printf("目录中没有此文件！\n");
        return;
    }
    int FileDesCriptorNum = menuitem[MenuItemNum].FileDescriptorNum;
    //将目录项重置,重置时只需要将文件名删除，而不需要重置文件描述符，因为前面判断文件是否存在的条件是文件名是否存在
    memset(menuitem[MenuItemNum].FileName,0, sizeof(menuitem[MenuItemNum].FileName));
    //修改文件描述符为空闲状态
    filedescriptor[FileDesCriptorNum].IsFree = 'Y';
    for(int i=0;i<3;i++)
    {
        if(filedescriptor[FileDesCriptorNum].DiskNum[i]!=-1)
        ChangeBitMap(filedescriptor[FileDesCriptorNum].DiskNum[i],'N');
    }
}

int open(char filename[])
{
    int MenuItemNum=-1;
    for(int i=0;i<32;i++)
        if(strcmp(menuitem[i].FileName,filename)==0)
            MenuItemNum = i;
    if(MenuItemNum==-1)
    {
        printf("目录中没有此文件！\n");
        return -1;
    }
    else
        //返回文件描述符号
        return menuitem[MenuItemNum].FileDescriptorNum;
}

char* read(int index,int mem_area,int count)
{
   char *temp;
   char block[512];
   temp = (char *)malloc(count* sizeof(char));
   ReadBlock(filedescriptor[index].DiskNum[0],block);
   memcpy(temp,&block[mem_area],count);
   return temp;
}

void write(int index,int mem_area,int count,char content[])
{
    char temp[512];
    char *s1=(char*)malloc(mem_area* sizeof(char));
    char *s2=(char*)malloc(mem_area* sizeof(char));
    char *s;
    int DiskNum = filedescriptor[index].DiskNum[0];
    ReadBlock(DiskNum,temp);
    memcpy(s1,temp,mem_area);
    memcpy(s2,&temp[mem_area],512-mem_area);
    s = strcat(s1,content);
    s = strcat(s,s2);
    filedescriptor[index].Length = strlen(s);
    WriteBlock(DiskNum,s);
}

char* ReadFile(char filename[])
{
    int index = open(filename);
    char content[512];
    ReadBlock(filedescriptor[index].DiskNum[0],content);
    printf("%s\n",content);
    return content;
}

void ChangeFileAdd(char filename[])
{
    int mem_area,count;
    char *s;
    int index = open(filename);
    char *p = ReadFile(filename);
    printf("请输入要增加的位置(向后增加)：");
    scanf("%d",&mem_area);
    printf("请输入增加的字节数：");
    scanf("%d",&count);
    s = (char*)malloc(count* sizeof(char));
    printf("请输入增加的内容:");
    scanf("%s",s);
    write(index,mem_area,count,s);
    ReadFile(filename);
    return;
}

void ChangeFileDel(char filename[])
{
    int mem_area,count;
    int index = open(filename);
    printf("请输入要删除的位置(向后删除)：");
    scanf("%d",&mem_area);
    printf("请输入删除的字节数：");
    scanf("%d",&count);
    char temp[512];
    ReadBlock(filedescriptor[index].DiskNum[0],temp);
    char *s1=(char*)malloc(mem_area* sizeof(char));
    char *s2=(char*)malloc((512-mem_area-count)* sizeof(char));
    memcpy(s1,temp,mem_area);
    memcpy(s2,&temp[mem_area+count],512-mem_area-count);
    s1 = strcat(s1,s2);
    WriteBlock(filedescriptor[index].DiskNum[0],s1);
    filedescriptor[index].Length=strlen(s1);
    ReadFile(filename);
    return;
}
void Init(void)
{
    InitDisk();
    InitBitMap();
    InitFileDescriptor();
    InitMenu();
}

void load(char filename[])
{
    FileToDisk(filename);
    DiskToFileDescriptor();
    FileDescriptorToMenu();
}

void save(char filename[])
{
    MenuToFileDescriptor();
    FileDescriptorToDisk();
    DiskToFile(filename);
}
#endif //LAB5_FS_H
