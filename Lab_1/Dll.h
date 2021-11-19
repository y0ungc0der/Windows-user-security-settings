#pragma once

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <wchar.h>
#include <assert.h>
#include <lm.h>
#include <sddl.h>
#include <iostream>
#include <conio.h>
#include <string>
#include <ntstatus.h>
#include <ntsecapi.h>
#include <tchar.h>
#include <lmcons.h>
#include <lmaccess.h>
#include <lmerr.h>
#include <lmapibuf.h>

typedef NET_API_STATUS(_stdcall *My_NetApiBufferFree)(
	LPVOID Buffer
	);

typedef NET_API_STATUS(_stdcall *My_NetUserEnum)(
	LPCWSTR servername,
	DWORD   level,
	DWORD   filter,
	LPBYTE  *bufptr,
	DWORD   prefmaxlen,
	LPDWORD entriesread,
	LPDWORD totalentries,
	LPDWORD resume_handle
	);

typedef NET_API_STATUS(_stdcall *My_NetUserSetInfo)(
	LPCWSTR servername,
	LPCWSTR username,
	DWORD   level,
	LPBYTE  buf,
	LPDWORD parm_err
	);

typedef NET_API_STATUS(_stdcall *My_NetLocalGroupSetMembers)(
	LPCWSTR servername,
	LPCWSTR groupname,
	DWORD   level,
	LPBYTE  buf,
	DWORD   totalentries
	);

typedef NET_API_STATUS(_stdcall *My_NetLocalGroupSetInfo)(
	LPCWSTR servername,
	LPCWSTR groupname,
	DWORD   level,
	LPBYTE  buf,
	LPDWORD parm_err
	);

typedef NET_API_STATUS(_stdcall *My_NetUserAdd)(
	LMSTR   servername,
	DWORD   level,
	LPBYTE  buf,
	LPDWORD parm_err
	);

typedef NET_API_STATUS(_stdcall *My_NetUserDel)(
	LPCWSTR servername,
	LPCWSTR username
	);

typedef NET_API_STATUS(_stdcall *My_NetLocalGroupAddMembers)(
	LPCWSTR servername,
	LPCWSTR groupname,
	DWORD   level,
	LPBYTE  buf,
	DWORD   totalentries
	);

typedef NET_API_STATUS(_stdcall *My_NetLocalGroupDelMembers)(
	LPCWSTR servername,
	LPCWSTR groupname,
	DWORD   level,
	LPBYTE  buf,
	DWORD   totalentries
	);

typedef NET_API_STATUS(_stdcall *My_NetLocalGroupAdd)(
	LPCWSTR servername,
	DWORD   level,
	LPBYTE  buf,
	LPDWORD parm_err
	);

typedef NET_API_STATUS(_stdcall *My_NetLocalGroupDel)(
	LPCWSTR servername,
	LPCWSTR groupname
	);

typedef NET_API_STATUS(_stdcall *My_NetGroupEnum)(
	LPCWSTR    servername,
	DWORD      level,
	LPBYTE     *bufptr,
	DWORD      prefmaxlen,
	LPDWORD    entriesread,
	LPDWORD    totalentries,
	PDWORD_PTR resume_handle
	);

typedef NET_API_STATUS(_stdcall *My_NetGroupGetInfo)(
	LPCWSTR servername,
	LPCWSTR groupname,
	DWORD   level,
	LPBYTE  *bufptr
	);

typedef NET_API_STATUS(_stdcall *My_NetLocalGroupEnum)(
	LPCWSTR    servername,
	DWORD      level,
	LPBYTE     *bufptr,
	DWORD      prefmaxlen,
	LPDWORD    entriesread,
	LPDWORD    totalentries,
	PDWORD_PTR resumehandle
	);

typedef NET_API_STATUS(_stdcall * My_NetUserChangePassword)(
	LPCWSTR domainname,
	LPCWSTR username,
	LPCWSTR oldpassword,
	LPCWSTR newpassword
	);

typedef NET_API_STATUS(_stdcall * My_NetUserGetInfo)(
	LPCWSTR servername,
	LPCWSTR username,
	DWORD   level,
	LPBYTE  *bufptr
	);


typedef BOOL(WINAPI *My_ConvertSidToStringSidA)(
	PSID   Sid,
	LPTSTR *StringSid
	);

typedef BOOL(WINAPI *My_LookupAccountNameW)(
	LPCWSTR       lpSystemName,
	LPCWSTR       lpAccountName,
	PSID          Sid,
	LPDWORD       cbSid,
	LPWSTR        ReferencedDomainName,
	LPDWORD       cchReferencedDomainName,
	PSID_NAME_USE peUse
	);

typedef BOOL(WINAPI *My_AdjustTokenPrivileges)(
	HANDLE            TokenHandle,
	BOOL              DisableAllPrivileges,
	PTOKEN_PRIVILEGES NewState,
	DWORD             BufferLength,
	PTOKEN_PRIVILEGES PreviousState,
	PDWORD            ReturnLength
	);

typedef NTSTATUS(_stdcall *My_LsaAddAccountRights)(
	LSA_HANDLE          PolicyHandle,
	PSID                AccountSid,
	PLSA_UNICODE_STRING UserRights,
	ULONG               CountOfRights
	);

typedef NTSTATUS(_stdcall *My_LsaRemoveAccountRights)(
	LSA_HANDLE          PolicyHandle,
	PSID                AccountSid,
	BOOLEAN             AllRights,
	PLSA_UNICODE_STRING UserRights,
	ULONG               CountOfRights
	);

typedef ULONG(_stdcall *My_LsaNtStatusToWinError)(
	NTSTATUS Status
	);

typedef NET_API_STATUS(_stdcall *My_OpenProcessToken)(
	HANDLE  ProcessHandle,
	DWORD   DesiredAccess,
	PHANDLE TokenHandle
	);

typedef NET_API_STATUS(_stdcall *My_LookupPrivilegeValueA)(
	LPCSTR lpSystemName,
	LPCSTR lpName,
	PLUID  lpLuid
	);

typedef NTSTATUS(_stdcall *My_LsaOpenPolicy)(
	PLSA_UNICODE_STRING    SystemName,
	PLSA_OBJECT_ATTRIBUTES ObjectAttributes,
	ACCESS_MASK            DesiredAccess,
	PLSA_HANDLE            PolicyHandle
	);

typedef NTSTATUS(_stdcall *My_LsaEnumerateAccountRights)(
	LSA_HANDLE          PolicyHandle,
	PSID                AccountSid,
	PLSA_UNICODE_STRING *UserRights,
	PULONG              CountOfRights
	);


extern HINSTANCE Netapi32dllH;	// дескриптор экземпляра DLL
extern HINSTANCE Advapi32dllH;	// дескриптор экземпляра DLL

extern My_NetApiBufferFree          MyNetApiBufferFree;
extern My_NetUserEnum               MyNetUserEnum;
extern My_NetUserGetInfo			MyNetUserGetInfo;
extern My_NetUserAdd                MyNetUserAdd;
extern My_NetUserDel                MyNetUserDel;
extern My_NetUserSetInfo			MyNetUserSetInfo;
extern My_NetUserChangePassword		MyNetUserChangePassword;
extern My_NetLocalGroupSetMembers	MyNetLocalGroupSetMembers;
extern My_NetLocalGroupDelMembers	MyNetLocalGroupDelMembers;
extern My_NetLocalGroupAddMembers	MyNetLocalGroupAddMembers;
extern My_NetLocalGroupSetInfo		MyNetLocalGroupSetInfo;
extern My_NetLocalGroupDel          MyNetLocalGroupDel;
extern My_NetLocalGroupAdd          MyNetLocalGroupAdd;
extern My_NetLocalGroupEnum			MyNetLocalGroupEnum;
extern My_NetGroupEnum				MyNetGroupEnum;
extern My_NetGroupGetInfo			MyNetGroupGetInfo;

extern My_LookupPrivilegeValueA		MyLookupPrivilegeValueA;
extern My_LookupAccountNameW		MyLookupAccountNameW;
extern My_ConvertSidToStringSidA    MyConvertSidToStringSidA;
extern My_AdjustTokenPrivileges     MyAdjustTokenPrivileges;
extern My_LsaRemoveAccountRights    MyLsaRemoveAccountRights;
extern My_LsaAddAccountRights       MyLsaAddAccountRights;
extern My_LsaEnumerateAccountRights MyLsaEnumerateAccountRights;
extern My_LsaNtStatusToWinError     MyLsaNtStatusToWinError;
extern My_OpenProcessToken			MyOpenProcessToken;
extern My_LsaOpenPolicy             MyLsaOpenPolicy;

void UploadMyDll();
void OffloadMyDll();