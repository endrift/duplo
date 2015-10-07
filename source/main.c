#include <fat.h>
#include <nds.h>
#include <stdio.h>

void writeChange(const u32* buffer) {
	// Input registers are at 0x08800000 - 0x088001FF
	*(vu32*) 0x08800184 = buffer[1];
	*(vu32*) 0x08800188 = buffer[2];
	*(vu32*) 0x0880018C = buffer[3];

	*(vu32*) 0x08800180 = buffer[0];
}

void readChange(void) {
	// Output registers are at 0x08800100 - 0x088001FF
	while (*(vu32*) 0x08000180 & 0x1000); // Busy bit
}

void dump(void) {
	char name[13] = "";
	char code[5] = "";
	strncpy(name, (void*) 0x080000A0, 12);
	memcpy(code, (void*) 0x080000AC, 4);
	char fname[23];
	snprintf(fname, sizeof(fname), "/%s-%s.gba", name, code);
	printf("Opening %s.\n", fname);
	FILE* f = fopen(fname, "wb");

	u32 cmd[4] = {
		0x11, // Command
		0, // ROM address
		0x08001000, // Virtual address
		0x8, // Size (in 0x200 byte blocks)
	};

	size_t i;
	for (i = 0x0; i < 0x04000000; i += 0x1000) {
		printf("Dumping %07X...\n", i);
		cmd[1] = i,
		writeChange(cmd);
		readChange();
		fwrite(GBAROM + (0x1000 >> 1), 0x1000, 1, f);
	}

	printf("Done!\n");
	fclose(f);
}

int main(void) {
	consoleDemoInit();
	fatInitDefault();

	videoSetMode(MODE_FB0);

	vramSetBankA(VRAM_A_LCD);

	sysSetCartOwner(1);

	dump();

	while (1) {
		swiWaitForVBlank();
	}

	return 0;
}
