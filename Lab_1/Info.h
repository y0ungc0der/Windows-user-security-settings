#pragma once
#pragma warning(disable : 4996)

LSA_HANDLE GetPolicyHandle();
bool InitLsaString(PLSA_UNICODE_STRING, LPCWSTR);

int UserGetInfoByName(LSA_HANDLE, LPWSTR);
int UserChangePassword();
int UserChangeName();
int UserDelete();
int NewUserCreate(LSA_HANDLE);
int GetUsersList(LSA_HANDLE);

int GroupSetMembers();
int GroupDelMemberN();
int GroupDelMemberID();
int GroupAddMemberN();
int GroupAddMemberID();
int GroupChangeName();
int GroupDelete();
int NewGroupCreate();
int GetLocalGroupsList(LSA_HANDLE);
int GetGlobalGroupsList(LSA_HANDLE);
void GetGroupsList(LSA_HANDLE);

int EnDisPrivileges();
int UserPriv(LSA_HANDLE, bool);
int GroupPriv(LSA_HANDLE, bool);
void AddPrivileges(PSID, LSA_HANDLE, LPCWSTR);
void DelPrivileges(PSID, LSA_HANDLE, LPCWSTR);
void ShowPrivileges(LSA_HANDLE, PSID);
BOOL SetPrivilege(HANDLE, LPCTSTR, BOOL);