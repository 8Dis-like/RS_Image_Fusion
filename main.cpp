#include"Methods.h"

int main()
{
    //请按RGB顺序排列图片路径
    string path1 = ".\\subset-red-byte-297-216.bmp";
    string path2 = ".\\subset-green-byte-297-216.bmp";
    string path3 = ".\\subset-blue-byte-297-216.bmp";
    char imageName[] = ".\\subset-pan-byte-594-430.bmp";
    Mat pan = imread(imageName);   // 读入图片 
    if (pan.empty())     // 判断文件是否正常打开  
    {   fprintf(stderr, "Can not load image %s\n", imageName);
        waitKey(6000);  // 等待6000 ms后窗口自动关闭   
        return -1;
    }


    namedWindow("pre-processing", WINDOW_AUTOSIZE);
    imshow("pre-processing", pan);

    //融合图像
    Mat fusionimage = RGBfusion(path1, path2, path3);
    weighing(pan, fusionimage);
  

    namedWindow("post-processing", WINDOW_AUTOSIZE);
    imshow("post-processing", fusionimage);
    waitKey();
	
	return 0;
}
