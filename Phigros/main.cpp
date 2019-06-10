#include <windows.h>
#include <graphics.h>
#include <algorithm>
#include "translate.hpp"

#define MAXN 100010
#define X first
#define Y second

using namespace std;

const int W = 1003;
const int H = 602;

typedef pair<int, int> Dnt;

struct Line {
	Dnt S, T;//起始位置
	Dnt dJ, dA;//判定框与判定算法偏移量
	int st, et;//起始时间
	int judst, judet;//判定框起始时间
	bool jud_Del;//判定框是否消除	
	int State;
	PIMAGE img;//元素图像
}L[MAXN];

class Interface {
private:
	int pos_Play[4] = { 457,287,542,362 };//Play键位置
	int pos_Musi[4] = { 373,378,458,453 };//Misic键位置
	int pos_Clos[4] = { 545,378,630,453 };//Close键位置

	bool Play_Button, Musi_Button, Clos_Button;//控件是否按下
	bool Musi_On;//音乐是否播放
	bool Mouse_Up;//鼠标是否按下

	PIMAGE img[2];//图片指针
	MUSIC music;//音乐指针
	mouse_msg msg = { 0 };//鼠标状态指针

	bool is_Play(int x, int y) {//判断鼠标是否在Play键内部
		return x >= pos_Play[0] && x <= pos_Play[2] && y >= pos_Play[1] && y <= pos_Play[3];
	}
	bool is_Musi(int x, int y) {//判断鼠标是否在Music键内部
		return x >= pos_Musi[0] && x <= pos_Musi[2] && y >= pos_Musi[1] && y <= pos_Musi[3];
	}
	bool is_Clos(int x, int y) {//判断鼠标是否在Close键内部
		return x >= pos_Clos[0] && x <= pos_Clos[2] && y >= pos_Clos[1] && y <= pos_Clos[3];
	}

	void Init() {//初始化
		img[0] = newimage();  getimage(img[0], "img\\parface.png");
		img[1] = newimage();  getimage(img[1], "img\\button.png");
		music.OpenFile("music\\interface.mp3");
		//xyprintf(60, 60, "%d", (int)music.GetPlayStatus());
	}

	void Climate() {//刷新屏幕
		if (Play_Button)  Play_Button = 0;
		if (Musi_Button)  Musi_Button = 0;
		if (Clos_Button)  Clos_Button = 0;
		cleardevice();
		putimage(0, 0, img[0]);
	}

	void MouseEventSolve() {//处理鼠标事件
		for (; is_run(); delay_fps(100)) {//按帧处理鼠标事件
			int x, y;  mousepos(&x, &y);//抓取鼠标坐标
			while (mousemsg()) msg = getmouse();//抓取鼠标事件消息
			if (msg.is_up()) Mouse_Up = 1;
			if (keystate(81)) exit(0);
			if (keystate(82)) break;
			if (keystate(80)) {
				music.Pause();
				Musi_On = 0;
			}
			if (keystate(77)) {
				music.Play();
				Musi_On = 1;
			}
			if (is_Play(x, y)) {
				if (!Play_Button) {//处理Play键显示
					putimage_alphablend(NULL, img[1], pos_Play[0], pos_Play[1], 0x50, 0, 0, pos_Play[2] - pos_Play[0], pos_Play[3] - pos_Play[1]);
					Play_Button = 1;
				}
				if (msg.is_left()) {//处理Play键点击事件
					cleardevice();
					Play_Button = 0;
					music.Stop();
					return;
				}
			}
			else if (is_Musi(x, y)) {
				if (!Musi_Button) {//处理Music键显示
					putimage_alphablend(NULL, img[1], pos_Musi[0], pos_Musi[1], 0x50, 0, 0, pos_Musi[2] - pos_Musi[0], pos_Musi[3] - pos_Musi[1]);
					Musi_Button = 1;
				}
				if (msg.is_left() && msg.is_down() && Mouse_Up) {//处理Music键点击事件
					if (Musi_On) music.Pause();
					else music.Play();
					Musi_On = !Musi_On;
					Mouse_Up = 0;
				}
			}
			else if (is_Clos(x, y)) {
				if (!Clos_Button) {//处理Music键显示
					putimage_alphablend(NULL, img[1], pos_Clos[0], pos_Clos[1], 0x50, 0, 0, pos_Clos[2] - pos_Clos[0], pos_Clos[3] - pos_Clos[1]);
					Clos_Button = 1;
				}
				if (msg.is_left())  exit(0);
			}
			else Climate();//刷新屏幕
		}
	}
public:
	void Make_Interface() {
		Init();
		putimage(0, 0, img[0]);//输出主界面
		music.Play();//播放背景音乐
		MouseEventSolve();
	}
}Interface;

class Game_Play {
private:
	PIMAGE img[15];//图片指针
	PIMAGE bpg[15];
	MUSIC music;//音乐指针
	mouse_msg msg = { 0 };//鼠标状态指针
	obj_list objects[3];

	int Lnum = 0, score = 0, sum[4] = { 0 };
	Line q[MAXN];

	int Judge(int jx, int jy, int x, int y) {
		int R[3] = { 15,30,60 };//判定半径
		if (x >= jx - R[0] && x <= jx + R[0] && y >= jy - R[0] && y <= jy + R[0]) return 1;
		else if (x >= jx - R[1] && x <= jx + R[1] && y >= jy - R[1] && y <= jy + R[1]) return 2;
		else if (x >= jx - R[2] && x <= jx + R[2] && y >= jy - R[2] && y <= jy + R[2]) return 3;
		else return 0;
	}
	void putjudge(int x, int y) {
		int R[3] = { 15,30,60 };
		setcolor(YELLOW);
		rectangle(x - R[0], y - R[0], x + R[0], y + R[0]);
		rectangle(x - R[1], y - R[1], x + R[1], y + R[1]);
		rectangle(x - R[2], y - R[2], x + R[2], y + R[2]);
	}
	bool Check_End(int Time) {
		for (int i = 1; i <= Lnum; ++i)
			if (L[i].judet >= Time) return 0;
		return 1;
	}
	void Init() {
		img[0] = newimage();  getimage(img[0], "img\\judge.png");
		img[1] = newimage();  getimage(img[1], "img\\perfect.png");
		img[2] = newimage();  getimage(img[2], "img\\good.png");
		img[3] = newimage();  getimage(img[3], "img\\bad.png");
		img[4] = newimage();  getimage(img[4], "img\\miss.png");
		img[5] = newimage();  getimage(img[5], "img\\end.png");
		bpg[0] = newimage();  getimage(bpg[0], "img\\bgp1.png");
		bpg[1] = newimage();  getimage(bpg[1], "img\\bgp2.png");
		bpg[2] = newimage();  getimage(bpg[2], "img\\bgp3.png");
		bpg[3] = newimage();  getimage(bpg[3], "img\\bgp4.png");
		bpg[4] = newimage();  getimage(bpg[4], "img\\bgp5.png");
		bpg[5] = newimage();  getimage(bpg[5], "img\\bgp6.png");
		bpg[6] = newimage();  getimage(bpg[6], "img\\bgp7.png");
		bpg[7] = newimage();  getimage(bpg[7], "img\\bgp8.png");
		bpg[8] = newimage();  getimage(bpg[8], "img\\bgp9.png");
		music.OpenFile("music\\play.mp3");
		objects[0].load_map("json\\map1.json");
		objects[1].load_map("json\\map2.json");
		setfont(25, 0, "黑体");
	}
	void Load_Map1() {
		Lnum = objects[0].length;
		for (int i = 1; i <= objects[0].length; ++i)  {
			obj ret = objects[0].objs[i];
			L[i].S = make_pair(ret.beginLocate.x, ret.beginLocate.y);
			L[i].T = make_pair(ret.endLocate.x, ret.endLocate.y - 4);
			L[i].st = ret.beginTime.ms;
			L[i].et = ret.endTime.ms;
			L[i].dJ = make_pair(-35, -70);
			L[i].dA = make_pair(50, 8);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
			L[i].img = newimage(); getimage(L[i].img, "img\\line1.png");
		}
	}
	void Load_Map2() {
		Lnum = 35;
		for (int i = 1; i <= 7; ++i) {
			Dnt S = make_pair(0, 60 + (i - 1) * 20);
			Dnt T = make_pair(W / 2 - 8, S.Y);
			L[i].S = S;  L[i].st = (i - 1) * 300;
			L[i].T = T;  L[i].et = L[i].st + 500;
			L[i].dJ = make_pair(-77, -28);
			L[i].dA = make_pair(8, 50);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
			L[i].img = newimage(); getimage(L[i].img, "img\\line2.png");
		}
		for (int i = 8; i <= 15; ++i) {
			Dnt S = make_pair(W, 60 + (i - 1) * 20 + 100);
			Dnt T = make_pair(W / 2 - 8, S.Y);
			L[i].S = S;  L[i].st = (i - 1) * 300;
			L[i].T = T;  L[i].et = L[i].st + 500;
			L[i].dJ = make_pair(-77, -28);
			L[i].dA = make_pair(8, 50);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
			L[i].img = newimage(); getimage(L[i].img, "img\\line2.png");
		}
		for (int i = 16; i <= 35; ++i) {
			Dnt S = make_pair((i & 1) ? 0 : W, 60 + (i - 16) * 20);
			Dnt T = make_pair(W / 2 - 8, S.Y);
			L[i].S = S;  L[i].st = (i - 16) * 150 + L[15].st + 500;
			L[i].T = T;  L[i].et = L[i].st + 700;
			L[i].dJ = make_pair(-77, -28);
			L[i].dA = make_pair(8, 50);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
			L[i].img = newimage(); getimage(L[i].img, "img\\line3.png");
		}
	}
	void Load_Map3() {
		Lnum = 3;
		L[1].S = L[1].T = make_pair(W / 3 - 30, H / 2 - 8);
		L[2].S = L[2].T = make_pair(W * 2 / 3, H / 2 - 8);
		L[1].st = L[2].st = 0;
		L[1].et = L[2].et = 1000;
		L[1].dJ = L[2].dJ = make_pair(-35, -70);
		L[1].dA = L[2].dA = make_pair(50, 8);
		L[1].judst = 800;   L[1].judet = 1000;
		L[2].judst = 1000;  L[2].judet = 1200;
		L[1].jud_Del = L[2].jud_Del = 0;
		L[1].img = newimage(); getimage(L[1].img, "img\\line1.png");
		L[2].img = newimage(); getimage(L[2].img, "img\\line1.png");
		L[3].S = make_pair(0, H / 2 - 8);
		L[3].T = make_pair(0, H / 2 + 242);
		L[3].st = 1200;  L[3].et = 2000;  L[3].jud_Del = 1;
		L[3].judst = 2000;   L[3].judet = 2000;
		L[3].img = newimage(); getimage(L[3].img, "img\\line4.png");
	}
	void Load_Map4() {
		Lnum = 10;
		L[1].S = make_pair(260, H / 2 - 8);
		L[2].S = make_pair(W - 360, H / 2 - 8);
		L[3].S = make_pair(W / 2 - 50, H / 2 - 8);
		L[4].S = make_pair(390, H / 2 - 8);
		L[5].S = make_pair(W - 490, H / 2 - 8);
		L[6].S = make_pair(490, H / 2 - 8);
		L[7].S = make_pair(W - 390, H / 2 - 8);
		L[8].S = make_pair(290, H / 2 - 8);
		L[9].S = make_pair(W - 590, H / 2 - 8);
		L[10].S = make_pair(W / 2 - 50, H / 2 - 8);
		L[1].T = make_pair(20, H / 2 + 242);
		L[2].T = make_pair(W - 120, H / 2 + 242);
		L[3].T = make_pair(W / 2 - 50, H / 2 + 242);
		L[4].T = make_pair(150, H / 2 + 242);
		L[5].T = make_pair(W - 250, H / 2 + 242);
		L[6].T = make_pair(250, H / 2 + 242);
		L[7].T = make_pair(W - 150, H / 2 + 242);
		L[8].T = make_pair(50, H / 2 + 242);
		L[9].T = make_pair(W - 350, H / 2 + 242);
		L[10].T = make_pair(W / 2 - 50, H / 2 + 242);
		L[1].st = 0;     L[1].et = 2000;
		L[2].st = 2000;  L[2].et = 4000;
		L[3].st = 4000;  L[3].et = 6000;
		L[4].st = 6000;  L[4].et = 7000;
		L[5].st = 6000;  L[5].et = 7000;
		L[6].st = 7000;  L[6].et = 8000;
		L[7].st = 7000;  L[7].et = 8000;
		L[8].st = 8000;  L[8].et = 9000;
		L[9].st = 8000;  L[9].et = 9000;
		L[10].st = 9000;  L[10].et = 10000;
		L[1].dJ = L[2].dJ = L[3].dJ = L[4].dJ = L[5].dJ = L[6].dJ = L[7].dJ = L[8].dJ = L[9].dJ = L[10].dJ = make_pair(-35, -70);
		L[1].dA = L[2].dA = L[3].dA = L[4].dA = L[5].dA = L[6].dA = L[7].dA = L[8].dA = L[9].dA = L[10].dA = make_pair(50, 8);
		L[1].judst = 2000;  L[1].judet = 2200;
		L[2].judst = 4000;  L[2].judet = 4200;
		L[3].judst = 6000;  L[3].judet = 6200;
		L[4].judst = 7000;  L[4].judet = 7200;
		L[5].judst = 7000;  L[5].judet = 7200;
		L[6].judst = 8000;  L[6].judet = 8200;
		L[7].judst = 8000;  L[7].judet = 8200;
		L[8].judst = 9000;  L[8].judet = 9200;
		L[9].judst = 9000;  L[9].judet = 9200;
		L[10].judst = 10000;  L[10].judet = 10200;
		L[1].jud_Del = L[2].jud_Del = L[3].jud_Del = L[4].jud_Del = L[5].jud_Del = L[6].jud_Del = L[7].jud_Del = L[8].jud_Del = L[9].jud_Del = L[10].jud_Del = 0;
		L[1].img = newimage(); getimage(L[1].img, "img\\line1.png");
		L[2].img = newimage(); getimage(L[2].img, "img\\line1.png");
		L[3].img = newimage(); getimage(L[3].img, "img\\line1.png");
		L[4].img = newimage(); getimage(L[4].img, "img\\line1.png");
		L[5].img = newimage(); getimage(L[5].img, "img\\line1.png");
		L[6].img = newimage(); getimage(L[6].img, "img\\line1.png");
		L[7].img = newimage(); getimage(L[7].img, "img\\line1.png");
		L[8].img = newimage(); getimage(L[8].img, "img\\line1.png");
		L[9].img = newimage(); getimage(L[9].img, "img\\line1.png");
		L[10].img = newimage(); getimage(L[10].img, "img\\line1.png");

	}
	void Load_Map5() {
		Lnum = 12;
		L[1].S = make_pair(0, H / 2 + 242);
		L[1].T = make_pair(0, H / 2 - 8);
		L[1].st = 0;  L[1].et = 800;  L[1].jud_Del = 1;
		L[1].judst = 800;   L[1].judet = 800;
		L[2].S = make_pair(0, H / 2 - 8);
		L[2].T = make_pair(0, H / 2 - 250);
		L[2].st = 1300;  L[2].et = 2100;  L[2].jud_Del = 1;
		L[2].judst = 2100;   L[2].judet = 2100;
		L[3].S = make_pair(0, H / 2 - 8);
		L[3].T = make_pair(0, H / 2 - 8);
		L[3].st = 800;  L[3].et = 1300;  L[3].jud_Del = 1;
		L[3].judst = 1300;   L[3].judet = 1300;
		L[4].S = make_pair(0, H / 2 - 250);
		L[4].T = make_pair(0, H / 2 + 250);
		L[4].st = 2100;  L[4].et = 5100;  L[4].jud_Del = 1;
		L[4].judst = 5100;   L[4].judet = 5100;
		L[1].img = newimage(); getimage(L[1].img, "img\\line4.png");
		L[2].img = newimage(); getimage(L[2].img, "img\\line4.png");
		L[3].img = newimage(); getimage(L[3].img, "img\\line4.png");
		L[4].img = newimage(); getimage(L[4].img, "img\\line4.png");
		for (int i = 1; i <= 4; ++i) {
			L[i + 4].S = make_pair((i - 1) * 100 + 200, (i - 1) * 100 + 200);
			L[i + 4].T = make_pair((i - 1) * 100 + 200, (i - 1) * 100 + 200);
			L[i + 8].S = make_pair((i - 1) * 100 + 320, (i - 1) * 100 + 200);
			L[i + 8].T = make_pair((i - 1) * 100 + 320, (i - 1) * 100 + 200);
			L[i + 4].st = L[i + 8].st = 2100;
			L[i + 4].et = L[i + 8].et = 2100 + 1000 + (i - 1) * 600;
			L[i + 4].dJ = L[i + 8].dJ = make_pair(-35, -70);
			L[i + 4].dA = L[i + 8].dA = make_pair(50, 8);
			L[i + 4].judst = L[i + 4].et;
			L[i + 4].judet = L[i + 4].et + 200;
			L[i + 8].judst = L[i + 8].et;
			L[i + 8].judet = L[i + 8].et + 200;
			L[i + 4].jud_Del = L[i + 8].jud_Del = 0;
			L[i + 4].img = newimage(); getimage(L[i + 4].img, "img\\line1.png");
			L[i + 8].img = newimage(); getimage(L[i + 8].img, "img\\line1.png");
		}
	}
	void Load_Map6() {
		Lnum = objects[1].length;
		for (int i = 1; i <= objects[1].length; ++i)  {
			obj ret = objects[1].objs[i];
			L[i].S = make_pair(ret.beginLocate.x, ret.beginLocate.y);
			L[i].T = make_pair(ret.endLocate.x, ret.endLocate.y);
			L[i].st = ret.beginTime.ms;
			L[i].et = ret.endTime.ms;
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
			L[i].img = newimage(); 
			if (ret.type == "segment_h") {
				L[i].dJ = make_pair(-35, -70);
				L[i].dA = make_pair(50, 8);
				getimage(L[i].img, "img\\line1.png");
			}
			else if (ret.type == "segment_v") {
				L[i].dJ = make_pair(-77, -28);
				L[i].dA = make_pair(8, 50);
				getimage(L[i].img, "img\\line2.png");
			}
			else if (ret.type == "fold_line1") {
				L[i].dJ = make_pair(-85, -75);
				L[i].dA = make_pair(0, 3);
				getimage(L[i].img, "img\\line8.png");
			}
			else if (ret.type == "fold_line2") {
				L[i].S.X -= 50;  L[i].T.X -= 50;
				L[i].dJ = make_pair(-40, -70);
				L[i].dA = make_pair(45, 8);
				getimage(L[i].img, "img\\line5.png");
			}
			else if (ret.type == "fold_line3") {
				L[i].S.Y -= 50;  L[i].T.Y -= 50;
				L[i].dJ = make_pair(-85, -25);
				L[i].dA = make_pair(0, 53);
				getimage(L[i].img, "img\\line7.png");
			}
			else if (ret.type == "fold_line4") {
				L[i].S.X -= 50;  L[i].T.X -= 50;
				L[i].S.Y -= 50;  L[i].T.Y -= 50;
				L[i].dJ = make_pair(-40, -30);
				L[i].dA = make_pair(45, 48);
				getimage(L[i].img, "img\\line6.png");
			}
		}
	}
	void Load_Map7() {
		Lnum = 16;
		for (int i = 1; i <= 16; ++i) {
			L[i].st = (i - 1) * 500;
			L[i].et = L[i].st + 1000;
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
		}
		for (int i = 2; i <= 4; ++i) {
			L[i].S = make_pair(535, 252);
			L[i].T = make_pair(767, 252 + (i - 3) * 150);
			L[i].dJ = make_pair(-77, -28);
			L[i].dA = make_pair(8, 50);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].img = newimage(); getimage(L[i].img, "img\\line2.png");
		}
		for (int i = 6; i <= 8; ++i) {
			L[i].S = make_pair(450, 335);
			L[i].T = make_pair(450 + (7 - i) * 150, 545);
			L[i].dJ = make_pair(-35, -70);
			L[i].dA = make_pair(50, 8);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].img = newimage(); getimage(L[i].img, "img\\line1.png");
		}
		for (int i = 10; i <= 12; ++i) {
			L[i].S = make_pair(450, 252);
			L[i].T = make_pair(221, 252 + (11 - i) * 150);
			L[i].dJ = make_pair(-77, -28);
			L[i].dA = make_pair(8, 50);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].img = newimage(); getimage(L[i].img, "img\\line2.png");
		}
		for (int i = 14; i <= 16; ++i) {
			L[i].S = make_pair(450, 252);
			L[i].T = make_pair(450 + (i - 15) * 150, 46);
			L[i].dJ = make_pair(-35, -70);
			L[i].dA = make_pair(50, 8);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].img = newimage(); getimage(L[i].img, "img\\line1.png");
		}
		L[1].S = make_pair(500, 252);
		L[5].S = make_pair(500, 302);
		L[9].S = make_pair(451, 302);
		L[13].S = make_pair(451, 252);
		L[1].T = make_pair(730, 46);
		L[5].T = make_pair(728, 505);
		L[9].T = make_pair(229, 502);
		L[13].T = make_pair(229, 53);
		L[1].dJ = make_pair(-40, -70);
		L[1].dA = make_pair(45, 8);
		L[5].dJ = make_pair(-40, -30);
		L[5].dA = make_pair(45, 48);
		L[9].dJ = make_pair(-85, -25);
		L[9].dA = make_pair(0, 53);
		L[13].dJ = make_pair(-85, -75);
		L[13].dA = make_pair(0, 3);
		L[1].img = newimage(); getimage(L[1].img, "img\\line5.png");
		L[5].img = newimage(); getimage(L[5].img, "img\\line6.png");
		L[9].img = newimage(); getimage(L[9].img, "img\\line7.png");
		L[13].img = newimage(); getimage(L[13].img, "img\\line8.png");
	}
	void Load_Map8() {
		Lnum = 16;
		for (int l = 1, r = 16; l < r; ++l, --r) swap(L[l], L[r]);
		for (int i = 1; i <= 16; ++i) {
			L[i].st = (i - 1) * 300;
			L[i].et = L[i].st + 500;
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
		}
	}
	void Load_Map9() {
		Lnum = 24;
		for (int i = 1; i <= 12; ++i) {
			L[i].st = (i - 1) * 300;
			L[i].et = L[i].st + 300;
			L[i].dJ = make_pair(-35, -70);
			L[i].dA = make_pair(50, 8);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
			L[i].img = newimage(); getimage(L[i].img, "img\\line9.png");
		}
		for (int i = 1; i <= 3; ++i) {
			L[i].S = make_pair(40 + (i - 1) * 110, 0);
			L[i].T = make_pair(40 + (i - 1) * 110, 252);
		}
		for (int i = 4; i <= 6; ++i) {
			L[10 - i].S = make_pair(863 - (i - 4) * 110, 0);
			L[10 - i].T = make_pair(863 - (i - 4) * 110, 252);
		}
		for (int i = 7; i <= 9; ++i) {
			L[i].S = make_pair(40 + (i - 7) * 110, H);
			L[i].T = make_pair(40 + (i - 7) * 110, 332);
		}
		for (int i = 10; i <= 12; ++i) {
			L[22 - i].S = make_pair(863 - (i - 10) * 110, H);
			L[22 - i].T = make_pair(863 - (i - 10) * 110, 332);
		}
		for (int i = 13; i <= 24; ++i) {
			L[i].st = (i - 1) * 300;
			L[i].et = L[i].st + 300;
			L[i].dJ = make_pair(-77, -28);
			L[i].dA = make_pair(8, 50);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
			L[i].img = newimage(); getimage(L[i].img, "img\\line10.png");
		}
		for (int i = 13; i <= 15; ++i) {
			L[i].S = make_pair(0, 40 + (i - 13) * 70);
			L[i].T = make_pair(451, 40 + (i - 13) * 70);
		}
		for (int i = 16; i <= 18; ++i) {
			L[34 - i].S = make_pair(0, 462 - (i - 16) * 70);
			L[34 - i].T = make_pair(451, 462 - (i - 16) * 70);
		}
		for (int i = 19; i <= 21; ++i) {
			L[i].S = make_pair(W, 40 + (i - 19) * 70);
			L[i].T = make_pair(530, 40 + (i - 19) * 70);
		}
		for (int i = 22; i <= 24; ++i) {
			L[46 - i].S = make_pair(W, 462 - (i - 22) * 70);
			L[46 - i].T = make_pair(530, 462 - (i - 22) * 70);
		}
	}
	void Load_Map10() {
		Lnum = 10;
		for (int i = 1; i <= 10; ++i) {
			Dnt S = make_pair(343, rand() % (H - 100));
			Dnt T = make_pair(870, rand() % (H - 100));
			L[i].S = S;  L[i].st = (i - 1) * 300;
			L[i].T = T;  L[i].et = L[i].st + 1000;
			L[i].dJ = make_pair(-77, -28);
			L[i].dA = make_pair(8, 50);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
			L[i].img = newimage(); getimage(L[i].img, "img\\line10.png");
		}
	}
	void Load_Map11() {
		Lnum = 10;
		for (int i = 1; i <= 10; ++i) {
			Dnt S = make_pair(274, 40 + (i - 1) * 30);
			Dnt T = make_pair(770, 40 + (i - 1) * 30);
			L[i].S = S;  L[i].st = (i - 1) * 200;
			L[i].T = T;  L[i].et = L[i].st + 500;
			L[i].dJ = make_pair(-77, -28);
			L[i].dA = make_pair(8, 50);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
			L[i].img = newimage(); getimage(L[i].img, "img\\line3.png");
		}
	}
	void Load_Map12() {
		Lnum = 10;
		for (int i = 1; i <= 10; ++i) {
			Dnt S = make_pair(274, 462 - (i - 1) * 30);
			Dnt T = make_pair(770, 462 - (i - 1) * 30);
			L[i].S = S;  L[i].st = (i - 1) * 200;
			L[i].T = T;  L[i].et = L[i].st + 500;
			L[i].dJ = make_pair(-77, -28);
			L[i].dA = make_pair(8, 50);
			L[i].judst = L[i].et;
			L[i].judet = L[i].et + 200;
			L[i].jud_Del = 0;
			L[i].img = newimage(); getimage(L[i].img, "img\\line3.png");
		}
	}
	void Test(int CASE) {
		putimage(0, 0, bpg[CASE]);
		for (int i = 1; i <= Lnum; ++i) {
			Dnt posi = L[i].S;
			putimage_transparent(NULL, L[i].img, posi.X, posi.Y, BLACK);
			putimage_transparent(NULL, img[0], posi.X + L[i].dJ.X, posi.Y + L[i].dJ.Y, BLACK);
			putjudge(posi.X + L[i].dA.X, posi.Y + L[i].dA.Y);
		}
	}
	Dnt calcposi(Line Li, int t) {
		Dnt S = Li.S, T = Li.T;
		double rate = (double)(t - Li.st) / (double)(Li.et - Li.st);
		int x = (int)(S.X + rate * (T.X - S.X));
		int y = (int)(S.Y + rate * (T.Y - S.Y));
		return make_pair(x, y);
	}
	void Play(int CASE) {//动画制作
		bool Mouse_Up = 0; int Time = 0, head = 1, tail = 0;
		for (; is_run(); delay_fps(100), Time += 10) {
			int x, y;
			mousepos(&x, &y);//抓取鼠标坐标
			while (mousemsg()) msg = getmouse();//抓取鼠标事件消息
			if (msg.is_up())  Mouse_Up = 1;
			cleardevice();
			putimage(0, 0, bpg[CASE]);
			xyprintf(60, 60, "得分：%d", score);
			for (int i = 1; i <= Lnum; ++i) {
				if (L[i].st <= Time && L[i].et >= Time) {
					Dnt posi = calcposi(L[i], Time);
					putimage_transparent(NULL, L[i].img, posi.X, posi.Y, BLACK);
				}
				if (L[i].judst <= Time && L[i].judet >= Time && !L[i].jud_Del) {
					Dnt posi = L[i].T;
					putimage_transparent(NULL, img[0], posi.X + L[i].dJ.X, posi.Y + L[i].dJ.Y, BLACK);
					int State = Judge(posi.X + L[i].dA.X, posi.Y + L[i].dA.Y, x, y);
					putjudge(posi.X + L[i].dA.X, posi.Y + L[i].dA.Y);
					if (State && msg.is_left() && msg.is_down()) {
						Mouse_Up = 0;
						L[i].jud_Del = 1;
						++sum[State];  ++tail;
						q[tail].S = make_pair(700, 100);
						q[tail].T = make_pair(700, 70);
						q[tail].st = Time;
						q[tail].et = Time + 200;
						q[tail].State = State;
						if (State == 1) score += 100;
						else if (State == 2) score += 50;
						else if (State == 3) score += 10;
					}
				}
				if (L[i].judet < Time && !L[i].jud_Del) {
					L[i].jud_Del = 1;
					++sum[0];  ++tail;
					q[tail].S = make_pair(700, 100);
					q[tail].T = make_pair(700, 70);
					q[tail].st = Time;
					q[tail].et = Time + 200;
					q[tail].State = 4;
				}
				while (head <= tail && q[head].et < Time) ++head;
				for (int i = head; i <= tail; ++i) {
					Dnt posi = calcposi(q[head], Time);
					putimage_transparent(NULL, img[q[i].State], posi.X, posi.Y, BLACK);
				}
			}
			if (Check_End(Time)) break;
		}
	}
	void Report() {
		cleardevice();
		putimage(0, 0, bpg[0]);
		setcolor(YELLOW);
		setfont(35, 0, "黑体");
		xyprintf(W / 2 - 50, H / 2 - 200, "游戏结束");
		setfont(25, 0, "黑体");
		xyprintf(W / 2 - 150, H / 2 - 100, "Perfect:");
		xyprintf(W / 2 + 150, H / 2 - 100, "%d", sum[1]);
		xyprintf(W / 2 - 150, H / 2 - 50, "Good:");
		xyprintf(W / 2 + 150, H / 2 - 50, "%d", sum[2]);
		xyprintf(W / 2 - 150, H / 2 + 0, "Bad:");
		xyprintf(W / 2 + 150, H / 2 + 0, "%d", sum[3]);
		xyprintf(W / 2 - 150, H / 2 + 50, "Miss:");
		xyprintf(W / 2 + 150, H / 2 + 50, "%d", sum[0]);
		setfont(30, 0, "黑体");
		xyprintf(W / 2 - 150, H / 2 + 125, "总分:");
		xyprintf(W / 2 + 150, H / 2 + 125, "%d", score);
	}
public:
	void Start() {
		Init();
		music.Play();
		Load_Map1();  Play(1);  Sleep(1000);
		Load_Map2();  Play(2);  Sleep(1000);
		Load_Map3();  Play(1);
		Load_Map4();  Play(3);
		Load_Map5();  Play(0);  Sleep(1000);
		Load_Map6();  Play(4);  Sleep(500);
		Load_Map7();  Play(4);  Sleep(500);
		Load_Map8();  Play(4);  Sleep(2000);
		Load_Map9();  Play(5);  Sleep(1000);
		Load_Map10();  Play(6);  Sleep(1000);
		Load_Map11();  Play(7);  Sleep(1000);
		Load_Map12();  Play(8); Sleep(1000);
		Load_Map11();  Play(7);  Sleep(1000);
		Load_Map12();  Play(8); Sleep(1000);
		Report();
	}
}Game_Play;

int main() {
	initgraph(W, H);
	setcaption("Phigros");
	Interface.Make_Interface();
	Game_Play.Start();
	getch();
	closegraph();
	return 0;
}
