
//���������Ҫ���ļ�

#include "stdafx.h"
#include "CImage.h"
#include "ChildView.h"
#include "InfoDlg.h"
#include <string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*------------------------------------------------------------------------------------------------------------
1.���������ǻ���MFC��������ʾ������ͼ���Ӧ�ó����ܣ�ͬѧ�ǿ����ڴ˿�ܵĻ����������ҵ����Ĺ��ܴ��롣
2.�����д���ͼ�������΢��MFC��������ṩ��CImage�࣬�����ṩ�˶�ȡ����ʾ���ָ�ʽͼ��ĺ��������Դ��򻯴�
��ͼ������еĹ�������ʹ��Ұ���Ҫ������������ҵ���еĹ���ʵ���ϡ�
3.ÿ����ҵ��Ӧ�Ų˵���Tools�µ�һ���˵��ÿ���˵����Ӧһ���հ׺���������ʵ��ÿ����ҵ�Ĺ��ܴ��롣
4.�����һ��ͼ���Ժ�ͼ�������洢��CImage���͵�imgOriginal�����У����CImage��ĳ�Ա������ǿ�󣬿�����MSDN
����F1�򿪣��в�ѯ�й����Ĳ�����imgOriginal�����Ǳ�������CChildView��˽�б��������������µĳ�Ա�����б����á�
�й�ͼ���һЩ��������������imgOriginal.������()�ķ������ã������һ��ͼ�������imgOriginal.Load("x.jpg")��
�������õĲ�������MSDN������CImage��
5.�����CChildView::OnToolsMakeBW��һ���򵥵����ӣ����ǲ˵���Tools�µĵ�һ���˵����Ӧ����Ϣ��Ӧ�������Ѳ�ɫͼ��
תΪ�Ҷ�ͼ��
------------------------------------------------------------------------------------------------------------*/
void CChildView::OnToolsMakeBW(void)
{
	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��

	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
	{
		//��ÿ�����شӲ�ɫתΪ�Ҷ�
		COLORREF pixel;//����һ�����ص�ɫ����Ϣ
		int maxY = imgOriginal.GetHeight(), maxX = imgOriginal.GetWidth();//��imgOriginalȡ��ͼ��ĳ�maxX�͸�maxY
		byte r, g, b, avg;//r,g,bֵ��ƽ���Ҷ�ֵ����
		for (int x = 0; x < maxX; x++) //��ѭ��
		{
			for (int y = 0; y < maxY; y++) //��ѭ��
			{
				pixel = imgOriginal.GetPixel(x, y);//ȡ��λ��(x,y)λ���ϵ����ص�ɫ����Ϣ
				r = GetRValue(pixel);//ȡ��pixel��rֵ
				g = GetGValue(pixel);//ȡ��pixel��gֵ
				b = GetBValue(pixel);//ȡ��pixel��bֵ
				avg = ((r + g + b) / 3);//�ô�ʽ��rgb��ɫֵתΪ�Ҷ�ֵ
				imgOriginal.SetPixelRGB(x, y, avg, avg, avg);//��λ��(x,y)λ���ϵ����ص�rgbɫ��ͨ����Ϊ(avg,avg,avg)����ת��Ϊ�Ҷ�
			}
		}
	}
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
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

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}

//Ϊ�˵��������Ϣ��Ӧ���������ʵ�ִ��루ϸ�ڲμ�ppt��
void CChildView::OnToolsBinarization()
{
	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
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
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsBinarizationLocal()
{
	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
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

		/*��ͼ��ֳ�10*10�Ŀ飬�ֲ�����ֵ��*/
		for (int ii = 0; ii < 10; ii++)
		for (int jj = 0; jj < 10; jj++)
		{
			int countTotal;
			countTotal = h*w/100;
			double maxBetween = 0;
			int resT;
			for (int T = 0; T < 255; T++){
				int  countFG = 0, countBG = 0;
				int sumTotal = 0, sumFG = 0, sumBG = 0;
				double avgTotal, avgFG, avgBG;
				for (int i = ii*h / 10; i < (ii+1)*h / 10; i++)
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
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsErosion()
{
	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
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
			else if(grayScale[i][j - 1] == 0 && grayScale[i][j] == 0 && grayScale[i][j + 1] == 0)
				erosion[i][j] = 0;
			else
				erosion[i][j] = 255;
		}
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			imgOriginal.SetPixelRGB(j, i, erosion[i][j], erosion[i][j], erosion[i][j]);
		}
	}
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsDilation()
{

	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
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
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsOpenning()
{
	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
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
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsClosing()
{
	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
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
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
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
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
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
			Y[i][j]=255*log(Y[i][j]/255 + 1) / (log(max/255 + 1));
			double r = Y[i][j] + 1.14 * V[i][j];
			double b = Y[i][j] + 2.03 * U[i][j];
			double g = Y[i][j] - 0.39*U[i][j] - 0.58* V[i][j];
			r = r>255 ? 255 : r;
			g = g>255 ? 255 : g;
			b = b>255 ? 255 : b;
			r = r<0 ? 0: r;
			g = g<0 ? 0 : g;
			b = b<0 ? 0 : b;
			
			imgOriginal.SetPixelRGB(j, i, r, g, b);
			
		}
	}
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsEqualization()
{
	COLORREF pixel;
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
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
			b = b>255 ? 255 : b;
			r = r<0 ? 0 : r;
			g = g<0 ? 0 : g;
			b = b<0 ? 0 : b;

			imgOriginal.SetPixelRGB(j, i, r, g, b);			
		}		
	}
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}

void CChildView::OnToolsTranslation()
{
	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	CImage imgNew;	
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();
		int x0 = 100;
		int y0 = 100;
		imgNew.Create(w+y0, h+x0, imgOriginal.GetBPP());
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){			
			pixel = imgOriginal.GetPixel(j, i);			
			imgNew.SetPixel(j+y0, i+x0, pixel);
			
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
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();

}

void CChildView::OnToolsRotation()
{
	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	CImage imgNew;
	double theta = 3.14/8;		
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
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
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsScale()
{
	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	CImage imgNew ;
	double c = 1.5;
	double d = 1.5;
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();		
		imgNew.Create(w*c, h*d, imgOriginal.GetBPP());
		for (int i = 0; i < h*d; i++)
		for (int j = 0; j < w*c; j++){
			pixel = imgOriginal.GetPixel(j/c, i/d);
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
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsShear()
{

	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	CImage imgNew;
	double d=0.2;
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();
		imgNew.Create(w,  h+d*w, imgOriginal.GetBPP());
		for (int i = 0; i < h + d*w; i++)
		for (int j = 0; j < w; j++){
			pixel = imgOriginal.GetPixel(j, i-d*j);
			imgNew.SetPixel(j , i, pixel);
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
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}
void CChildView::OnToolsMirror()
{
	CWaitCursor wait;//�ں���ִ�й�����ʹ���ͼ���ɵȴ�ͼ��
	COLORREF pixel;
	CImage imgNew;
	if (!imgOriginal.IsIndexed()) //�����ǰ�򿪵�ͼ���ÿ�����ش���8λ����û��ʹ��ɫ���������󲿷�ͼ���������֣�
	{
		int h = imgOriginal.GetHeight();
		int w = imgOriginal.GetWidth();
		imgNew.Create(w, h, imgOriginal.GetBPP());
		for (int i = 0; i < h; i++)
		for (int j = 0; j < w; j++){
			pixel = imgOriginal.GetPixel(j, i);
			imgNew.SetPixel(j, h-i, pixel);
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
	//�°벿�ֵĴ����Ǵ���ͼ���ÿ������С��8λ�ģ�������ϸע��
	else
	{
		::AfxMessageBox("Please use 24-bit images!");
	}

	//ˢ����ʾͼ��
	Invalidate();
	UpdateWindow();
}



/*-------------------------------------------------------------------------------------------------
���´����Ǳ�������ܰ������ֳɵĺ�������򿪡���ʾ���洢ͼ���ļ��Ĵ��룬ͬѧ�ǿ��Բ�ȥ�����о�
--------------------------------------------------------------------------------------------------*/

// CChildView�Ĺ��캯�������������Լ���Ϣӳ���(����ϸ��������ο�C++��MFC�ľ����鼮)

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
	




END_MESSAGE_MAP()



// CChildView message handlers
// �����еĴ󲿷ְ�ť��Ӧ���ֳɵĺ���
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

//���δ�����ÿһ����Ϣѭ���б���������һ����С������ʾ��ǰͼƬ
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

//���δ���ʵ�ִ��ļ��Ի���ѡ��һ��ͼƬ�ļ�����ʾ
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
//���δ���ʵ�ֽ���ǰ��ͼƬ�洢������
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



//���δ����ѯͼ�������Ϣ
void CChildView::OnImageInfo()
{
	CInfoDlg infoDlg(imgOriginal);
	infoDlg.DoModal();
}

//���δ���ı�ͼ����ʾ��С
void CChildView::OnChangeSize(UINT nID)
{
	m_nImageSize = (SizesEnum)(nID - ID_SIZE_BASE);
	Invalidate();
	UpdateWindow();
}

//�����Ķδ����Բ�ͬ�Ĵ�С��ʾͼƬ
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



