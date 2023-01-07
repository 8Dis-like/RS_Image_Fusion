#include"HIS.h"
int main() {
	string path_r = "../img_sources/subset-red-byte-297-216.bmp";
	string path_g = "../img_sources/subset-green-byte-297-216.bmp";
	string path_b = "../img_sources/subset-blue-byte-297-216.bmp";
	string path_pan = "../img_sources/subset-pan-byte-594-430.bmp";
	HISchanged(path_r, path_g, path_b, path_pan);

	return 0;
}