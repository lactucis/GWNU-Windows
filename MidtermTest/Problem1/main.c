#include <stdio.h>

//PassOrFail �Լ� ����
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
        printf("����� �����ϴ�!\n");
    }
    else {
        printf("�츮�� ���ߴ١�  ������̴١�\n");
    }

    return 0;
}
