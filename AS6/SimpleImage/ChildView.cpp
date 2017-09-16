
//本框架最主要的文件

#include "stdafx.h"
#include "CImage.h"
#include "ChildView.h"
#include "InfoDlg.h"
#include <string>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*------------------------------------------------------------------------------------------------------------
1.本程序框架是基于MFC开发的显示及处理图像的应用程序框架，同学们可以在此框架的基础上完成作业所需的功能代码。
2.程序中处理图像的类是微软MFC基础类库提供的CImage类，该类提供了读取和显示各种格式图像的函数，可以大大简化处
理图像过程中的工作量，使大家把主要精力集中在作业所列的功能实现上。
3.每个作业对应着菜单上Tools下的一个菜单项，每个菜单项对应一个空白函数，用于实现每个作业的功能代码。
4.当你打开一幅图像以后，图像句柄被存储在CImage类型的imgOriginal对象中，这个CImage类的成员函数很强大，可以在MSDN
（按F1打开）中查询有关它的操作。imgOriginal对象是本程序中CChildView的私有变量，可以在以下的成员函数中被调用。
有关图像的一些基础操作都可用imgOriginal.函数名()的方法调用，比如打开一幅图像可以用imgOriginal.Load("x.jpg")，
更多有用的操作请在MSDN中搜索CImage。
5.下面的CChildView::OnToolsMakeBW是一个简单的例子，它是菜单上Tools下的第一个菜单项对应的消息响应函数，把彩色图像
转为灰度图。
------------------------------------------------------------------------------------------------------------*/
void CChildView::OnToolsMakeBW(void)
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标

	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		//把每个像素从彩色转为灰度
		COLORREF pixel;//保存一个像素的色彩信息
		int maxY = imgOriginal.GetHeight(), maxX = imgOriginal.GetWidth();//从imgOriginal取得图像的长maxX和高maxY
		byte r, g, b, avg;//r,g,b值和平均灰度值变量
		for (int x = 0; x < maxX; x++) //列循环
		{
			for (int y = 0; y < maxY; y++) //行循环
			{
				pixel = imgOriginal.GetPixel(x, y);//取得位于(x,y)位置上的像素的色彩信息
				r = GetRValue(pixel);//取得pixel的r值
				g = GetGValue(pixel);//取得pixel的g值
				b = GetBValue(pixel);//取得pixel的b值
				avg = ((r + g + b) / 3);//用此式将rgb彩色值转为灰度值
				imgOriginal.SetPixelRGB(x, y, avg, avg, avg);//将位于(x,y)位置上的像素的rgb色彩通道设为(avg,avg,avg)，即转换为灰度
			}
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		// the image uses an indexed palette, so we will just change the palette table entries to their B&W equivalents 
		int MaxColors = imgOriginal.GetMaxColorTableEntries();
		RGBQUAD* ColorTable;
		ColorTable = new RGBQUAD[MaxColors];

		imgOriginal.GetColorTable(0, MaxColors, ColorTable);
		for (int i = 0; i < MaxColors; i++)
		{
			int avg = (ColorTable[i].rgbBlue + ColorTable[i].rgbGreen + ColorTable[i].rgbRed) / 3;
			ColorTable[i].rgbBlue = (BYTE)avg;
			ColorTable[i].rgbGreen = (BYTE)avg;
			ColorTable[i].rgbRed = (BYTE)avg;
		}
		imgOriginal.SetColorTable(0, MaxColors, ColorTable);

		delete[] ColorTable;
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}

//为菜单项添加消息响应函数并添加实现代码（细节参加ppt）
void CChildView::OnToolsBinarization()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();

		byte** grayScale = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			grayScale[i] = (byte*)malloc(w*sizeof(byte));
		}


		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			byte r, g, b, avg;
			pixel = imgOriginal.GetPixel(j, i);
			r = GetRValue(pixel);
			g = GetGValue(pixel);
			b = GetBValue(pixel);
			avg = (r + g + b) / 3;
			grayScale[i][j] = avg;
		}


		int countTotal;
		countTotal = h*w;
		double maxBetween = 0;
		int resT;
		for (int T = 0; T < 255; T++){
			int  countFG = 0, countBG = 0;
			int sumTotal = 0, sumFG = 0, sumBG = 0;
			double avgTotal, avgFG, avgBG;
			for (int i = 0; i < h; i++)
			for (int j = 0; j < w; j++){
				sumTotal += grayScale[i][j];
				if (grayScale[i][j]>T){
					sumFG += grayScale[i][j];
					countFG++;
				}
				else{
					sumBG += grayScale[i][j];
					countBG++;
				}
			}

			avgBG = (countBG > 0) ? sumBG / countBG : 0;
			avgFG = (countFG > 0) ? sumFG / countFG : 0;
			avgTotal = sumTotal / countTotal;
			double between = (double)countBG / countTotal * (avgBG - avgTotal)*(avgBG - avgTotal) + (double)countFG / countTotal * (avgFG - avgTotal)*(avgFG - avgTotal);
			if (between > maxBetween){
				resT = T;
				maxBetween = between;
			}
		}
		int T = resT;

		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			if (grayScale[i][j]>T)
				grayScale[i][j] = 255;
			else
				grayScale[i][j] = 0;
		}

		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			imgOriginal.SetPixelRGB(j, i, grayScale[i][j], grayScale[i][j], grayScale[i][j]);
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsBinarizationLocal()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();

		byte** grayScale = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			grayScale[i] = (byte*)malloc(w*sizeof(byte));
		}


		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			byte r, g, b, avg;
			pixel = imgOriginal.GetPixel(j, i);
			r = GetRValue(pixel);
			g = GetGValue(pixel);
			b = GetBValue(pixel);
			avg = (r + g + b) / 3;
			grayScale[i][j] = avg;
		}

		/*把图像分成10*10的块，局部做二值化*/
		for (int ii = 0; ii < 10; ii++)
		for (int jj = 0; jj < 10; jj++)
		{
			int countTotal;
			countTotal = h*w / 100;
			double maxBetween = 0;
			int resT;
			for (int T = 0; T < 255; T++){
				int  countFG = 0, countBG = 0;
				int sumTotal = 0, sumFG = 0, sumBG = 0;
				double avgTotal, avgFG, avgBG;
				for (int i = ii*h / 10; i < (ii + 1)*h / 10; i++)
				for (int j = jj*w / 10; j < (jj + 1)*w / 10; j++){
					sumTotal += grayScale[i][j];
					if (grayScale[i][j]>T){
						sumFG += grayScale[i][j];
						countFG++;
					}
					else{
						sumBG += grayScale[i][j];
						countBG++;
					}
				}

				avgBG = (countBG > 0) ? sumBG / countBG : 0;
				avgFG = (countFG > 0) ? sumFG / countFG : 0;
				avgTotal = sumTotal / countTotal;
				double between = (double)countBG / countTotal * (avgBG - avgTotal)*(avgBG - avgTotal) + (double)countFG / countTotal * (avgFG - avgTotal)*(avgFG - avgTotal);
				if (between > maxBetween){
					resT = T;
					maxBetween = between;
				}
			}
			int T = resT;
			for (int i = ii*h / 10; i < (ii + 1)*h / 10; i++)
			for (int j = jj*w / 10; j < (jj + 1)*w / 10; j++){
				if (grayScale[i][j]>T)
					grayScale[i][j] = 255;
				else
					grayScale[i][j] = 0;
			}
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			imgOriginal.SetPixelRGB(j, i, grayScale[i][j], grayScale[i][j], grayScale[i][j]);
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsErosion()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();

		byte** grayScale = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			grayScale[i] = (byte*)malloc(w*sizeof(byte));
		}


		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			byte r, g, b, avg;
			pixel = imgOriginal.GetPixel(j, i);
			r = GetRValue(pixel);
			g = GetGValue(pixel);
			b = GetBValue(pixel);
			avg = (r + g + b) / 3;
			grayScale[i][j] = avg;
		}

		byte** erosion = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			erosion[i] = (byte*)malloc(w*sizeof(byte));
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			if (j - 1 < 0 && j + 1 >= w)
				erosion[i][j] = grayScale[i][j];
			else if (grayScale[i][j - 1] == 0 && grayScale[i][j] == 0 && grayScale[i][j + 1] == 0)
				erosion[i][j] = 0;
			else
				erosion[i][j] = 255;
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			imgOriginal.SetPixelRGB(j, i, erosion[i][j], erosion[i][j], erosion[i][j]);
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsDilation()
{

	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();

		byte** grayScale = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			grayScale[i] = (byte*)malloc(w*sizeof(byte));
		}


		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			byte r, g, b, avg;
			pixel = imgOriginal.GetPixel(j, i);
			r = GetRValue(pixel);
			g = GetGValue(pixel);
			b = GetBValue(pixel);
			avg = (r + g + b) / 3;
			grayScale[i][j] = avg;
		}

		byte** dilation = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			dilation[i] = (byte*)malloc(w*sizeof(byte));
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			if (j - 1 < 0 && j + 1 >= w)
				dilation[i][j] = grayScale[i][j];
			else if (grayScale[i][j - 1] == 0 || grayScale[i][j] == 0 || grayScale[i][j + 1] == 0)
				dilation[i][j] = 0;
			else
				dilation[i][j] = 255;
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			imgOriginal.SetPixelRGB(j, i, dilation[i][j], dilation[i][j], dilation[i][j]);
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsOpenning()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();

		byte** grayScale = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			grayScale[i] = (byte*)malloc(w*sizeof(byte));
		}


		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			byte r, g, b, avg;
			pixel = imgOriginal.GetPixel(j, i);
			r = GetRValue(pixel);
			g = GetGValue(pixel);
			b = GetBValue(pixel);
			avg = (r + g + b) / 3;
			grayScale[i][j] = avg;
		}

		byte** erosion = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			erosion[i] = (byte*)malloc(w*sizeof(byte));
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			if (j - 1 < 0 && j + 1 >= w)
				erosion[i][j] = grayScale[i][j];
			else if (grayScale[i][j - 1] == 0 && grayScale[i][j] == 0 && grayScale[i][j + 1] == 0)
				erosion[i][j] = 0;
			else
				erosion[i][j] = 255;
		}
		byte** openning = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			openning[i] = (byte*)malloc(w*sizeof(byte));
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			if (j - 1 < 0 && j + 1 >= w)
				openning[i][j] = erosion[i][j];
			else if (erosion[i][j - 1] == 0 || erosion[i][j] == 0 || erosion[i][j + 1] == 0)
				openning[i][j] = 0;
			else
				openning[i][j] = 255;
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			imgOriginal.SetPixelRGB(j, i, openning[i][j], openning[i][j], openning[i][j]);
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsClosing()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();

		byte** grayScale = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			grayScale[i] = (byte*)malloc(w*sizeof(byte));
		}


		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			byte r, g, b, avg;
			pixel = imgOriginal.GetPixel(j, i);
			r = GetRValue(pixel);
			g = GetGValue(pixel);
			b = GetBValue(pixel);
			avg = (r + g + b) / 3;
			grayScale[i][j] = avg;
		}

		byte** dilation = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			dilation[i] = (byte*)malloc(w*sizeof(byte));
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			if (j - 1 < 0 && j + 1 >= w)
				dilation[i][j] = grayScale[i][j];
			else if (grayScale[i][j - 1] == 0 || grayScale[i][j] == 0 || grayScale[i][j + 1] == 0)
				dilation[i][j] = 0;
			else
				dilation[i][j] = 255;
		}
		byte** closing = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			closing[i] = (byte*)malloc(w*sizeof(byte));
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			if (j - 1 < 0 && j + 1 >= w)
				closing[i][j] = dilation[i][j];
			else if (dilation[i][j - 1] == 0 && dilation[i][j] == 0 && dilation[i][j + 1] == 0)
				closing[i][j] = 0;
			else
				closing[i][j] = 255;
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			imgOriginal.SetPixelRGB(j, i, closing[i][j], closing[i][j], closing[i][j]);
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsHMT()
{
	// TODO: Add your command handler code here
	::AfxMessageBox("Hello World!");
}

void CChildView::OnToolsLog()
{
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		float max = 0;
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();

		float** Y = (float**)malloc(h*sizeof(float *));
		for (int i = 0; i < h; i++){
			Y[i] = (float*)malloc(w*sizeof(float));
		}
		float** U = (float**)malloc(h*sizeof(float *));
		for (int i = 0; i < h; i++){
			U[i] = (float*)malloc(w*sizeof(float));
		}
		float** V = (float**)malloc(h*sizeof(float *));
		for (int i = 0; i < h; i++){
			V[i] = (float*)malloc(w*sizeof(float));
		}


		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			byte r, g, b;
			pixel = imgOriginal.GetPixel(j, i);
			r = GetRValue(pixel);
			g = GetGValue(pixel);
			b = GetBValue(pixel);
			Y[i][j] = 0.3*r + 0.59*g + 0.11*b;
			U[i][j] = 0.493*(b - Y[i][j]);
			V[i][j] = 0.877*(r - Y[i][j]);
			if (Y[i][j]>max)
				max = Y[i][j];
		}

		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			Y[i][j] = 255 * log(Y[i][j] / 255 + 1) / (log(max / 255 + 1));
			double r = Y[i][j] + 1.14 * V[i][j];
			double b = Y[i][j] + 2.03 * U[i][j];
			double g = Y[i][j] - 0.39*U[i][j] - 0.58* V[i][j];
			r = r>255 ? 255 : r;
			g = g>255 ? 255 : g;
			b = b > 255 ? 255 : b;
			r = r < 0 ? 0 : r;
			g = g < 0 ? 0 : g;
			b = b < 0 ? 0 : b;

			imgOriginal.SetPixelRGB(j, i, r, g, b);

		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsEqualization()
{
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();

		float** Y = (float**)malloc(h*sizeof(float *));
		for (int i = 0; i < h; i++){
			Y[i] = (float*)malloc(w*sizeof(float));
		}
		float** U = (float**)malloc(h*sizeof(float *));
		for (int i = 0; i < h; i++){
			U[i] = (float*)malloc(w*sizeof(float));
		}
		float** V = (float**)malloc(h*sizeof(float *));
		for (int i = 0; i < h; i++){
			V[i] = (float*)malloc(w*sizeof(float));
		}
		int n[256];
		int tranform[256];
		for (int i = 0; i < 256; i++)
			n[i] = 0;


		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			byte r, g, b, avg;
			pixel = imgOriginal.GetPixel(j, i);
			r = GetRValue(pixel);
			g = GetGValue(pixel);
			b = GetBValue(pixel);
			Y[i][j] = 0.3*r + 0.59*g + 0.11*b;
			U[i][j] = 0.493*(b - Y[i][j]);
			V[i][j] = 0.877*(r - Y[i][j]);
			n[(int)Y[i][j]]++;
		}

		int accumulate = 0;
		for (int i = 0; i < 256; i++){
			accumulate += n[i];
			double p = (double)accumulate / h / w;
			tranform[i] = roundl(p * 255);
		}


		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			Y[i][j] = tranform[(int)Y[i][j]];
			double r = Y[i][j] + 1.14 * V[i][j];
			double b = Y[i][j] + 2.03 * U[i][j];
			double g = Y[i][j] - 0.39*U[i][j] - 0.58* V[i][j];
			r = r>255 ? 255 : r;
			g = g>255 ? 255 : g;
			b = b > 255 ? 255 : b;
			r = r < 0 ? 0 : r;
			g = g < 0 ? 0 : g;
			b = b < 0 ? 0 : b;

			imgOriginal.SetPixelRGB(j, i, r, g, b);
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}

void CChildView::OnToolsTranslation()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	CImage imgNew;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();
		int x0 = 100;
		int y0 = 100;
		imgNew.Create(w + y0, h + x0, imgOriginal.GetBPP());
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			pixel = imgOriginal.GetPixel(j, i);
			imgNew.SetPixel(j + y0, i + x0, pixel);

		}
		imgOriginal.Destroy();
		imgOriginal.Create(imgNew.GetWidth(), imgNew.GetHeight(), imgNew.GetBPP());
		for (int i = 0; i < imgNew.GetHeight(); i++){
			for (int j = 0; j < imgNew.GetWidth(); j++){
				pixel = imgNew.GetPixel(j, i);
				imgOriginal.SetPixel(j, i, pixel);
			}
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();

}

void CChildView::OnToolsRotation()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	CImage imgNew;
	double theta = 3.14 / 8;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();
		imgNew.Create(round(w*cos(theta) + h*sin(theta)), round(w*sin(theta) + h*cos(theta)), imgOriginal.GetBPP());
		for (int i = 0; i < round(w*sin(theta) + h*cos(theta)); i++)
		for (int j = 0; j < round(w*cos(theta) + h*sin(theta)); j++){
			pixel = imgOriginal.GetPixel(round((j - h*sin(theta))*cos(-theta) - i*sin(-theta)), round((j - h*sin(theta))*sin(-theta) + i*cos(-theta)));
			imgNew.SetPixel(j, i, pixel);
		}
		imgOriginal.Destroy();
		imgOriginal.Create(imgNew.GetWidth(), imgNew.GetHeight(), imgNew.GetBPP());
		for (int i = 0; i < imgNew.GetHeight(); i++){
			for (int j = 0; j < imgNew.GetWidth(); j++){
				pixel = imgNew.GetPixel(j, i);
				imgOriginal.SetPixel(j, i, pixel);
			}
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsScale()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	CImage imgNew;
	double c = 1.5;
	double d = 1.5;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();
		imgNew.Create(w*c, h*d, imgOriginal.GetBPP());
		for (int i = 0; i < h*d; i++)
		for (int j = 0; j < w*c; j++){
			pixel = imgOriginal.GetPixel(j / c, i / d);
			imgNew.SetPixel(j, i, pixel);
		}
		imgOriginal.Destroy();
		imgOriginal.Create(imgNew.GetWidth(), imgNew.GetHeight(), imgNew.GetBPP());
		for (int i = 0; i < imgNew.GetHeight(); i++){
			for (int j = 0; j < imgNew.GetWidth(); j++){
				pixel = imgNew.GetPixel(j, i);
				imgOriginal.SetPixel(j, i, pixel);
			}
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsShear()
{

	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	CImage imgNew;
	double d = 0.2;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();
		imgNew.Create(w, h + d*w, imgOriginal.GetBPP());
		for (int i = 0; i < h + d*w; i++)
		for (int j = 0; j < w; j++){
			pixel = imgOriginal.GetPixel(j, i - d*j);
			imgNew.SetPixel(j, i, pixel);
		}
		imgOriginal.Destroy();
		imgOriginal.Create(imgNew.GetWidth(), imgNew.GetHeight(), imgNew.GetBPP());
		for (int i = 0; i < imgNew.GetHeight(); i++){
			for (int j = 0; j < imgNew.GetWidth(); j++){
				pixel = imgNew.GetPixel(j, i);
				imgOriginal.SetPixel(j, i, pixel);
			}
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsMirror()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	CImage imgNew;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();
		imgNew.Create(w, h, imgOriginal.GetBPP());
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			pixel = imgOriginal.GetPixel(j, i);
			imgNew.SetPixel(j, h - i, pixel);
		}
		imgOriginal.Destroy();
		imgOriginal.Create(imgNew.GetWidth(), imgNew.GetHeight(), imgNew.GetBPP());
		for (int i = 0; i < imgNew.GetHeight(); i++){
			for (int j = 0; j < imgNew.GetWidth(); j++){
				pixel = imgNew.GetPixel(j, i);
				imgOriginal.SetPixel(j, i, pixel);
			}
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}


void CChildView::OnToolsMeanFilter()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();
		byte** grayScale = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			grayScale[i] = (byte*)malloc(w*sizeof(byte));
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			byte r, g, b, avg;
			pixel = imgOriginal.GetPixel(j, i);
			r = GetRValue(pixel);
			g = GetGValue(pixel);
			b = GetBValue(pixel);
			avg = (r + g + b) / 3;
			grayScale[i][j] = avg;
		}

		int a = 7;
		int b = 7;
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			double count = 0;
			double sum = 0;
			for (int ii = (i - a >= 0 ? i - a : 0); ii <= (i + a < h ? i + a : h - 1); ii++)
			for (int jj = (j - b >= 0 ? j - b : 0); jj <= (j + b < w ? j + b : w - 1); jj++){
				count++;
				sum += grayScale[ii][jj];
			}
			imgOriginal.SetPixelRGB(j, i, sum / count, sum / count, sum / count);
		}
	}
	//下半部分的代码是处理图像的每个像素小于8位的，不再详细注解
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//刷新显示图像
	Invalidate();
	UpdateWindow();
}

void CChildView::OnToolsLaplacian()
{

	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //如果当前打开的图像的每个像素大于8位，即没有使用色彩索引表（大部分图像都属于这种）
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();
		byte** grayScale = (byte**)malloc(h*sizeof(byte *));
		for (int i = 0; i < h; i++){
			grayScale[i] = (byte*)malloc(w*sizeof(byte));
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			byte r, g, b, avg;
			pixel = imgOriginal.GetPixel(j, i);
			r = GetRValue(pixel);
			g = GetGValue(pixel);
			b = GetBValue(pixel);
			avg = (r + g + b) / 3;
			grayScale[i][j] = avg;
		}

		double** lap_res = (double**)malloc(h*sizeof(double *));
		for (int i = 0; i < h; i++){
			lap_res[i] = (double*)malloc(w*sizeof(double));
		}

		int mask[3][3] = { { 0, 1, 0 }, { 1, -4, 1 }, { 0, 1, 0 } };
		double min1 = 999999999999;//记录Laplacian results最大最小值，便于rearrange
		double max1 = -99999999999;
		for (int i = 1; i < h - 1; i++)
		for (int j = 1; j < w - 1; j++){
			double sum = 0;
			for (int x = -1; x < 2; x++)
			for (int y = -1; y<2; y++)
			{
				sum += mask[x + 1][y + 1] * grayScale[i + x][j + y];
			}


			if (sum  > max1)
				max1 = sum;
			if (sum < min1)
				min1 = sum;
			lap_res[i][j] = sum;
		}

		double** final = (double**)malloc(h*sizeof(double *));
		for (int i = 0; i < h; i++){
			final[i] = (double*)malloc(w*sizeof(double));
		}


		//double max2 = -999999, min2 = 99999;
		for (int i = 1; i < h - 1; i++)
		for (int j = 1; j < w - 1; j++){
			final[i][j] = -(lap_res[i][j] - min1) * 50 / (max1 - min1) + grayScale[i][j];
			if (final[i][j]>255)
				final[i][j] = 255;
			if (final[i][j] < 0)
				final[i][j] = 0;
			/*if (final[i][j]  > max2)
				max2 = final[i][j];
				if (final[i][j] <min2)
				min2 = final[i][j];*/
		}
		for (int i = 1; i < h - 1; i++)
		for (int j = 1; j < w - 1; j++){
			imgOriginal.SetPixelRGB(j, i, (byte)final[i][j], (byte)final[i][j], (byte)final[i][j]);
			//if (lap_res[i][j]>255)
			//lap_res[i][j] = 255;
			//if (lap_res[i][j]<0)
			//lap_res[i][j] = 0;
			//imgOriginal.SetPixelRGB(j, i, (byte)lap_res[i][j], (byte)lap_res[i][j], (byte)lap_res[i][j]);
		}
	}
	//刷新显示图像
	Invalidate();
	UpdateWindow();

}

void CChildView::OnToolsBilateral()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	COLORREF pixel;	
	if (!imgOriginal.IsIndexed()){
		int nWidth = imgOriginal.GetWidth();
		int nHeight = imgOriginal.GetHeight();

		CImage imgBackup;	
		imgBackup.Create(nWidth, nHeight, imgOriginal.GetBPP());
		for (int i = 0; i < nHeight; i++)
		for (int j = 0; j < nWidth; j++){
			pixel = imgOriginal.GetPixel(j, i);
			imgBackup.SetPixel(j, i, pixel);
		}
		double dSigma1 = 0.02*pow((nWidth*nWidth + nHeight*nHeight),0.5);
		double dSigmaR = 0, dSigmaG = 0, dSigmaB = 0;
		for (int i = 0; i < nHeight-1; i++)
		{
			for (int j = 0; j < nWidth-1; j++)
			{
				pixel = imgBackup.GetPixel(j, i);
				double r = GetRValue(pixel);
				double g = GetGValue(pixel);
				double b = GetBValue(pixel);
				pixel = imgBackup.GetPixel(j + 1, i);
				double r1 = GetRValue(pixel);
				double g1 = GetGValue(pixel);
				double b1 = GetBValue(pixel);
				pixel = imgBackup.GetPixel(j, i + 1);
				double r2 = GetRValue(pixel);
				double g2 = GetGValue(pixel);
				double b2 = GetBValue(pixel);
				dSigmaR += abs(r - r1) + abs(r - r2);
				dSigmaG += abs(g - g1) + abs(g - g2);
				dSigmaB += abs(b - b1) + abs(b - b2);
			}
		}
		dSigmaR /= (nHeight - 1)*(nWidth - 1);
		dSigmaG /= (nHeight - 1)*(nWidth - 1);
		dSigmaB /= (nHeight - 1)*(nWidth - 1);		
		//char buffer[50];
		//sprintf(buffer, "%lf %lf %lf", dSigmaR, dSigmaG, dSigmaB);
		//::AfxMessageBox(buffer);		
		int nWindows = (int)(1 + 2 * ceil(3 * dSigma1));
		char*  buffer = new char[200];
		sprintf(buffer, "nWindows:%d\ndSigma1:%lf\ndSigmaR:%lf\ndSigmaG:%lf\ndSigmab:%lf", nWindows, dSigma1, dSigmaR, dSigmaG, dSigmaB);
		::AfxMessageBox(buffer);
		////////////////////////参数说明///////////////////////////////////  
		//nWidth:图像宽度  
		//nHeight:图像高度  
		//dSigma1、dSigma2:分别为几何与灰度相关高斯函数的方差  
		double* dDistance = new double[nWindows*nWindows]; //计算距离中间点的几何距离  
		for (int i = 0; i < nWindows*nWindows; i++)
		{
			int nNumX = i / nWindows;
			int nNumY = i%nWindows;
			dDistance[i] = ((nWindows - 1) / 2 - nNumX)*((nWindows - 1) / 2 - nNumX) + ((nWindows - 1) / 2 - nNumY)*((nWindows - 1) / 2 - nNumY);
			dDistance[i] = exp(-0.5*dDistance[i] / dSigma1 / dSigma1);   //C参数  
		}
		//以下求解灰度值的差  
		for (int i = 0; i < nHeight; i++)
		{
			for (int j = 0; j < nWidth; j++)
			{
				pixel = imgBackup.GetPixel(j, i);
				double rP = GetRValue(pixel);
				double gP = GetGValue(pixel);
				double bP = GetBValue(pixel);
				double dDataR = 0.0;
				double dTotalR = 0.0;                      //用于进行归一化   
				double dDataG = 0.0;
				double dTotalG = 0.0;                      //用于进行归一化   
				double dDataB = 0.0;
				double dTotalB = 0.0;                      //用于进行归一化   
				for (int m = 0; m < nWindows*nWindows; m++)
				{
					int nNumX = m / nWindows;               //行索引  
					int nNumY = m%nWindows;               //列索引  
					int nX = i - (nWindows - 1) / 2 + nNumX;
					int nY = j - (nWindows - 1) / 2 + nNumY;
					if ((nY >= 0) && (nY < nWidth) && (nX >= 0) && (nX < nHeight))
					{
						pixel = imgBackup.GetPixel(nY, nX);
						double r = GetRValue(pixel);
						double g = GetGValue(pixel);
						double b = GetBValue(pixel);
						double dRDiff = fabs(rP - r);
						double dGDiff = fabs(gP - g);
						double dBDiff = fabs(bP - b);
						dRDiff = exp(-0.5*dRDiff * dRDiff / dSigmaR / dSigmaR); //色参数  
						dGDiff = exp(-0.5*dGDiff * dGDiff / dSigmaG / dSigmaG); //色参数  
						dBDiff = exp(-0.5*dBDiff * dBDiff / dSigmaB / dSigmaB); //色参数  
						if (m != 4)
						{
							dDataR += r * dRDiff * dDistance[m]; //距离和色参数综合  
							dTotalR += dRDiff * dDistance[m];    //加权系数求和，进行归一化  
							dDataG += g * dGDiff * dDistance[m]; //距离和色参数综合  
							dTotalG += dGDiff * dDistance[m];    //加权系数求和，进行归一化  
							dDataB += b * dBDiff * dDistance[m]; //距离和色参数综合  
							dTotalB += dBDiff * dDistance[m];    //加权系数求和，进行归一化  
						}
					}
				}
				dDataR /= dTotalR;
				dDataG /= dTotalG;
				dDataB /= dTotalB;
				imgOriginal.SetPixelRGB(j, i, (byte)dDataR, (byte)dDataG, (byte)dDataB);
				//char*  buffer2 = new char[200];
				//sprintf(buffer2, "%d %d\n %d %d\n %d %d", (int)rP, (int)dDataR, (int)gP, (int)dDataG, (int)bP, (int)dDataB);
				//::AfxMessageBox(buffer2);
			}
			Invalidate();
			UpdateWindow();
		}
		delete[]dDistance;
	}
	Invalidate();
	UpdateWindow();
}


/*-------------------------------------------------------------------------------------------------
以下代码是本基础框架包含的现成的函数，如打开、显示、存储图像文件的代码，同学们可以不去深入研究
--------------------------------------------------------------------------------------------------*/

// CChildView的构造函数和析构函数以及消息映射表(更详细的内容请参考C++及MFC的经典书籍)

CChildView::CChildView()
{
	m_nImageSize = SIZE_NONE;
}

CChildView::~CChildView()
{

}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_FILE_OPENIMAGE, OnFileOpenimage)
	ON_COMMAND(ID_FILE_SAVEIMAGE, OnFileSaveImage)
	ON_COMMAND(ID_TOOLS_MAKEBW, OnToolsMakeBW)
	ON_COMMAND(ID_FILE_IMAGEINFO, OnImageInfo)
	ON_UPDATE_COMMAND_UI(ID_SIZE_HALF, OnUpdateSizeHalf)
	ON_UPDATE_COMMAND_UI(ID_SIZE_ORIGINAL, OnUpdateSizeOriginal)
	ON_UPDATE_COMMAND_UI(ID_SIZE_DOUBLE, OnUpdateSizeDouble)
	ON_UPDATE_COMMAND_UI(ID_SIZE_FILL, OnUpdateSizeFill)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVEIMAGE, OnUpdateImage)
	ON_UPDATE_COMMAND_UI(ID_FILE_IMAGEINFO, OnUpdateImage)
	ON_UPDATE_COMMAND_UI(ID_TOOLS_MAKEBW, OnUpdateImage)
	ON_COMMAND_RANGE(ID_SIZE_HALF, ID_SIZE_FILL, OnChangeSize)
	ON_COMMAND(ID_TOOLS_Binarization, &CChildView::OnToolsBinarization)
	ON_COMMAND(ID_TOOLS_BinarizationLocal, &CChildView::OnToolsBinarizationLocal)
	ON_COMMAND(ID_TOOLS_Dilation, &CChildView::OnToolsDilation)
	ON_COMMAND(ID_TOOLS_Erosion, &CChildView::OnToolsErosion)
	ON_COMMAND(ID_TOOLS_Openning, &CChildView::OnToolsOpenning)
	ON_COMMAND(ID_TOOLS_Closing, &CChildView::OnToolsClosing)
	ON_COMMAND(ID_TOOLS_HMT, &CChildView::OnToolsHMT)
	ON_COMMAND(ID_TOOLS_Logarithmic_Operation, &CChildView::OnToolsLog)
	ON_COMMAND(ID_TOOLS_Equalization, &CChildView::OnToolsEqualization)
	ON_COMMAND(ID_TOOLS_Translation, &CChildView::OnToolsTranslation)
	ON_COMMAND(ID_TOOLS_Rotation, &CChildView::OnToolsRotation)
	ON_COMMAND(ID_TOOLS_Scale, &CChildView::OnToolsScale)
	ON_COMMAND(ID_TOOLS_Shear, &CChildView::OnToolsShear)
	ON_COMMAND(ID_TOOLS_Mirror, &CChildView::OnToolsMirror)
	ON_COMMAND(ID_TOOLS_Mean_Filter, &CChildView::OnToolsMeanFilter)
	ON_COMMAND(ID_TOOLS_Laplacian, &CChildView::OnToolsLaplacian)
	ON_COMMAND(ID_TOOLS_Bilateral, &CChildView::OnToolsBilateral)






END_MESSAGE_MAP()



// CChildView message handlers
// 程序中的大部分按钮对应的现成的函数
BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1), NULL);

	return TRUE;
}

//本段代码在每一次消息循环中被调用来以一定大小比例显示当前图片
void CChildView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if (!imgOriginal.IsNull())
	{

		switch (m_nImageSize)
		{
		case SIZE_HALF:
			imgOriginal.StretchBlt(dc, 0, 0, imgOriginal.GetWidth() / 2, imgOriginal.GetHeight() / 2, SRCCOPY);
			break;
		case SIZE_ORIGINAL:
			imgOriginal.StretchBlt(dc, 0, 0, imgOriginal.GetWidth(), imgOriginal.GetHeight(), SRCCOPY);
			break;
		case SIZE_DOUBLE:
			imgOriginal.StretchBlt(dc, 0, 0, imgOriginal.GetWidth() * 2, imgOriginal.GetHeight() * 2, SRCCOPY);
			break;
		case SIZE_FILL:
			CRect rctWindowSize;
			GetClientRect(rctWindowSize);
			imgOriginal.StretchBlt(dc, 0, 0, rctWindowSize.Width(), rctWindowSize.Height(), SRCCOPY);
		};
	}

}

//本段代码实现打开文件对话框并选择一个图片文件并显示
void CChildView::OnFileOpenimage(void)
{
	// TODO: Add your command handler code here
	CString strFilter;
	CSimpleArray<GUID> aguidFileTypes;
	HRESULT hResult;

	hResult = imgOriginal.GetExporterFilterString(strFilter, aguidFileTypes);
	if (FAILED(hResult)) {
		CString fmt;
		fmt.Format("GetExporterFilter failed:\n%x - %s", hResult, _com_error(hResult).ErrorMessage());
		::AfxMessageBox(fmt);
		return;
	}

	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST, strFilter);
	dlg.m_ofn.nFilterIndex = m_nFilterLoad;
	hResult = (int)dlg.DoModal();
	if (FAILED(hResult)) {
		return;
	}

	m_nFilterLoad = dlg.m_ofn.nFilterIndex;
	imgOriginal.Destroy();
	hResult = imgOriginal.Load(dlg.GetPathName());
	if (FAILED(hResult)) {
		CString fmt;
		fmt.Format("Load image failed:\n%x - %s", hResult, _com_error(hResult).ErrorMessage());
		::AfxMessageBox(fmt);
		return;
	}

	m_nImageSize = SIZE_ORIGINAL;
	Invalidate();
	UpdateWindow();

}
//本段代码实现将当前的图片存储至本地
void CChildView::OnFileSaveImage(void)
{
	CString strFilter;
	CSimpleArray<GUID> aguidFileTypes;
	HRESULT hResult;


	strFilter = "Bitmap image|*.bmp|JPEG image|*.jpg|GIF image|*.gif|PNG image|*.png||";

	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER, strFilter);
	dlg.m_ofn.nFilterIndex = m_nFilterLoad;
	hResult = (int)dlg.DoModal();
	if (FAILED(hResult)) {
		return;
	}

	// Add the appropriate extension if the user didn't type one

	CString strFileName;
	CString strExtension;

	strFileName = dlg.m_ofn.lpstrFile;


	// add the file extension if the user didn't supply one
	if (dlg.m_ofn.nFileExtension == 0)
	{
		switch (dlg.m_ofn.nFilterIndex)
		{
		case 1:
			strExtension = "bmp";
			break;
		case 2:
			strExtension = "jpg";
			break;
		case 3:
			strExtension = "gif";
			break;
		case 4:
			strExtension = "png";
			break;
		default:
			break;
		}

		strFileName = strFileName + '.' + strExtension;

	}

	// the extension on the file name will determine the file type that is saved
	hResult = imgOriginal.Save(strFileName);
	if (FAILED(hResult)) {
		CString fmt;
		fmt.Format("Save image failed:\n%x - %s", hResult, _com_error(hResult).ErrorMessage());
		::AfxMessageBox(fmt);
		return;
	}

}



//本段代码查询图像相关信息
void CChildView::OnImageInfo()
{
	CInfoDlg infoDlg(imgOriginal);
	infoDlg.DoModal();
}

//本段代码改变图像显示大小
void CChildView::OnChangeSize(UINT nID)
{
	m_nImageSize = (SizesEnum)(nID - ID_SIZE_BASE);
	Invalidate();
	UpdateWindow();
}

//下面四段代码以不同的大小显示图片
void CChildView::OnUpdateSizeHalf(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!imgOriginal.IsNull());
	pCmdUI->SetCheck((UINT)(m_nImageSize == SIZE_HALF));
}

void CChildView::OnUpdateSizeOriginal(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!imgOriginal.IsNull());
	pCmdUI->SetCheck((UINT)(m_nImageSize == SIZE_ORIGINAL));
}

void CChildView::OnUpdateSizeDouble(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!imgOriginal.IsNull());
	pCmdUI->SetCheck((UINT)(m_nImageSize == SIZE_DOUBLE));
}

void CChildView::OnUpdateSizeFill(CCmdUI* pCmdUI)
{
	pCmdUI->Enable(!imgOriginal.IsNull());
	pCmdUI->SetCheck((UINT)(m_nImageSize == SIZE_FILL));
}

void CChildView::OnUpdateImage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(!imgOriginal.IsNull());
}



