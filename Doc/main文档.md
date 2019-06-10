# main.cpp主程序文档

## 定义

```
#define MAXN 100010     宏定义MAXN表示100010，表示数组大小
#define X first         宏定义X表示first，记录pair类型的first
#define Y second        宏定义Y表示second，记录pair类型的second
const int W = 1003      常量W表示图形界面的宽度
const int H = 602       常量H表示图形界面的高度
typedef pair<int, int> Dnt     将pair类型定义为Dnt
```

## 结构体

### Line

```
成员数据
    Dnt S          元素起始位置
    Dnt T          元素结束位置
    Dnt dJ         判定框偏移量
    Dnt dA         判定算法偏移量
    int st         开始时间
    int et         结束时间
    int judst      判定框开始时间
    int judet      判定框结束时间
    bool jud_Del   判定框是否被消除	
    int State      特别记录判定结果元素类型
    PIMAGE img     元素图像

成员函数
    无
```

## 类

### Interface

```
成员数据
    int pos_Play[4]     Play键位置
    int pos_Musi[4]     Music键位置
    int pos_Clos[4]     Close键位置
    bool Play_Button    Play键是否被按下
    bool Musi_Button    Music键是否被按下 
    bool Clos_Button    Close键是否被按下
    bool Musi_On        音乐是否正在播放
    bool Mouse_Up       鼠标是否按下
    PIMAGE img[2]       图片引用指针
    MUSIC music         音乐引用指针
    mouse_msg msg       鼠标状态指针

成员函数
    bool is_Play(int x, int y)  
    判断坐标(x,y)是否位于Play键范围内

    bool is_Musi(int x, int y)
    判断坐标(x,y)是否位于Music键范围内

    bool is_Clos(int x, int y) 
    判断坐标(x,y)是否位于Close键内部

    void Init()
    导入所有图片文件及音乐文件

    void Climate() 
    清除屏幕上的所有内容

    void MouseEventSolve()
    鼠标事件处理函数

    void Make_Interface()
    生成主界面并开始播放背景音乐
```

### Game_Play

```
成员数据
    PIMAGE img[15]     元素图片指针
    PIMAGE bpg[15]     背景图片指针
    MUSIC music        音乐指针
    mouse_msg msg      鼠标状态指针
    int Lnum           元素的数量
    int score          当前得分
    int sum[4]         判定结果数量
    Line q[MAXN]       双端队列，用于实现判定结果动画实现

成员函数
    int Judge(int jx, int jy, int x, int y)
    判定算法函数，用于判定当前玩家点击操作，其返回值：
    1：Perfect!
    2：Good!
    3：Bad!
    4：Miss!

    void putjudge(int x, int y)
    输出判定中心坐标为(x,y)的手绘判定框

    bool Check_End(int Time)
    判定当前时间为Time时动画是否已经结束

    void Init()
    导入所有图片文件和音乐文件，并设置字体

    void Load_Map1() ~ void Load_Map11()
    读入地图

    void Test(int CASE)
    输出背景图片为img[CASE]时的所有静态元素，用于测试与调整元素位置

    void Play(int CASE)
    制作背景图片为img[CASE]时的动画，并判定玩家操作进行统计

    void Report()
    输出统计结果

    void Start()
    开始游戏

```