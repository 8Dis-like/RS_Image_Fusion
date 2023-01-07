#include"Methods.h"

Mat RGBfusion(string path1, string path2, string path3)
{
    Mat R = imread(path1,IMREAD_GRAYSCALE); Mat G = imread(path2,IMREAD_GRAYSCALE); Mat B = imread(path3,IMREAD_GRAYSCALE);
    vector<Mat> channels;
    channels.push_back(R);
    channels.push_back(G);
    channels.push_back(B);
    Mat MSData; 
    merge(channels, MSData);
    Contraststretch(MSData, 0, 255);
    return MSData;
}

void HPF(Mat& pan, Mat& color)
{
    //高通滤波
    Mat pantemp(pan);
    int i, j, k;
    int nHeight = color.rows; 	//行数，图像高度
    int nWidth = color.cols;  	//列数，图像宽度
    int channels = color.channels();

    int Division = 1;
    int Offset = 0;
    double values[9][9]{}; int Size = 3;
    values[0][0] = values[0][2] = values[2][0] = values[2][2] = -1;
    values[0][1] = values[1][0] = values[1][2] = values[2][1] = -1;
    values[1][1] = 9;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)    values[i][j] /= 9;

    for (i = 0; i < nHeight; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            if (i == 0 || i == nHeight - 1 || j == 0 || j == nWidth - 1)
                pantemp.at<uchar>(i, j) = pan.at<uchar>(i, j);
            else {
                double gray = 0;
                double sum = 0.0;
                double cof = 0; double size = 0;
                double c = 0;
                for (int m = 0; m < Size; m++) {
                    for (int n = 0; n < Size; n++) {
                        uchar pTemp = pan.at<uchar>(i - Size / 2 + m, j - Size / 2 + n);
                        sum += (values[m][n] * pTemp);
                        c += values[m][n];
                        size++;
                    }
                }
                gray = sum; cof = c;
                if (cof < 0) {
                    uchar pTemp = pan.at<uchar>(i, j);
                    gray = pTemp;
                }
                else if (size < Size * Size) {
                    if (cof == 0)cof = 1;
                    uchar tt = gray / cof + Offset;
                    if (tt < 255)gray = tt; else gray = 255;
                }
                else {
                    uchar ttt = gray / Division + Offset;
                    if (ttt < 255)gray = ttt; else gray = 255;
                }
                if (gray < 0)gray = 0;
                pantemp.at<uchar>(i, j) = gray;
            }
        }
    }
    //逐波段相加
    for (k = 0; k < channels; k++)
    {
        for (i = 0; i < nHeight; i++)
        {
            for (j = 0; j < nWidth; j++)
            {
                uchar t1 = pantemp.at<uchar>(i, j);
                uchar t2 = color.at<Vec3b>(i, j)[k];
                if (t1 + t2 <= 255) {
                    if (t1 + t2 > 0) { color.at<Vec3b>(i, j)[k] = t1 + t2; }
                    else { color.at<Vec3b>(i, j)[k] = 0; }
                }
                else { color.at<Vec3b>(i, j)[k] = 255; }

            }
        }
    }
}

void weighing(Mat& pan, Mat& color)
{
    //低通-均匀滤波
    Mat pantemp(pan);
    int i, j, k;
    int nHeight = color.rows; 	//行数，图像高度
    int nWidth = color.cols;  	//列数，图像宽度
    int channels = color.channels();

    //滤波1
    //blur(pan, pantemp, Size(7,7));
    
    //滤波2
   /* float exp = 1.0 / 9;    
    Point anchor = Point(-1, -1);
    Mat kernel = (Mat_<float>(3, 3) << exp, exp, exp, exp, exp, exp, exp, exp, exp);
    filter2D(pan, pantemp, -1, kernel,anchor, 0.0, BORDER_DEFAULT);*/

    // 滤波3--- work 
    int Division = 1;
    int Offset = 0;
    double values[9][9]; int Size = 3;
    values[0][0] = values[0][1] = values[0][1] = 1;
    values[1][0] = values[1][1] = values[1][2] = 1;
    values[2][0] = values[2][1] = values[2][2] = 1;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)    values[i][j] /= 9;

    for (i = 0; i < nHeight; i++)
    {
        for (j = 0; j < nWidth; j++)
        {
            if (i == 0 || i == nHeight - 1 || j == 0 || j == nWidth - 1)pantemp.at<uchar>(i, j) = pan.at<uchar>(i, j);
            else {
                double gray = 0;
                double sum = 0.0;
                double cof = 0; double size = 0;
                double c = 0;
                for (int m = 0; m < Size; m++) {
                    for (int n = 0; n < Size; n++) {
                        uchar pTemp = pan.at<uchar>(i - Size / 2 + m, j - Size / 2 + n);
                        sum += (values[m][n] * pTemp);
                        c += values[m][n];
                        size++;
                    }
                }
                gray = sum; cof = c;
                if (cof < 0) { uchar pTemp = pan.at<uchar>(i, j); gray = pTemp;}
                else if (size < Size * Size) { 
                    if (cof == 0)cof = 1;
                    uchar tt = gray / cof + Offset;
                    if (tt < 255)gray = tt; else gray = 255;
                }
                else {
                    uchar ttt = gray / Division + Offset;
                    if (ttt < 255)gray = ttt; else gray = 255;
                }
                    if (gray < 0)gray = 0;
                    pantemp.at<uchar>(i, j) = gray;
            }
        }
    }
    //逐波段加权
    for (k = 0; k < channels; k++)
    {
        for (i = 0; i < nHeight; i++)
        {
            for (j = 0; j < nWidth; j++)
            {
                uchar now = pantemp.at<uchar>(i, j);        //低频分量
                uchar row = pan.at<uchar>(i, j);            //原图
                uchar pData2 = color.at<Vec3b>(i, j)[k];    //色彩分量
                uchar high = row - now;                     //高频分量
                uchar fTemp = pData2 + pData2 * (high / now);//变换分量
                if (fTemp <=255) {
                    if (fTemp > 0) {color.at<Vec3b>(i, j)[k] = fTemp;}
                    else { color.at<Vec3b>(i, j)[k] = 0; }
                }else { color.at<Vec3b>(i, j)[k] = 255; }
            }
        }
    }
}

//对比度增强--默认拉伸至灰度区间[0,255]
void Contraststretch(Mat& M)
{
    int nHeight = M.rows; 	//行数，图像高度
    int nWidth = M.cols;  	//列数，图像宽度
    int channels = M.channels();
    //逐波段拉伸
    int i, j, k;
    for (k = 0; k < channels; k++)
    {
        //统计RGB分量各自 灰度区间
        uchar m_MinGray = 255, m_MaxGray = 0;
        for (i = 0; i < nHeight; i++)
        {
            for (j = 0; j < nWidth; j++)
            {
                if (M.at<Vec3b>(i, j)[k] < m_MinGray)   m_MinGray = M.at<Vec3b>(i, j)[k];
                if (M.at<Vec3b>(i, j)[k] > m_MaxGray)   m_MaxGray = M.at<Vec3b>(i, j)[k];
            }
        }
        //RGB各通道拉伸
        for (i = 0; i < nHeight; i++)
        {
            for (j = 0; j < nWidth; j++)
            {
                M.at<Vec3b>(i, j)[k] = (M.at<Vec3b>(i, j)[k] - m_MinGray) * 255 / (m_MaxGray - m_MinGray);
            }
        }
    }
}

//对比度增强--自主选择目标拉伸区间
void Contraststretch(Mat& M, int minout, int maxout)
{
    int nHeight = M.rows; 	//行数，图像高度
    int nWidth = M.cols;  	//列数，图像宽度
    int channels = M.channels();
    //逐波段拉伸
    int i, j,k;
    for (k = 0; k < channels; k++)
    {
        //统计RGB分量各自 灰度区间
        uchar m_MinGray = 255, m_MaxGray = 0;
        for (i = 0; i < nHeight; i++)
        {
            for (j = 0; j < nWidth; j++)
            {
                if (M.at<Vec3b>(i, j)[k] < m_MinGray)   m_MinGray = M.at<Vec3b>(i, j)[k];
                if (M.at<Vec3b>(i, j)[k] > m_MaxGray)   m_MaxGray = M.at<Vec3b>(i, j)[k];
            }
        }
        //RGB各通道拉伸
        for (i = 0; i < nHeight; i++)
        {
            for (j = 0; j < nWidth; j++)
            {
                M.at<Vec3b>(i, j)[k] = (maxout - minout)/(m_MaxGray - m_MinGray)
                    * (M.at<Vec3b>(i, j)[k] - m_MinGray) + minout;
            }
        }
    }
}
