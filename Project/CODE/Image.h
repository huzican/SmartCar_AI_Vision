#ifndef _IMAGE_H
#define _IMAGE_H

#include "common.h"
#include "SEEKFREE_MT9V03X_CSI.h"

#define START_ROW   65     //ɨ����ʼ��
#define M_INIT  (MT9V03X_CSI_W/2 - 1)
#define L_INIT  0
#define R_INIT  (MT9V03X_CSI_W - 1)
#define TRACK_WIDTH          42
#define WORLD_H              80          //������͸�Ӻ�ͼ��߶�(cm)
#define WORLD_W              160          //������͸�Ӻ�ͼ����(cm)
#define FORESIGHT_FORK      55
#define FORESIGHT_COMMON    70
#define FORESIGHT_CROSS     70
#define FORESIGHT_CIRCLE    75   
#define FORESIGHT_CIRCLE_IN 40
#define FORESIGHT_CIRCLE_BEFORE 85


#define PI  3.14159265358979f




typedef enum    //Ԫ������ö��
{
  COMMON=0, //ֱ��
  BENT=1,     //���
  CROSS=2,    //ʮ��·��
  RAMP=3,     //�µ�
  FORK=4,     //����
  CIRCLE=5,   //Բ��
  GARAGE=6,   //����
}elem_type_e;

typedef enum    //����״̬ö��
{
  NOCICLE=0,      //�޻���
  LEFT=1,         //��һ��·��
  LEFT_BEFORE=2,  //��һ����ڴ����ս���ʧ
  LEFT_IN=3,      //����·��
  LCIRCLE_IN=4,   //������
  LEFT_OUT=5,     //�ճ�·��
  LEFT_END=6,     //������
  RIGHT=7,         //��һ��·��
  RIGHT_BEFORE=8,  //��һ����ڴ����ս���ʧ
  RIGHT_IN=9,      //����·��
  RCIRCLE_IN=10,   //������
  RIGHT_OUT=11,     //�ճ�·��
  RIGHT_END=12,     //������
}circle_status_e;

typedef enum    //ʮ��״̬ö��
{
  NOCROSS=0,      //��ʮ�֣��Ϲյ���ʧ
  CROSS_BEFORE=1, //ʮ��ǰ����������һ���¹յ�
  CROSS_IN=2,     //ʮ���У�ֻ�����ζ���
//  CROSS_END,    //ʮ�ֳ��������Ϲյ�
}cross_status_e;

typedef enum
{
  NOFORK=0,   //������
  FORK_BEFORE=1,  //������
  JUDGE_NUMBER=2,//�жϵ�����
  FORK_IN=3,      //������
  FORK_END=4,     //������
//  FORK_RIGHT_BEFORE,
//  FORK_RIGHT_IN,
//  FORK_RIGHT_END,
}fork_status_e;

typedef enum
{
  NO_GARAGE=0,
  ZABRA = 1,//��⵽������
  STOP_CAR=2,//ͣ��
  OUT = 3,//����
}garage_status_e;

typedef enum
{
  NO_APRILTAG=0,//��Apriltag
  APRILTAG=1,   //ʶ��Apriltag����¼Apriltag����ֵ������ͣ��������AprilTag��ֵѡ������ͷ��ת����̨Ѱ�ҿ�
  GRAY_FIND_REC=2,//��߻����ұ��ҿ�״̬
  IDENTIFY=3,     //ʶ�����ͼƬ
  FRUIT=4,     //ʶ��ˮ����׼�����
  ANIMAL=5,      //ʶ�𵽶����ʱ3sͣ��
}apriltag_status_e;

typedef enum
{
  IS_ANIMAL = 0,
  IS_FRUIT = 1,
}target_type_e;

typedef struct
{
  char apriltag_dir; //Apriltagָʾ��е�����
  target_type_e taget_type;//�������Ƕ�����ͣ����ˮ������
  float target_error;
}target_info_s;
extern target_info_s target_info;

typedef struct  //������ʶ
{
  elem_type_e elem_type;    //Ԫ������
  circle_status_e circle_e;
  cross_status_e  cross_e;
  fork_status_e fork_e;
  garage_status_e garage_e;
  apriltag_status_e apriltag_e;
  char fork_dir;
  char cross_dir;//ʮ�ַ���������
  char garage_dir;//���ⷽ��
  int8 zebra_flag;      //�����߱�־,ÿ��ͼ����̬�жϣ���ֹ����ǰ ������ʮ������
  char apriltag_dir;    //��е�����
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
//  float first_deriva[MT9V03X_CSI_H];    //б��
//  float first_deriva_true[WORLD_H];     //��ʵб��
  
  int16 breakpoint;         //�յ�
  float breakpoint_dis;     //�յ���복ʵ�ʾ���
  
  int16 breakpoint_up;      //�Ϲյ�
  float breakpoint_up_dis;  //�Ϲյ���복��ʵ�ʾ���
  
  int16 lost_head[2];       //����ͷ��
  int16 lost_tail[2];       //����β��
  int16 lost_count;         //�����������
  int16 lost_total;         //���ж��������ܶ�������
}boder_status;

typedef struct  
{
  float pitch;  
  float Height;
  float Fx;  // focal length ����
  float Fy;
  float Cx;// optical center ��ѧ����
  float Cy; 
  float ZeroAngle;     //���Ӵ�ֱʱ��������(��)
  float FaceAngle;     //����ͷ��ͷ������ӵļн�(��)
  float InterLength;   //���ᵽ����ͷ����Ӵ���ľ��루cm��
  float AxasLength;    //����ͷ�����ӵľ���(cm)
  float Wheelradius;   //���Ӱ뾶(cm)
  float c1;
  float s1;
//  float vpx;
  float vpy;
  int16 ENDdistance;
}camera_status;
extern camera_status camera;

extern boder_status left_boder;
extern boder_status right_boder;

extern uint8 GammaTable[256];//�ñ�
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

extern uint8 image[MT9V03X_CSI_H][MT9V03X_CSI_W];  //����֮���ͼ��
extern int16 xy_image[WORLD_H][WORLD_W]; 

extern int16 Lline[MT9V03X_CSI_H];     //����
extern int16 Rline[MT9V03X_CSI_H];    //����
extern int16 Mline[MT9V03X_CSI_H];      //����
extern float AvgGray[MT9V03X_CSI_H];   //ͼ��ÿ��ƽ���Ҷ�

extern int16 start_point[MT9V03X_CSI_H];   //ɨ����ʼ��
extern int16 last_start_point;             //��һ��
extern uint8 end_line;         //ɨ����ֹ��
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
void Arc_matchlineLY(int16 col1, int16 row1, int16 col2, int16 row2, int16* line, char d);//����Բ������(ԭ����ΪY��);
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
