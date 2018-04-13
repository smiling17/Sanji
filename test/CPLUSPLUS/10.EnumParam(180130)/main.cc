#include <iostream>
#include <string>

enum {
	Normal = 0,
	Secure,
	Current,
} Zone;

typedef enum {
	ZONE_NORMAL = 0,
	ZONE_SECURE,
	ZONE_CURRNET,
	ZONE_SUPER,
} ZONE_TOSNET;

void PrintZone(ZONE_TOSNET zone) {
	fprintf(stdout, "ZONE : %d\n", zone);
	return;
}

void PrintZone2(int zone) {
	fprintf(stdout, "ZONE 2 : %d\n", zone);
	return;
}

enum class MyZone {
	MyNormal = 0,
	MySecure,
	MyCurrnet,
};

int main() {
	fprintf(stdout, "===== PrintZone TEST =====\n");
	PrintZone(ZONE_SECURE);
//	PrintZone(0); /* error : type */
//	PrintZone(Normal); /* error : type */
	fprintf(stdout, "===== PrintZone TEST2 =====\n");
	PrintZone2(Normal);
	PrintZone2(1);
	PrintZone2(ZONE_SECURE);
//	PrintZone2(MyZone::MyNormal); /* error : type error */
//	PrintZone2(MyNormal); /* error : not found */
	return 0;
}
