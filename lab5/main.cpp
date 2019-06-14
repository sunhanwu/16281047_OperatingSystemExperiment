#include <stdio.h>
#include "IO.h"
#include "FS.h"
#include <string.h>

void ShowDir();
void ShowBitMap(void);

int main()
{
    int choice;
    char filename[12];
    printf("************文件系统**************\n");
    printf("\t1. 创建新的磁盘\n");
    printf("\t2. 从文件中恢复磁盘\n");
    printf("请选择:");
    scanf("%d",&choice);
    if(choice==1)
        Init();
    else if(choice==2)
    {
        printf("请输入加载文件名:");
        scanf("%s",filename);
        load(filename);
    }
    printf("文件系统准备完毕...\n");
    while (1)
    {
        int choice2;
        int flag=0;
        printf("\t1. 查看目录\t2. 创建文件\n");
        printf("\t3. 删除文件\t4. 打开文件\n");
        printf("\t5. 修改文件\t6. 查看位图\n");
        printf("\t7. 保存磁盘\t8. 退出\n");
        printf("请选择：");
        scanf("%d",&choice2);
        switch (choice2)
        {
            case 1:ShowDir();break;
            case 2:
                printf("请输入要创建的文件名：");
                scanf("%s",filename);
                create(filename);
                break;
            case 3:
                printf("请输入要删除的文件名：");
                scanf("%s",filename);
                destroy(filename);
                break;
            case 4:
                printf("请输入要打开的文件名：");
                scanf("%s",filename);
                ReadFile(filename);
                break;
            case 5:
                int choice3;
                printf("1. 增加内容\t2. 删除内容");
                printf("\n请选择：");
                scanf("%d",&choice3);
                printf("请输入要修改的文件名:");
                scanf("%s",filename);
                if(choice3==1)
                    ChangeFileAdd(filename);
                else if(choice3==2)
                    ChangeFileDel(filename);

                break;
            case 6:
                ShowBitMap();
                break;
            case 7:
                printf("请输入要保存的文件名：");
                scanf("%s",filename);
                save(filename);
                break;
            case 8:
                flag=1;
                break;
            default:
                break;
        }
        if(flag)
            break;

    }

    return 0;
}

void ShowDir()
{
    int index =1;
    int exist = 0;
    printf("****************目录***********************\n");
    printf("当前目录下文件有:\n");
    for(int i=0;i<32;i++)
    {
        if(strlen(menuitem[i].FileName)!=0)
        {
            printf("%d %s %dB\n",index,menuitem[i].FileName,filedescriptor[menuitem[i].FileDescriptorNum].Length);
            exist++;
        }
        index++;
    }
    printf("一共存在%d个文件\n",exist);
    printf("****************************************\n");
}

void ShowBitMap(void)
{
    printf("\n****************位图**********************\n");
    int used = 0;
    printf("当前的磁盘使用情况如下(Y表示使用，N表示未使用)\n");
    for(int i=0;i<C;i++)
    {
        printf("%d号柱面磁盘信息如下:\n",i);
        printf("   区:0 1 2 3 4 5 6 7 8 9\n");
        printf("头\n");
        for(int j=0;j<H;j++)
        {
            printf("%d\t:",j);
            for(int k=0;k<B;k++)
            {
                int t=i*H*B+j*B+k;
                if(t<512)
                {
                    printf("%c ",ldisk[0][0][0].Content[t]);
                    if(ldisk[0][0][0].Content[t] == 'Y')
                        used++;
                }
                else
                {
                    printf("%c ",ldisk[0][0][1].Content[t-512]);
                    if(ldisk[0][0][0].Content[t] == 'Y')
                        used++;
                }
            }
            printf("\n");
        }
    }
    printf("总共使用%d个磁盘块，剩余%d个磁盘块空闲\n",used,(C*B*H-used));
    printf("****************************************\n");
}

