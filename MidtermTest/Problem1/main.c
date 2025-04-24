#include <stdio.h>

//PassOrFail 함수 정의
int PassOrFail(int score) {
    if (score >= 50) {
        return 1;
    }
    else {
        return 0;
    }
}

int main() {
    int myScore = 50;

    if (PassOrFail(myScore)) {
        printf("재시험 없습니다!\n");
    }
    else {
        printf("우리는 망했다…  재시험이다…\n");
    }

    return 0;
}
