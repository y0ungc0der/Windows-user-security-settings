#include "Dll.h"

HINSTANCE Netapi32dllH = NULL;	// дескриптор экземпляра DLL
HINSTANCE Advapi32dllH = NULL;	// дескриптор экземпляра DLL

My_NetApiBufferFree          MyNetApiBufferFree;
My_NetUserEnum               MyNetUserEnum;
My_NetUserGetInfo			 MyNetUserGetInfo;
My_NetUserAdd                MyNetUserAdd;
My_NetUserDel                MyNetUserDel;
My_NetUserSetInfo			 MyNetUserSetInfo;
My_NetUserChangePassword	 MyNetUserChangePassword;
My_NetLocalGroupSetMembers	 MyNetLocalGroupSetMembers;
My_NetLocalGroupDelMembers	 MyNetLocalGroupDelMembers;
My_NetLocalGroupAddMembers	 MyNetLocalGroupAddMembers;
My_NetLocalGroupSetInfo		 MyNetLocalGroupSetInfo;
My_NetLocalGroupDel          MyNetLocalGroupDel;
My_NetLocalGroupAdd          MyNetLocalGroupAdd;
My_NetLocalGroupEnum		 MyNetLocalGroupEnum;
My_NetGroupEnum				 MyNetGroupEnum;
My_NetGroupGetInfo			 MyNetGroupGetInfo;

My_LookupPrivilegeValueA	 MyLookupPrivilegeValueA;
My_LookupAccountNameW		 MyLookupAccountNameW;
My_ConvertSidToStringSidA    MyConvertSidToStringSidA;
My_AdjustTokenPrivileges     MyAdjustTokenPrivileges;
My_LsaRemoveAccountRights    MyLsaRemoveAccountRights;
My_LsaAddAccountRights       MyLsaAddAccountRights;
My_LsaEnumerateAccountRights MyLsaEnumerateAccountRights;
My_LsaNtStatusToWinError     MyLsaNtStatusToWinError;
My_OpenProcessToken			 MyOpenProcessToken;
My_LsaOpenPolicy             MyLsaOpenPolicy;

void UploadMyDll()
{
	// загружаем динамически подключаемую библиотеку
	Netapi32dllH = LoadLibrary(TEXT("netapi32.dll"));
	// настраиваем адреса функций
	MyNetApiBufferFree = (My_NetApiBufferFree)GetProcAddress(Netapi32dllH, "NetApiBufferFree");
	MyNetUserEnum = (My_NetUserEnum)GetProcAddress(Netapi32dllH, "NetUserEnum");
	MyNetUserGetInfo = (My_NetUserGetInfo)GetProcAddress(Netapi32dllH, "NetUserGetInfo");
	MyNetUserAdd = (My_NetUserAdd)GetProcAddress(Netapi32dllH, "NetUserAdd");
	MyNetUserDel = (My_NetUserDel)GetProcAddress(Netapi32dllH, "NetUserDel");
	MyNetUserSetInfo = (My_NetUserSetInfo)GetProcAddress(Netapi32dllH, "NetUserSetInfo");
	MyNetUserChangePassword = (My_NetUserChangePassword)GetProcAddress(Netapi32dllH, "NetUserChangePassword");
	MyNetLocalGroupSetMembers = (My_NetLocalGroupSetMembers)GetProcAddress(Netapi32dllH, "NetLocalGroupSetMembers");
	MyNetLocalGroupDelMembers = (My_NetLocalGroupDelMembers)GetProcAddress(Netapi32dllH, "NetLocalGroupDelMembers");
	MyNetLocalGroupAddMembers = (My_NetLocalGroupAddMembers)GetProcAddress(Netapi32dllH, "NetLocalGroupAddMembers");
	MyNetLocalGroupSetInfo = (My_NetLocalGroupSetInfo)GetProcAddress(Netapi32dllH, "NetLocalGroupSetInfo");
	MyNetLocalGroupDel = (My_NetLocalGroupDel)GetProcAddress(Netapi32dllH, "NetLocalGroupDel");
	MyNetLocalGroupAdd = (My_NetLocalGroupAdd)GetProcAddress(Netapi32dllH, "NetLocalGroupAdd");
	MyNetLocalGroupEnum = (My_NetLocalGroupEnum)GetProcAddress(Netapi32dllH, "NetLocalGroupEnum");
	MyNetGroupEnum = (My_NetGroupEnum)GetProcAddress(Netapi32dllH, "NetGroupEnum");
	MyNetGroupGetInfo = (My_NetGroupGetInfo)GetProcAddress(Netapi32dllH, "NetGroupGetInfo");
	
	// загружаем динамически подключаемую библиотеку	
	Advapi32dllH = LoadLibrary(TEXT("advapi32.dll"));
	// настраиваем адреса функций
	MyLookupPrivilegeValueA = (My_LookupPrivilegeValueA)GetProcAddress(Advapi32dllH, "LookupPrivilegeValueA");
	MyLookupAccountNameW = (My_LookupAccountNameW)GetProcAddress(Advapi32dllH, "LookupAccountNameW");
	MyConvertSidToStringSidA = (My_ConvertSidToStringSidA)GetProcAddress(Advapi32dllH, "ConvertSidToStringSidA");
	MyAdjustTokenPrivileges = (My_AdjustTokenPrivileges)GetProcAddress(Advapi32dllH, "AdjustTokenPrivileges");
	MyLsaRemoveAccountRights = (My_LsaRemoveAccountRights)GetProcAddress(Advapi32dllH, "LsaRemoveAccountRights");
	MyLsaAddAccountRights = (My_LsaAddAccountRights)GetProcAddress(Advapi32dllH, "LsaAddAccountRights");
	MyLsaEnumerateAccountRights = (My_LsaEnumerateAccountRights)GetProcAddress(Advapi32dllH, "LsaEnumerateAccountRights");
	MyLsaNtStatusToWinError = (My_LsaNtStatusToWinError)GetProcAddress(Advapi32dllH, "LsaNtStatusToWinError");
	MyOpenProcessToken = (My_OpenProcessToken)GetProcAddress(Advapi32dllH, "OpenProcessToken");
	MyLsaOpenPolicy = (My_LsaOpenPolicy)GetProcAddress(Advapi32dllH, "LsaOpenPolicy");
}																																	   

void OffloadMyDll()
{
	// отключаем библиотеки
	FreeLibrary(Netapi32dllH);
	FreeLibrary(Advapi32dllH);
}