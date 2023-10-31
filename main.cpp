#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <memory>
#include <string_view>
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <random>
#include "xor.hpp"
#include "DriverRW.h"
#include "Tools.h"
using namespace std;

uintptr_t Pid;
uintptr_t BaseAddress;

struct GlowMode
{
	int8_t GeneralGlowMode, BorderGlowMode, BorderSize, TransparentLevel;
};

DWORD64 GetEntityById(int Ent, DWORD64 Base)
{
	DWORD64 EntityList = Base + 0x1974ad8;
	DWORD64 BaseEntity = read(EntityList, DWORD64);

	if (!BaseEntity)
		return NULL;

	return read(EntityList + (Ent << 5), DWORD64);
}

int main(int argCount, char** argVector)
{
	srand(time(NULL));
	std::string filePath = argVector[0];
	RenameFile(filePath);

	printf(E("[+] 正在获取游戏进程... \n"));

	while (!get_process_pid(E("r5apex.exe"))) Sleep(1000);

	Pid = get_process_pid(E("r5apex.exe"));
	printf(E("[+] Pid：%d \n"), Pid);

	if (!kernelHandler.attach("r5apex.exe"))
	{
		MessageBox(0, E("附加到游戏进程失败"), E("错误"), MB_OK | MB_ICONERROR);
		return 1;
	}

	//获取基模块地址
	while (BaseAddress == 0)
	{
		BaseAddress = kernelHandler.get_module_base(E("r5apex.exe"));
		Sleep(1000);
	}
	printf(E("[+] 基地址：%p \n"), BaseAddress);

	while (true)
	{
		for (int i = 0; i < 100; i++)
		{
			DWORD64 Entity = GetEntityById(i, BaseAddress);
			if (Entity == 0)
			continue;
			DWORD64 EntityHandle = read(Entity + 0x589, DWORD64); //m_iName
			std::string Identifier = read(EntityHandle, std::string);
			LPCSTR IdentifierC = Identifier.c_str();
			GlowMode glowMode = { 101,101,46,90 };
			if (strcmp(IdentifierC, E("player")))
			{
				write(Entity + 0x3C8, 1, int);
				write(Entity + 0x3D0, 2, int);
				write(Entity + 0x2C0, glowMode, GlowMode);
				write(Entity + 0x1D0, 61.f, float);
				write(Entity + 0x1D4, 2.f, float);
				write(Entity + 0x1D8, 2.f, float);
			}
		}
	}
}