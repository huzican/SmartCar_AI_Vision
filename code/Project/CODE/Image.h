#ifndef _IMAGE_H
#define _IMAGE_H

#include "common.h"
#include "SEEKFREE_MT9V03X_CSI.h"

#define START_ROW   65     //扫线起始行
#define M_INIT  (MT9V03X_CSI_W/2 - 1)
#define L_INIT  0
#define R_INIT  (MT9V03X_CSI_W - 1)
#define TRACK_WIDTH          42
#define WORLD_H              80          //定义逆透视后图像高度(cm)
#define WORLD_W              160          //定义逆透视后图像宽度(cm)
#define FORESIGHT_FORK      55
#define FORESIGHT_COMMON    70
#define FORESIGHT_CROSS     70
#define FORESIGHT_CIRCLE    75   
#define FORESIGHT_CIRCLE_IN 40
#define FORESIGHT_CIRCLE_BEFORE 85


#define PI  3.14159265358979f




typedef enum    //元素种类枚举
{
  COMMON=0, //直道
  BENT=1,     //弯道
  CROSS=2,    //十字路口
  RAMP=3,     //坡道
  FORK=4,     //三岔
  CIRCLE=5,   //圆环
  GARAGE=6,   //车库
}elem_type_e;

typedef enum    //环岛状态枚举
{
  NOCICLE=0,      //无环形
  LEFT=1,         //第一个路口
  LEFT_BEFORE=2,  //第一个入口处，拐角消失
  LEFT_IN=3,      //拐入路口
  LCIRCLE_IN=4,   //环岛内
  LEFT_OUT=5,     //拐出路口
  LEFT_END=6,     //出环岛
  RIGHT=7,         //第一个路口
  RIGHT_BEFORE=8,  //第一个入口处，拐角消失
  RIGHT_IN=9,      //拐入路口
  RCIRCLE_IN=10,   //环岛内
  RIGHT_OUT=11,     //拐出路口
  RIGHT_END=12,     //出环岛
}circle_status_e;

typedef enum    //十字状态枚举
{
  NOCROSS=0,      //无十字，上拐点消失
  CROSS_BEFORE=1, //十字前，有两个或一个下拐点
  CROSS_IN=2,     //十字中，只有两段丢线
//  CROSS_END,    //十字出现两个上拐点
}cross_status_e;

typedef enum
{
  NOFORK=0,   //无三岔
  FORK_BEFORE=1,  //进三岔
  JUDGE_NUMBER=2,//判断到数字
  FORK_IN=3,      //三岔里
  FORK_END=4,     //出三岔
//  FORK_RIGHT_BEFORE,
//  FORK_RIGHT_IN,
//  FORK_RIGHT_END,
}fork_status_e;

typedef enum
{
  NO_GARAGE=0,
  ZABRA = 1,//检测到斑马线
  STOP_CAR=2,//停车
  OUT = 3,//出库
}garage_status_e;

typedef enum
{
  NO_APRILTAG=0,//无Apriltag
  APRILTAG=1,   //识别到Apriltag，记录Apriltag的数值，立马停车，根据AprilTag的值选择摄像头（转动云台寻找框）
  GRAY_FIND_REC=2,//左边或者右边找框状态
  IDENTIFY=3,     //识别框内图片
  FRUIT=4,     //识别到水果，准备打靶
  ANIMAL=5,      //识别到动物，计时3s停车
}apriltag_status_e;

typedef enum
{
  IS_ANIMAL = 0,
  IS_FRUIT = 1,
}target_type_e;

typedef struct
{
  char apriltag_dir; //Apriltag指示标靶的左右
  target_type_e taget_type;//靶子上是动物则停车，水果则打靶
  float target_error;
}target_info_s;
extern target_info_s target_info;

typedef struct  //赛道标识
{
  elem_type_e elem_type;    //元素类型
  circle_status_e circle_e;
  cross_status_e  cross_e;
  fork_status_e fork_e;
  garage_status_e garage_e;
  apriltag_status_e apriltag_e;
  char fork_dir;
  char cross_dir;//十字方向，左中右
  char garage_dir;//车库方向
  int8 zebra_flag;      //斑马线标志,每次图像处理动态判断，防止车库前 环岛和十字误判
  char apriltag_dir;    //标靶的左右
}pic_info_s;  
extern pic_info_s pic_info;

typedef struct
{
  float error;
  uint8 endline;
  uint8 show_flag;
//  pic_info_s pic_info;
}image_status;
extern image_status image_s;

typedef struct
{
//  float first_deriva[MT9V03X_CSI_H];    //斜率
//  float first_deriva_true[WORLD_H];     //真实斜率
  
  int16 breakpoint;         //拐点
  float breakpoint_dis;     //拐点距离车实际距离
  
  int16 breakpoint_up;      //上拐点
  float breakpoint_up_dis;  //上拐点距离车的实际距离
  
  int16 lost_head[2];       //丢线头部
  int16 lost_tail[2];       //丢线尾部
  int16 lost_count;         //丢线区域计数
  int16 lost_total;         //所有丢线区域总丢线行数
}boder_status;

typedef struct  
{
  float pitch;  
  float Height;
  float Fx;  // focal length 焦距
  float Fy;
  float Cx;// optical center 光学中心
  float Cy; 
  float ZeroAngle;     //杆子垂直时车身俯仰角(°)
  float FaceAngle;     //摄像头镜头面与杆子的夹角(°)
  float InterLength;   //轮轴到摄像头与杆子垂足的距离（cm）
  float AxasLength;    //摄像头到杆子的距离(cm)
  float Wheelradius;   //轮子半径(cm)
  float c1;
  float s1;
//  float vpx;
  float vpy;
  int16 ENDdistance;
}camera_status;
extern camera_status camera;

extern boder_status left_boder;
extern boder_status right_boder;

extern uint8 GammaTable[256];//γ表
extern float gama;
extern uint16 image_gain;
extern uint16 exposure_time;
extern uint16 image_LRoffset;
extern uint16 CONTRAST_THRESHOLD;
extern uint16 END_TH ;
extern uint16 TARGET_TIME;
extern uint16 ANIMAL_TIME;
extern uint16 RIGHT_REC_START; 
extern uint16 RIGHT_REC_END;   
extern uint16 LEFT_REC_START;  
extern uint16 LEFT_REC_END;   
extern uint16 FORK_STOP_DIS;
extern uint16 SEEK_NUM_SPEED;

extern uint16 CIR_SPEED1; 
extern uint16 CIR_SPEED2; 
extern uint16 CIR_SPEED3; 
extern uint16 CIR_SPEED4;
extern uint16 CIR_NUM;

extern uint8 image[MT9V03X_CSI_H][MT9V03X_CSI_W];  //矫正之后的图像
extern int16 xy_image[WORLD_H][WORLD_W]; 

extern int16 Lline[MT9V03X_CSI_H];     //左线
extern int16 Rline[MT9V03X_CSI_H];    //右线
extern int16 Mline[MT9V03X_CSI_H];      //中线
extern float AvgGray[MT9V03X_CSI_H];   //图像每行平均灰度

extern int16 start_point[MT9V03X_CSI_H];   //扫线起始点
extern int16 last_start_point;             //上一场
extern uint8 end_line;         //扫线终止行
extern int16 WLline[WORLD_H];
extern int16 WRline[WORLD_H];
extern int16 WMline[WORLD_H];
extern int8 lap_cnt;
extern int8 fork_num;
extern int8 cir_num;
extern int8 AprilTag;
extern uint16 RUN_DIR; 
extern uint16 GARAGE_ERR1; 
extern uint16 GARAGE_ERR2;
extern uint16 GARAGE_ERR3;
extern uint16 OUT_ERR;
extern int16 track_l[MT9V03X_CSI_H];
extern int16 track_r[MT9V03X_CSI_H];
extern int16 track_cross_l[MT9V03X_CSI_H];
extern int16 track_cross_r[MT9V03X_CSI_H];
extern uint16 FIND_REC_V; 
extern uint16 OUT_SPEED;

extern int a1,b1;

void sobel();
void BuildGamaTable(float coefficient);
void ImageGamaCorrect();
void edge();
void edge_show();
void image_show();
void image_hanldle();
void line_init();
//void first_derivative(boder_status *boder, int16 *line);
//void first_derivative_true(boder_status *boder, int16 *line);
void find_breakpoint(boder_status *boder, int16 *line, int16 end, char flag);
void find_lost_line(int16 *line, boder_status *boder, char flag);
void track_mark_show();
float slope_range(int16 start, int16 end, float *line);
int deviate_mid(char flag,int16 end);
void circle_judge_r(boder_status *l, boder_status *r, pic_info_s * pic);
void circle_judge_l(boder_status *l, boder_status *r, pic_info_s * pic);
void circle_handle_r(circle_status_e flag, boder_status *l, boder_status *r);
void circle_handle_l(circle_status_e flag, boder_status *l, boder_status *r);
void find_breakpoint_up(boder_status *boder, int16 *line,int16 end, char flag);
void track_again_huandao(int16 start,int16 end,char flag);
void track_again_CrossIn(int16 start, int16 end, char flag);
void track_again_fork(int16 start, int16 end, char flag);
void track_again_cross(int16 start,int16 end,char flag);
void matchline_pingyi(int16 start,int16 end,char flag);
void extend_track(uint8 start, uint8 end, int16* line);
void ExtendTheTrack_down(uint8 start, uint8 end, int16* line);
void Arc_matchlineLY(int16 col1, int16 row1, int16 col2, int16 row2, int16* line, char d);//两点圆弧连线(原点轴为Y轴);
void zebra_judge(boder_status *l, boder_status *r, pic_info_s * pic);
void line_pingyi(int16 start, int16 end,float times, char flag);
void true_pingyi(int16 start,int16 end,char flag);
void cross_judge(boder_status *l, boder_status *r, pic_info_s * pic);
void cross_handle(cross_status_e flag, char dir, boder_status *l, boder_status *r);
void fork_judge(boder_status *l, boder_status *r, pic_info_s * pic);
bool IsLimitedScope(int16 value, int16 low, int16 high);
bool FloatIsLimitedScope(float value, float low, float high);
bool huandao_judge_continuity(int8 start, int8 end, int16* line,char flag);
void fork_handle(fork_status_e flag, char dir, boder_status *l, boder_status *r);
void garage_judge(boder_status *l, boder_status *r, pic_info_s * pic);
void garage_handle(garage_status_e flag, boder_status *l, boder_status *r);
void garage_out(boder_status *l, boder_status *r, pic_info_s * pic);
void apritag_judge(boder_status *l, boder_status *r,pic_info_s * pic);
bool apriltag_tuzi_judge(int16 start, int16 end, char flag, int16 wid_th);
void track_again_tuzi(int16 start,int16 end,char flag, int16 *l_line, int16 *r_line);
bool AprilAoziAgain(int16 start, int16 end, char flag, int16 wid_th);
float GetTure_Ysite(uint8 y);
float GetTure_Xsite(float x,uint8 y);
int16 GetImage_Ysite(int16 Y);
int16 GetImage_Xsite(int16 X,int16 y);
void camera_para_init();
void matchline(int16 start,int16 startp,int16 end,int16 endp,int16 * line);
void promatchline(int16 start,int16 end,int16 matstart,int16 matend,int16 * line);
void world_image_show();
void GetWorldImage();
void GetWorldEdge();
void Memset(int16 *arr, int16 start, int16 end, int16 val);

#endif
