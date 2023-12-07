#include "STC8H.h"
#include "intrins.h"
#include "field.h"

char show();
char dokey(char key);
void lighton(char iswin);
void lightoff(char iswin);
char brain();
char checktowin();
char checktowinuser();
char getlinesum(char a, char b, char c);
char setwin(char line);
char checkuser2(); 
char set3(char i); // set 3th boll
void nextstep(); 

char red = 0; // bright counter of red
char begin = 1;
char keyline = 0;

void Delay1ms()		//@33.1776MHz
{
	unsigned char i, j, k;
	k = 3;
	while(--k){
		i = 6;
		j = 93;
		do
		{
			while (--j);
		} while (--i);
	}
}


void main(){
	char t, tm, t0, t11 = 0;	
	char tkey = 0;
	int i = 0;
	char win = 0;
	int showwin = 0;
	char step = 0;

		P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
	
		P1 = 0xf8;
		P5 = 0xff;
		
		// key lines init as inputs (set 1)
		P3 = 0xf0;
		// load show
		for(i = 1; i < 10; i++){
			L[i] = 1;
			L[i-1] = 0;								
			for(showwin = 0; showwin < 100; showwin++){
				if(show() > 0) break;	
			}
		}
		L[9] = 0;
		
		while(1){			
			t0 = show();
			if(t0 == 11)
			{
				t0 = 0;
				if(t11 == 0){
					t11 = 1;
					step = 1;
				}
			}
			if(t0 == 0){
				t = tm;
				tm = 0;
			}else{
				tm = t0;
				t = 0;
			}
			Delay1ms();		
			if(t == 10){ // reset				
				for(i = 0; i < 10; i++){
					L[i] = 0;
				}
				win = 0;
				step = 0;
				showwin = 0;
				tkey = 0;
				t11 = 0;
				continue;
			}
			if(win > 0){ 
				if(++showwin == 200){
					lightoff(win);
					continue;
				}
				if(showwin == 400) {
					lighton(win);
					showwin = 0;
					continue;
				}
			}
			if(t > 0 && t != tkey && win == 0 && step == 0){
				if(dokey(t) == 0){
					t11 = 1;
					win = checktowinuser();	
					Delay1ms();					
					step = 1;
					continue;
				}
			}
			if(win == 0 && step == 1){
				win = brain();
				step = 0;
			}
			tkey = t;	
		}
}

char show(){
	char key = 0;
	P1 = 0xf9;
	P5 = 0xff;
	Delay1ms();
	if(L[1] == 1) P13 = 0;
	if(L[4] == 1) P14 = 0;
	if(L[7] == 1) P15 = 0;
	if(red == 0){
		if(L[1] == 10) P16 = 0;
		if(L[4] == 10) P17 = 0;
		if(L[7] == 10) P54 = 0;
	}
	Delay1ms();
	if(++keyline == 1){ 
		if(P3 == 0x80) key = 1; 
		if(P3 == 0x40) key = 4;
		if(P3 == 0x20) key = 7;
		if(P3 == 0x10) key = 10;
	}
	P1 = 0xfa;
	P5 = 0xff;
	Delay1ms();
	if(L[2] == 1) P13 = 0;
	if(L[5] == 1) P14 = 0;
	if(L[8] == 1) P15 = 0;
	if(red == 0){
		if(L[2] == 10) P16 = 0;
		if(L[5] == 10) P17 = 0;
		if(L[8] == 10) P54 = 0;
	}
	Delay1ms();
	if(keyline == 2){ 
		if(P3 == 0x80) key = 2;
		if(P3 == 0x40) key = 5;
		if(P3 == 0x20) key = 8;
	}		
	P1 = 0xfc;
	P5 = 0xff;
	Delay1ms();
	if(L[3] == 1) P13 = 0;
	if(L[6] == 1) P14 = 0;
	if(L[9] == 1) P15 = 0;
	if(red == 0){
		if(L[3] == 10) P16 = 0;
		if(L[6] == 10) P17 = 0;
		if(L[9] == 10) P54 = 0;
	}
	if(++red == 5) red = 0;
	Delay1ms();
	if(keyline == 3){ 
		if(P3 == 0x80) key = 3;
		if(P3 == 0x40) key = 6;
		if(P3 == 0x20) key = 9;
		if(P3 == 0x10) key = 11;
		keyline = 0;
	}
	
	return key;
}

char dokey(char key){	
	if(key == 11){ // step
		return 0;
	}
	if(L[key] != 0) return key;
	L[key] = 1;
	return 0; 
}

char brain(){
	char win;	
	// check lines to 1 step to win
	win = checktowin();
	if(win > 0) return win;
	if(checkuser2() > 0) return 0; // check users lines of 2 bolls
	nextstep();
	return 0;
}

char checktowin(){
	if (getlinesum(3, 2, 1) == 20) return setwin(1);
	if (getlinesum(6, 5, 4) == 20) return setwin(2);
	if (getlinesum(9, 8, 7) == 20) return setwin(3);
	if (getlinesum(3, 6, 9) == 20) return setwin(4);
	if (getlinesum(2, 5, 8) == 20) return setwin(5);
	if (getlinesum(1, 4, 7) == 20) return setwin(6);
	if (getlinesum(3, 5, 7) == 20) return setwin(7);
	if (getlinesum(1, 5, 9) == 20) return setwin(8);
	return 0;
}

char checkuser2(){
	if (getlinesum(3, 2, 1) == 2) return set3(1); 
	if (getlinesum(6, 5, 4) == 2) return set3(2);
	if (getlinesum(9, 8, 7) == 2) return set3(3);
	if (getlinesum(3, 6, 9) == 2) return set3(4);
	if (getlinesum(2, 5, 8) == 2) return set3(5);
	if (getlinesum(1, 4, 7) == 2) return set3(6);
	if (getlinesum(3, 5, 7) == 2) return set3(7);
	if (getlinesum(1, 5, 9) == 2) return set3(8);
	return 0;
}

char set3(char i){	
	if(i < 4){
		if(L[i*3] == 0) L[i*3] = 10;
		if(L[i*3-1] == 0) L[i*3-1] = 10;
		if(L[i*3-2] == 0) L[i*3-2] = 10;
		return 1;
	}
	if(i < 7){
		if(L[13-i] == 0) L[13-i] = 10;
		if(L[10-i] == 0) L[10-i] = 10;
		if(L[7-i] == 0) L[7-i] = 10;
		return 1;
	}
	if(i==7){
		if(L[3] == 0) L[3] = 10;
		if(L[7] == 0) L[7] = 10;
		return 1;
	}
	if(i==8){
		if(L[1] == 0) L[1] = 10;
		if(L[9] == 0) L[9] = 10;
		return 1;
	}	
	return 1;
}

void nextstep(){
	char i;
	char j;
	if(begin == 5){
		if(L[5] == 0){
			L[5] = 10; return;
		}
	}
	if(++begin > 9) begin = 1;
	j = begin;
	for(i = 1; i< 10; i++){		
		if(L[j] == 0){
			L[j] = 10;
			return;
		}
		if(++j > 9) j = 1;
	}
}

char setwin(char iswin){
	switch(iswin){
		case 1:
			L[3] = 10; L[2] = 10; L[1] = 10; 
			break;
		case 2:
			L[6] = 10; L[5] = 10; L[4] = 10;
			break;
		case 3:
			L[9] = 10; L[8] = 10; L[7] = 10;
			break;
		case 4:
			L[3] = 10; L[6] = 10; L[9] = 10;
			break;
		case 5:
			L[2] = 10; L[5] = 10; L[8] = 10;
			break;
		case 6:
			L[1] = 10; L[4] = 10; L[7] = 10;
			break;
		case 7:
			L[3] = 10; L[5] = 10; L[7] = 10;
			break;
		case 8:
			L[1] = 10; L[5] = 10; L[9] = 10;
			break;
	}
	return iswin;
}

char checktowinuser(){
	if (getlinesum(3, 2, 1) == 3) return 1;
	if (getlinesum(6, 5, 4) == 3) return 2;
	if (getlinesum(9, 8, 7) == 3) return 3;
	if (getlinesum(3, 6, 9) == 3) return 4;
	if (getlinesum(2, 5, 8) == 3) return 5;
	if (getlinesum(1, 4, 7) == 3) return 6;
	if (getlinesum(3, 5, 7) == 3) return 7;
	if (getlinesum(1, 5, 9) == 3) return 8;
	return 0;
}

char getlinesum(char a, char b, char c){
	return L[a] + L[b] + L[c];
}

void lightoff(char iswin){
	switch(iswin){
		case 1:
			L[3]++; L[2]++; L[1]++;
			return;
		case 2:
			L[6]++; L[5]++; L[4]++;
			return;
		case 3:
			L[9]++; L[8]++; L[7]++;
			return;
		case 4:
			L[3]++; L[6]++; L[9]++;
			return;
		case 5:
			L[2]++; L[5]++; L[8]++;
			return;
		case 6:
			L[1]++; L[4]++; L[7]++;
			return;
		case 7:
			L[3]++; L[5]++; L[7]++;
			return;
		case 8:
			L[1]++; L[5]++; L[9]++;
			return;
	}
}

void lighton(char iswin){
	switch(iswin){
		case 1:
			L[3]--; L[2]--; L[1]--;
			return;
		case 2:
			L[6]--; L[5]--; L[4]--;
			return;
		case 3:
			L[9]--; L[8]--; L[7]--;
			return;
		case 4:
			L[3]--; L[6]--; L[9]--;
			return;
		case 5:
			L[2]--; L[5]--; L[8]--;
			return;
		case 6:
			L[1]--; L[4]--; L[7]--;
			return;
		case 7:
			L[3]--; L[5]--; L[7]--;
			return;
		case 8:
			L[1]--; L[5]--; L[9]--;
			return;
	}
}