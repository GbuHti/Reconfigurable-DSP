#define TERMINAL_ADDR 0x20000000
int sum(int end);

int x = 5;
int y;

int main() {
	int a = 6;
	int b = 7;
	a = a + b + x + y;
	a = sum(a);
	*(char*)(TERMINAL_ADDR) = 'X';
	return a;
}
