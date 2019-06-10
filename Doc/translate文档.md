# translate.hpp库文件文档

## 简介

​	在本项目中，将map.json文件解析到obj_list结构

## APIs

### 类

#### obj_list

```
成员数据
	obj objs[]	组件数组大小
	int length	组件数
成员函数
	 obj_list load_map(const char *filename)
	 从文件名为filename的文档中解析json数据到该类中，返回值为该结构体本身
	 （注：该接口为调用时的主要接口）
	 例：obj_list objects;
	 	objects.load_map("map.json");
```

#### obj

```
成员数据
	string type;					组件类型
	locate beginLocate,endLocate;	起止位置
	timestamp beginTime,endTime;	开始和结束时间
	int rotate;						旋转角度（正数值为顺时针）
成员函数
	无
```

#### locate

```
成员数据
	double x,y;	x,y坐标
成员函数
	locate load_locate(string ss)
	将形为(x,y)的坐标反序列化到结构体，返回值为该结构体本身
```

#### timestamp

成员数据

		int minute,second,millisecond;	未转化到毫秒表示的原始数据
		int ms;							毫秒值
	成员函数
		timestamp load_timestamp(string s)
		将形为xx:xx:xxxx的时间反序列化到结构体并转化到毫秒
