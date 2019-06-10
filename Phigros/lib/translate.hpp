struct obj;
struct locate;
struct timestamp;//转化到毫秒 
#include <fstream>
#include <string>
#include <cstdlib>
#include <streambuf>
#include "json.h"
#include <cmath>
#define Type_Length 20
#define Max_Object_numbers 1000
#define Length_pt 1003
#define Height_pt 602
#define Length_cm 33.867
#define Height_cm 19.05
using namespace std;
using namespace Json;


struct locate{
	double sx,sy;
	int x, y;
	locate() { sx = sy = x = y = 0; }

	void serialization() {
		this->x = floor(sx * Length_pt / Length_cm);
		this->y = floor(sy * Height_pt / Height_cm);
	}

	locate load_locate(string ss){
		string s,str_x,str_y;
		int sep;
		s = ss.substr(1,ss.length()-2);
		sep = s.find(',');
		str_x = s.substr(0,sep);
		str_y = s.substr(sep+1,s.length()-sep-1);
		this->sx = atof(str_x.c_str());
		this->sy = atof(str_y.c_str());
		this->serialization();
		return *this;
	}
};


struct timestamp{
	int minute,second,millisecond;
	int ms;
	
	timestamp() { minute = second = millisecond = ms = 0; }
	timestamp set_stamp(int mit,int sec,int msd){
		this->minute = mit;
		this->second = sec * 10 + msd / 1000;
		this->millisecond = msd % 1000;
		this->ms = minute*60000 + second*1000 + millisecond;
		return *this;
	}
	
	timestamp load_timestamp(string s){
		string str_a,temp,str_b,str_c;
		int sep;
		sep = s.find(':');
		str_a = s.substr(0,sep);
		temp = s.substr(sep+1,s.length()-sep-1);
		sep = temp.find(':');
		str_b = temp.substr(0,sep);
		str_c = temp.substr(sep+1,temp.length()-sep-1);
		int a = atoi(str_a.c_str());
		int b = atoi(str_b.c_str());
		int c = atoi(str_c.c_str());
		this->set_stamp(a,b,c);
		return *this;
	}
};


struct obj{
		string type;
		locate beginLocate,endLocate;
		timestamp beginTime,endTime;
		int rotate;
		obj(){}
};


struct obj_list{
	obj objs[Max_Object_numbers];
	int length;
	obj_list(){}
	obj_list load_map(const char *filename){
		//文件打开 	
		ifstream t;  
		t.open(filename, std::ios::binary);
		//转换开始
		Reader reader;
		Value mapData;
		if (reader.parse(t, mapData, false)) {
			int length = mapData.size();
			for (int i = 1; i <= length; i++)//逐步解构数据 
			{
				Value tempObj = mapData[i-1];
				obj temp;
				string tempStr;

				tempStr = tempObj["type"].asString();
				temp.type = tempStr;

				tempStr = tempObj["begin_time"].asString();
				temp.beginTime.load_timestamp(tempStr);

				tempStr = tempObj["end_time"].asString();
				temp.endTime.load_timestamp(tempStr);

				tempStr = tempObj["begin_locate"].asString();
				temp.beginLocate.load_locate(tempStr);

				tempStr = tempObj["end_locate"].asString();
				temp.endLocate.load_locate(tempStr);

				tempStr = tempObj["rotate"].asString();
				temp.rotate = atoi(tempStr.c_str());

				this->objs[i] = temp;
			}
			this->length = length;
			return *this;
		}
		}
};
