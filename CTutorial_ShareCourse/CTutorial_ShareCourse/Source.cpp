#include <stdio.h>
#include <iostream>
void part3(){
	//part3
	float a, b;//精確度小數6位  , double 可以保10位
	b = 1.0e12;
	b = b - 1.0;
	a = b + 1.0;
	printf("%f\n", a); //round-off error :超大數-極小數 會產生誤差。  

	//用scanf讀取浮點數
	double xx;
	scanf("%lf", &xx); //%lf搭配double才會有正確的結果    %f搭配float

	float x = 123456.0;
	double y = 2.34e12;
	printf("%.2f or %e\n", x, x);//預設小數點位數六位，可藉由.X 來設定要顯示幾位，還可以用此功能來做四捨五入、顯示位數、顯示+-號、設定顯示位數不足補0等...
	printf("%.2f or %e\n", y, y);
	printf("%12.2f \n", y);//總長度12,小數點2位 剩下的四捨五入
	int i, j;
	scanf("%d  %d", &i, &j);

	printf("%*.*f", i, j, y);//此處的*(星號)會被後面的i j取代

}
void part2(){
	//part2
	char ch;
	char chArray[10];
	printf("Please enter an char");
	scanf("%c", &ch);
	printf("Please enter an string");
	scanf("%c", chArray);//此處不需要用&  因為陣列名本身就指向其位置
	printf("ASCII code for '%c' is %d \n", ch, ch);//字元的ASCII碼對應到一個二進位數值，此數值可以藉由%d得知
	printf("%c%c", 0xa7, 0x70);//2bit 組合成一個中文字
}

void part1(){ 
	//part1
	int x;
	int y;
	unsigned unsignJ = 4294967295;
	printf("Enter an integer:");
	scanf("%d", &x);//告訴scanf要如何解讀收到的資料  &取得變數的位址
	printf("Enter an integer:");
	scanf("%d", &y);
	//也可以寫成這樣::
	//scanf(%d%d,&x,&y); 當使用者輸入時  以空白建、Tab、enter分隔
	printf("%d\n", x + y);//%d代表有號整數
	printf("%u\n", unsignJ);//%u代表unsign int ,若使用錯誤會造成輸出不符合預期
}
int main(){

	char ss[10] = "123456789"; //陣列長度為10的char 只能存9位的資料 最後一位是結尾符號 '\0'
	ss[2] = '0';
	printf("%s", ss);
	
	system("PAUSE");
	return 0;
}

