#include <stdlib.h>
#include "acllib.h"
#include <math.h>
#include <time.h>
#include <stdio.h>

#define RAD(x) ((x)/360.0*2*3.1415926535)

static struct tm *pSysTime = 0;
static int painted = 0;

void paintClock(int hour, int minute , int second)
{
	int x = 300, y = 120, r = 100*3/4;
	int sl = 85*3/4, ml = 75*3/4, hl = 60*3/4;
	int i;
	double alpha;
	char timestr[64] = {0};
	char *weekstr[]={"","一","二","三","四","五","六","日"}; 
	
	beginPaint();
	
	//systime time info
	sprintf(timestr,"%d年%02d月%02d日",1900 + pSysTime->tm_year,pSysTime->tm_mon + 1,pSysTime->tm_mday);
	paintText(getWidth()/2 - 8 * strlen(timestr) / 2, 2*8, timestr);
	sprintf(timestr,"%s%2d:%02d:%02d",pSysTime->tm_hour >= 12 ? "下午":"上午",pSysTime->tm_hour%12,pSysTime->tm_min,pSysTime->tm_sec);
	paintText(x - 6*8, y + r + 10, timestr); 	
	sprintf(timestr,"星期%s",weekstr[pSysTime->tm_wday]);
	paintText(x - 3*8, y + r + 30, timestr);
	
	//circle and lable
	setPenWidth(4);
	setPenColor(BLACK);
	setBrushColor(WHITE);
	ellipse(x-r,y-r,x+r,y+r);
	setPenWidth(2);
	for(i = 0; i < 12; i ++)
	{
		alpha = RAD( i*30);
		line(x + sl*sin(alpha), y - sl*cos(alpha), x + r*sin(alpha) , y - r*cos(alpha));
	}	
	setPenWidth(1);
	for(i = 0; i < 60; i++)
	{
		alpha = RAD(i*6);
		line(x + (r-5)*sin(alpha) , y - (r-5)*cos(alpha), x + r*sin(alpha) , y - r*cos(alpha));
	}
	
	//hour
	setPenWidth(4);
	alpha = RAD( ( hour + minute/60.0 )*(360/12) );
	line(x , y, x + hl*sin(alpha) , y - hl*cos(alpha));
	
	//minute
	setPenWidth(2);
	setPenColor(BLUE);
	alpha = RAD(minute*360/60);
	line(x , y, x + ml*sin(alpha) , y - ml*cos(alpha));
	
	//second
	setPenWidth(1);
	setPenColor(RED);
	alpha = RAD(second*360/60);
	line(x , y, x + sl*sin(alpha) , y - sl*cos(alpha));
	
	endPaint();
}

static int isLeapYear(int year)
{
	return (year % 4 == 0 && year % 100 != 0)||(year % 100 == 0);
}

static int getWeekday(int year , int month, int day)
{
	int w;
	int L , D;
	int mdays[2][13] = {{0,31,59,90,120,151,181,212,243,273,304,334,365},
	                    {0,31,30,91,121,152,182,213,244,274,305,335,366}};
	
	L = (year -1) / 4 - (year - 1) / 100 + (year -1) / 400 + 1;
	D = mdays[isLeapYear(year)][month-1] + day;
	
	w = (6 + year + L + D - 1) % 7;
	
	return w;
}
void paintCalendar(int year, int month , int day)
{
	char str[64] = {0};
	char str2[] = {"日  一  二  三  四  五  六"}; 
	int monthdays[2][12] = {{31,28,31,30,31,30,31,31,30,31,30,31},
	                    {31,29,31,30,31,30,31,31,30,31,30,31}};
	int i, j , w , x, y,str2_len;
	
	beginPaint();
	painted = 1;
	
	w = getWeekday(year, month, 1);
	
	//systime time info
	sprintf(str,"<       %4d年%02d月       >", year, month);
	x = getWidth()/4 - 4*strlen(str2) /2 - 20;
	
	y = 50;
	paintText(x, y, str);
	paintText(x, y + 20, str2);
	
	for(i = 0,y += 40; i < w; x +=26,i++)
	{
		paintText(x,y,"  ");
	}
	for(i = w, j = 1; j <= monthdays[isLeapYear(year)][month-1]; j++, i = (i + 1) % 7)
	{
		if(j == day)
		{
			setTextColor(RED);
		}
		else
		{
			setTextColor(BLACK);
		}
		
		sprintf(str,"%2d",j);
		paintText(x,y,str);
		if(i == 6)
		{
			x = getWidth() / 4 - (4*strlen(str2) + 40) / 2 ;
			y += 20;
		}
		else
		{
			x += 26;
		}
	}
	for(j = 0; j < 7; j++ ,i = (i + 1) % 7)
	{
		if(i == 0)
		{
			x = getWidth() / 4 - (4*strlen(str2) + 40) / 2 ;
			y += 20;
		}
		paintText(x,y,"  ");
		x += 26;
	}
	endPaint();
}
void mouseEvent(int x, int y, int button, int event)
{
	static int year = 0, month = 0, day = 0;
	
	if(year < 1900 && pSysTime)
	{
		year = pSysTime->tm_year + 1900;
		month = pSysTime->tm_mon + 1;
		day = pSysTime->tm_mday;
	}
	// last month
	if( y >= 50 && y < 70 && x >= 28 && x < 54 && event == BUTTON_DOWN && month > 0)
	{
		month--;
		if(month == 0) 
		{
			year--;
			month = 12;
		}
		paintCalendar(year,month,1);
	}
	// next month
	if(y >= 50 && y < 70 && x >= 28 + 6*27 && x < 28 + 7*27 && event == BUTTON_DOWN)
	{
		month++;
		if(month > 12)
		{
			year++;
			month = month % 12;
		}
		paintCalendar(year,month,1);
	}
	// today
	if( y >= 16 && y < 36 && x >= getWidth()/2 - 60 && x < getWidth()/2 + 60 && event == BUTTON_DOWN)
	{
		paintCalendar(pSysTime->tm_year + 1900, pSysTime->tm_mon + 1, pSysTime->tm_mday);
	}

}
void timerEvent()
{	
	time_t nowtime;
	time( &nowtime );
	pSysTime = localtime( &nowtime );

	paintClock(pSysTime->tm_hour%12, pSysTime->tm_min, pSysTime->tm_sec);
	if(!painted)
		paintCalendar(1900 + pSysTime->tm_year, pSysTime->tm_mon + 1, pSysTime->tm_mday);
}


int Setup()
{
	initWindow("Clock",400,200,400,260);
	initConsole();
	
	registerTimerEvent(timerEvent);
	registerMouseEvent(mouseEvent);
	 
	startTimer(0,1000);	
	
	return 0;
}

   


