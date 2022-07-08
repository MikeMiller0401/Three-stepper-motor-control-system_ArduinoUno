//x轴：LOW右移，HIGH左移
//y轴：LOW后退，HIGH前进
//Z轴：LOW下降，HIGH上升
#include <math.h>
int PULL = 3;//x方向
int DIRL = 2;
int PULR = 7;//y方向
int DIRR = 6;
int PULZ = 5;//z方向
int DIRZ = 4;
int pinRelay = 11;
static float distance = 20;//比例 = 实际距离/像素距离
float xl;
float xr;
unsigned long bushu_l;
unsigned long bushu_r;
double points_source[6] = {82, 85, 152.0, 98.0, 223, 76}; //输入的数组
unsigned long TIME;
int time_l = 25; //左 半脉冲
int time_r;//右 半脉冲
//左脉冲必须小于等于右脉冲
unsigned long t = 0;
boolean status = false;

void setup() {
  pinMode(DIRL, OUTPUT);
  pinMode(DIRR, OUTPUT);
  pinMode(PULL, OUTPUT);
  pinMode(PULR, OUTPUT);
  pinMode(PULZ, OUTPUT);
  pinMode(DIRZ, OUTPUT);
  pinMode(pinRelay, OUTPUT);
  Serial.begin(19200);

}
//跳跃函数，根据需要调用
void jump() {

  for (int i = 0; i < 1280; i++)
  {
    digitalWrite(DIRL, LOW); //方向
    digitalWrite(PULL, HIGH);
    delayMicroseconds(50);
    digitalWrite(PULL, LOW);
    delayMicroseconds(50);
  }
}

//z轴函数，根据数据调用
void z() {
  for (int i = 0; i < 1280; i++)
  {
    digitalWrite(DIRZ, LOW); //方向
    digitalWrite(PULZ, HIGH);
    delayMicroseconds(50);
    digitalWrite(PULZ, LOW);
    delayMicroseconds(50);
  }
}

//回撤函数
void back(float factor, double y1, double y3, double x1, double x3) {
  //对像素距离进行处理：
  double deltapix_y;
  double deltapix_x;
  if (y1 > y3) {
    deltapix_y = y1 - y3;
  } else {
    deltapix_y = y3 - y1;
  }
  if (x1 > x3) {
    deltapix_x = x1 - x3;
  } else {
    deltapix_x = x3 - x1;
  }
  double deltastep_y = deltapix_y * factor * 1280;
  double deltastep_x = deltapix_x * factor * 1280;

  double x = deltapix_x;
  double y = factor;
  double delta_x = x * y;
  Serial.println(x);
  Serial.println(y);
  Serial.println(delta_x);



  Serial.print("y轴退回:6400");
  Serial.println();


  for (int i = 0; i < 6400; i++)
  {

    digitalWrite(DIRR, LOW); //方向
    digitalWrite(PULR, HIGH);
    delayMicroseconds(50);
    digitalWrite(PULR, LOW);
    delayMicroseconds(50);
  }

  Serial.print("x轴退回:");
  Serial.print(deltastep_x - 500);
  Serial.println();


  for (int i = 0; i < deltastep_x - 500; i++)
  {

    digitalWrite(DIRL, HIGH); //方向
    digitalWrite(PULL, HIGH);
    delayMicroseconds(50);
    digitalWrite(PULL, LOW);
    delayMicroseconds(50);
  }
  Serial.print("y轴前进:");
  Serial.print(6400 + deltastep_y);
  Serial.println();

  for (int i = 0; i < (6400 + deltastep_y - 200); i++)
  {

    digitalWrite(DIRR, HIGH); //方向
    digitalWrite(PULR, HIGH);
    delayMicroseconds(50);
    digitalWrite(PULR, LOW);
    delayMicroseconds(50);
  }
  Serial.println();

}

//openMV通讯函数
double *openmv() {

  static double points_source[6];
  String str1, str2, str3;
  delay(4000);
  Serial.write("Hello world");//唤醒openmv
  int n = 0;
  do //数据写入
  {
    do {
      delay(2);
    }
    while (!(Serial.available() > 0));
    str1 = Serial.readString();
    double f = str1.toDouble();
    points_source[n] = f;
    Serial.println(points_source[n]);
    n++;
    Serial.println(n);
    for (int i = 0; i < n; i++)
    {
      Serial.print(points_source[i]);
      Serial.print(" ");
    }
    Serial.print("\n");
    while (Serial.read() >= 0) {}
  }
  while (n < 6);
  Serial.println("End of data recording.");
  while (Serial.read() >= 0) {}; //clear serialbuffer
  return points_source;

}

//求比例系数
double scalenumber(double x1, double x3) {
  double factor = distance / (x3 - x1);
  Serial.print("比例系数 = ");
  Serial.println(factor);
  return factor;
}

//位置初始化
void initialization() {
  delay(5000);
  Serial.println("y轴初始化");

  for (int j = 0; j < 9; ++j) {
    for (int i = 0; i < 3200; i++) //运行1280步（转角）
    {
      digitalWrite(DIRR, HIGH);//dir低电平正转
      digitalWrite(PULR, HIGH);//高电平
      delayMicroseconds(50);//暂停50微秒(转速)
      digitalWrite(PULR, LOW);
      delayMicroseconds(50);
    }
  }
  delay(1000);
  Serial.println("x轴初始化");
  for (int j = 0; j < 24; ++j) {
    for (int i = 0; i < 3200; i++) //运行1280步（转角）
    {
      digitalWrite(DIRL, HIGH);//dir低电平正转
      digitalWrite(PULL, HIGH);//高电平
      delayMicroseconds(50);//暂停50微秒(转速)
      digitalWrite(PULL, LOW);
      delayMicroseconds(50);
    }
  }
  delay(1000);
  Serial.println("z轴初始化");
  for (int j = 0; j < 17; ++j) {
    for (int i = 0; i < 3200; i++) //运行1280步（转角）
    {
      digitalWrite(DIRZ, LOW);//dir低电平正转
      digitalWrite(PULZ, HIGH);//高电平
      delayMicroseconds(50);//暂停50微秒(转速)
      digitalWrite(PULZ, LOW);
      delayMicroseconds(50);
    }
  }
  delay(1000);
  for (int j = 0; j < 17; ++j) {
    for (int i = 0; i < 3200; i++) //运行1280步（转角）
    {
      digitalWrite(DIRZ, LOW);//dir低电平正转
      digitalWrite(PULZ, HIGH);//高电平
      delayMicroseconds(50);//暂停50微秒(转速)
      digitalWrite(PULZ, LOW);
      delayMicroseconds(50);
    }
  }
}

//找基准点
void referencePoint(float factor, double x1, double y1) {
  long delta_x = 1280 * factor * (160 - x1);
  long delta_y = 1280 * factor * (200 - y1);
  Serial.print("基准点delta_x = ");
  Serial.println(delta_x);
  for (int i = 0; i < delta_x; i++) //运行1280步（转角）
  {
    digitalWrite(DIRL, HIGH);//dir低电平正转
    digitalWrite(PULL, HIGH);//高电平
    delayMicroseconds(50);//暂停50微秒(转速)
    digitalWrite(PULL, LOW);
    delayMicroseconds(50);
  }
  Serial.print("基准点delta_y = ");
  Serial.println(delta_y);

  for (int i = 0; i < delta_y; i++) //运行1280步（转角）
  {
    digitalWrite(DIRR, HIGH);//dir低电平正转
    digitalWrite(PULR, HIGH);//高电平
    delayMicroseconds(50);//暂停50微秒(转速)
    digitalWrite(PULR, LOW);
    delayMicroseconds(50);
  }
}



void finalfunction() {

  delay(1000);
  for (int j = 0; j < 5; ++j) {
    for (int i = 0; i < 3200; i++) //运行1280步（转角）
    {
      digitalWrite(DIRR, LOW);//dir低电平正转
      digitalWrite(PULR, HIGH);//高电平
      delayMicroseconds(50);//暂停50微秒(转速)
      digitalWrite(PULR, LOW);
      delayMicroseconds(50);
    }
  }
  delay(1000);
  for (int j = 0; j < 12; ++j) {
    for (int i = 0; i < 3200; i++) //运行1280步（转角）
    {
      digitalWrite(DIRL, LOW);//dir低电平正转
      digitalWrite(PULL, HIGH);//高电平
      delayMicroseconds(50);//暂停50微秒(转速)
      digitalWrite(PULL, LOW);
      delayMicroseconds(50);
    }
  }
  delay(1000);
  for (int j = 0; j < 34; ++j) {
    for (int i = 0; i < 3200; i++) //运行1280步（转角）
    {
      digitalWrite(DIRZ, HIGH);//dir低电平正转
      digitalWrite(PULZ, HIGH);//高电平
      delayMicroseconds(50);//暂停50微秒(转速)
      digitalWrite(PULZ, LOW);
      delayMicroseconds(50);
    }
  }
}

//运动函数
void move(int time_l, int time_r, unsigned long bushu_l, unsigned long bushu_r)
{
  if (time_l <= time_r)
  {
    for (int i = 0; i < bushu_l; i++) {
      digitalWrite(PULL, HIGH);
      if (t % time_r == 0)
      {
        status = !status;
        if (status == true) {
          digitalWrite(PULR, HIGH);
        }
        else {
          digitalWrite(PULR, LOW);
        }
        delayMicroseconds(time_l);
        t = time_l;
      }
      else if ((t < time_r) && (time_r < t + time_l)) {
        int time = time_r - t;
        delayMicroseconds(time);
        status = !status;
        if (status == true) {
          digitalWrite(PULR, HIGH);
        }
        else {
          digitalWrite(PULR, LOW);
        }
        time = time_l - time;
        delayMicroseconds(time);
        t = time;
      }
      else {
        t = t + time_l;
        delayMicroseconds(time_l);
      }
      digitalWrite(PULL, LOW);
      if (t % time_r == 0) {
        status = !status;
        if (status == true) {
          digitalWrite(PULR, HIGH);
        }
        else {
          digitalWrite(PULR, LOW);
        }
        delayMicroseconds(time_l);
        t = time_l;
      }
      else if ((t < time_r) && (time_r < t + time_l)) {
        int time = time_r - t;
        delayMicroseconds(time);
        status = !status;
        if (status == true) {
          digitalWrite(PULR, HIGH);
        }
        else {
          digitalWrite(PULR, LOW);
        }
        time = time_l - time;
        delayMicroseconds(time);
        t = time;
      }
      else {
        t = t + time_l;
        delayMicroseconds(time_l);
      }
    }
  }
  else {
    Serial.print("erro date");
    Serial.println();
  }

};

void loop()
{ //初始化位置
  initialization();

  double *p;
  p = openmv();
  double points_source[10];
  for (int i = 0; i < 6; i++) {
    points_source[i] = *(p + i);
  }

  double x1 = points_source[0];
  double y1 = points_source[1];
  double x2 = points_source[2];
  double y2 = points_source[3];
  double x3 = points_source[4];
  double y3 = points_source[5];
  double dx1 = x1;
  double dx2 = x2;
  double dx3 = x3;
  double dy1 = y1;
  double dy2 = y2;
  double dy3 = y3;
  //计算中间参数
  double a = x1 - x2;
  double b = y1 - y2;
  double c = x1 - x3;
  double d = y1 - y3;
  double e = ((x1 * x1 - x2 * x2) + (y1 * y1 - y2 * y2)) / 2.0;
  double f = ((x1 * x1 - x3 * x3) + (y1 * y1 - y3 * y3)) / 2.0;
  double g = b * c - a * d;
  //计算圆心坐标及半径
  double x0 = -(d * e - b * f) / g;
  double y0 = -(a * f - c * e) / g;
  double r = sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
  //补充坐标点
  double x4 = (x1 + x2) / 2;
  double x5 = (x2 + x3) / 2;
  double y4 = fabs(y0 + sqrt(r * r - (x4 - x0) * (x4 - x0)));
  double y5 = fabs(y0 + sqrt(r * r - (x5 - x0) * (x5 - x0)));
  double x6 = (x1 + x4) / 2;
  double x7 = (x4 + x2) / 2;
  double x8 = (x2 + x5) / 2;
  double x9 = (x5 + x3) / 2;
  double y6 = fabs(y0 + sqrt(r * r - (x6 - x0) * (x6 - x0)));
  double y7 = fabs(y0 + sqrt(r * r - (x7 - x0) * (x7 - x0)));
  double y8 = fabs(y0 + sqrt(r * r - (x8 - x0) * (x8 - x0)));
  double y9 = fabs(y0 + sqrt(r * r - (x9 - x0) * (x9 - x0)));
  double x_cha[8] = {x1 - x6, x6 - x4, x4 - x7, x7 - x2, x2 - x8, x8 - x5, x5 - x9, x9 - x3};
  double y_cha[8] = {y1 - y6, y6 - y4, y4 - y7, y7 - y2, y2 - y8, y8 - y5, y5 - y9, y9 - y3};

  double factor = scalenumber(x1, x3);

  referencePoint(factor, x1, y1);
  digitalWrite(pinRelay, LOW);
  double tuidao = ((y2 - y1) / 2) * factor * 1280;
  Serial.println(tuidao);
  for (int k = 0; k < 3; k = k + 1) {
    delay(500);
    Serial.println();
    Serial.print("k = ");
    Serial.println(k);
    Serial.println();
    /*
      if (k == 0) {
      Serial.println("back:");
      Serial.println(tuidao);
      for (int i = 0; i < tuidao; i++) //运行1280步（转角）
      {
        digitalWrite(DIRR, LOW);//dir低电平正转
        digitalWrite(PULR, HIGH);//高电平
        delayMicroseconds(50);//暂停50微秒(转速)
        digitalWrite(PULR, LOW);
        delayMicroseconds(50);
      }

      }
      if (k == 1) {
      Serial.println("back:");
      Serial.println(tuidao);
      for (int i = 0; i < tuidao; i++) //运行1280步（转角）
      {
        digitalWrite(DIRR, LOW);//dir低电平正转
        digitalWrite(PULR, HIGH);//高电平
        delayMicroseconds(50);//暂停50微秒(转速)
        digitalWrite(PULR, LOW);
        delayMicroseconds(50);
      }

      }*/
    for (int p = 0; p < 8; p++)
    {
      xl = x_cha[p];
      xr = y_cha[p];
      Serial.print(xl);
      Serial.println();
      Serial.print(xr);
      Serial.println();
      if (xl >= 0) {
        digitalWrite(DIRL, HIGH);//左正向
        if (xr >= 0) {
          digitalWrite(DIRR, HIGH);//右正向
          Serial.print("左正向");
          Serial.print("右正向");
          Serial.println();
        } else {
          xr = xr * (-1);
          digitalWrite(DIRR, LOW);//右反向
          Serial.print("左正向");
          Serial.print("右反向");
          Serial.println();
        }
      }

      else
      {
        xl = xl * (-1);
        digitalWrite(DIRL, LOW);//左反向
        if (xr >= 0) {
          digitalWrite(DIRR, HIGH);//右正向
          Serial.print("左反向");
          Serial.print("右正向");
          Serial.println();
        } else {
          xr = xr * (-1);
          digitalWrite(DIRR, LOW);
          Serial.print("左反向");
          Serial.print("右反向");
          Serial.println();
        }
      }

      Serial.print("xr：");
      Serial.print(xr);
      Serial.println();

      //根据实际比例将xl和xr进行放大或缩小（xlxr单位为pix，而32细分下1280步运动1mm）

      xl = xl * factor;//将像素距离换算为实际距离
      xr = xr * factor;
      //将实际距离换算为步数
      bushu_l = xl * 1280;
      TIME = bushu_l * 50;
      bushu_r = xr * 1280;
      time_r = (TIME / bushu_r) / 2;
      Serial.print("左侧步数：");
      Serial.print(bushu_l);
      Serial.println();
      Serial.print("右侧步数：");
      Serial.print(bushu_r);
      Serial.println();
      Serial.print("脉冲时间(微秒)：");
      Serial.print(TIME);
      Serial.println();
      Serial.print("time_r：");
      Serial.print(time_r);
      Serial.println();
      Serial.println();
      move(time_l, time_r, bushu_l, bushu_r);
      delay(1000);
    }
    if (k == 2) {
      break;
    } else {
      back(factor, dy1, dy3, dx1, dx3);
    }
  }
  finalfunction();

  digitalWrite(pinRelay, HIGH);
  while (1);
}
