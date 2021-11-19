#include "Main.h"
#include "Info.h"

using namespace std;

bool InitLsaString(PLSA_UNICODE_STRING pLsaString, LPCWSTR pwszString)
{
	DWORD dwLen = 0;

	if (NULL == pLsaString)
		return FALSE;

	if (NULL != pwszString)
	{
		dwLen = wcslen(pwszString);
		if (dwLen > 0x7ffe)   // Слишком большая строка
			return FALSE;
	}

	// Сохраняем строку.
	pLsaString->Buffer = (WCHAR *)pwszString;
	pLsaString->Length = (USHORT)dwLen * sizeof(WCHAR);
	pLsaString->MaximumLength = (USHORT)(dwLen + 1) * sizeof(WCHAR);

	return TRUE;
}

/* Получаем информацио о пользователе по его имени. */
int UserGetInfoByName(LSA_HANDLE handle, LPWSTR user_name)
{
	USER_INFO_4 *ui;			// информация о пользователе
	NET_API_STATUS ret_status;	// код возврата из функции

	ret_status = MyNetUserGetInfo(
		NULL,			// используется локальный компьютер
		user_name,		// имя пользователя
		4,				// уровень информации 4
		(LPBYTE*)&ui);	// адрес информации о пользователе

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("Не удалось получить информацию о пользователе.\n");
		return ret_status;
	}

	wprintf(L"Имя пользователя: %s\n", ui->usri4_name);
	wprintf(L"Комментарий: %s\n", ui->usri4_comment);

	LPSTR s_user_sid;
	// Преобразуем SID в строковой вид
	if (!MyConvertSidToStringSidA(ui->usri4_user_sid, &s_user_sid))
	{
		printf("Невозможно определить SID пользователя.");
		return NULL;
	}

	printf("SID пользователя: %s\n", s_user_sid);
	if (ui->usri4_priv == USER_PRIV_GUEST) wprintf(L"Уровень привилегий: Guest\n");
	else if (ui->usri4_priv == USER_PRIV_USER) wprintf(L"Уровень привилегий: User\n");
	else if (ui->usri4_priv == USER_PRIV_ADMIN) wprintf(L"Уровень привилегий: Administrator\n");
	ShowPrivileges(handle, ui->usri4_user_sid);

	MyNetApiBufferFree(ui); // освобождаем буфер
	return 0;
}

/* Изменяем пароль пользователя. */
int UserChangePassword()
{
	wchar_t user_name[UNLEN];		// имя пользователя
	wchar_t old_password[PWLEN];	// старый пароль пользователя
	wchar_t new_password[PWLEN];	// новый пароль пользователя
	NET_API_STATUS ret_status;		// код возврата из функции

	printf("Введите имя пользователя, для которого нужно изменить пароль: ");
	wscanf(L"%s", user_name);		// читаем имя пользователя

	printf("Введите старый пароль: ");
	wscanf(L"%s", old_password);	// читаем старый пароль пользователя

	printf("Введите новый пароль: ");
	wscanf(L"%s", new_password);	// читаем новый пароль пользователя

	// Изменяем пароль пользователя
	ret_status = MyNetUserChangePassword(
		NULL,			// используется локальный компьютер
		user_name,		// имя пользователя
		old_password,	// старый пароль
		new_password);	// новый пароль

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("Не удалось изменить пароль пользователя.\n");
		printf("Статус сетевого API: %d\n\n", ret_status);
		return ret_status;
	}
	printf("Пароль был успешно изменен.\n");

	return 0;
}

/* Изменяем имя пользователя. */
int UserChangeName()
{
	wchar_t old_name[UNLEN];	// старое имя пользователя
	wchar_t new_name[UNLEN];	// новое имя пользователя
	USER_INFO_0 ui;				// информация о пользователе
	NET_API_STATUS ret_status;	// код возврата из функции

	printf("Введите старое имя пользователя: ");
	wscanf(L"%s", old_name);	// читаем старое имя пользователя

	printf("Введите новое имя пользователя: ");
	wscanf(L"%s", new_name);	// читаем новое имя пользователя
	ui.usri0_name = new_name;	// устанавливаем новое имя пользователя

	// Устанавливаем информацию о пользователе
	ret_status = MyNetUserSetInfo(
		NULL,			// используется локальный компьютер
		old_name,		// имя пользователя
		0,				// изменяем имя пользователя
		(LPBYTE)&ui,	// адрес информации о пользователе
		NULL);			// нет индексации

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("Не удалось ввести информацию о пользователе.\n");
		printf("Статус сетевого API: %d\n\n", ret_status);
		return ret_status;
	}
	printf("Имя пользователя успешно изменено:\n");

	USER_INFO_1 *uix; // информация о пользователе

	// Получаем информацию о пользователе
	ret_status = MyNetUserGetInfo(
		NULL,			// используется локальный компьютер
		new_name,		// имя пользователя
		1,				// вернуть подробную информацию об учетной записи пользователя
		(LPBYTE*)&uix); // адрес информации о пользователе

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("Не удалось получить информацию о пользователе.\n");
		return ret_status;
	}
	wprintf(L"Имя пользователя: %s\n", uix->usri1_name);
	wprintf(L"Комментарий пользователя: %s\n", uix->usri1_comment);

	return 0;
}

/* Удаляем учетную запись пользователя. */
int UserDelete()
{
	wchar_t user_name[UNLEN];	// имя пользователя
	NET_API_STATUS ret_status;	// код возврата из функции

	printf("Введите имя пользователя для удаления: ");
	wscanf(L"%s", user_name);	// читаем имя пользователя

	// Удалаем пользователя
	ret_status = MyNetUserDel(
		NULL,		// используется локальный компьютер
		user_name); // имя пользователя

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("\nОшибка при удалении пользователя.\n");
		printf("Статус сетевого API: %d\n\n", ret_status);
		return ret_status;
	}

	printf("Пользователь успешно удален.\n");
	return 0;
}

/* Добавляем учетную запись нового пользователя. */
int NewUserCreate(LSA_HANDLE handle)
{
	wchar_t user_name[UNLEN];		// имя пользователя
	wchar_t user_password[PWLEN];	// пароль пользователя
	wchar_t user_comment[256];		// комментарии о пользователе
	USER_INFO_1 ui;					// информация о пользователе
	NET_API_STATUS ret_status;		// код возврата из функции

	printf("Введите имя нового пользователя: ");	// читаем имя пользователя
	wscanf(L"%s", user_name);						// читаем имя пользователя
	ui.usri1_name = user_name;						// устанавливаем имя пользователя

	printf("Введите пароль пользователя: ");
	wscanf(L"%s", user_password);		// читаем пароль пользователя
	ui.usri1_password = user_password;	// устанавливаем пароль пользователя

	ui.usri1_priv = USER_PRIV_USER; // обычный пользователь
	ui.usri1_home_dir = NULL;		// домашнего каталога нет

	printf("Введите комментарий пользователя: ");
	getwchar(); // очищаем поток
	wcin >> user_comment; // читаем комментарии о пользователе
	ui.usri1_comment = user_comment; // устанавливаем комментарии

	ui.usri1_flags = UF_SCRIPT;		// исполнять скрипт при входе пользователя в систему
	ui.usri1_script_path = NULL;	// пока файл со скриптом не определяем 


	// Добавляем пользователя (работает только если запускать программу от имени администратора)
	ret_status = MyNetUserAdd(
		NULL,				// используется локальный компьютер
		1,					// уровень информации 1
		(LPBYTE)&ui,		// адрес информации о пользователе
		NULL);				// индексирования в структуре данных нет

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("Не удалось добавить пользователя.\n");
		printf("Код ошибки %u\n", ret_status);
		return ret_status;
	}

	printf("Пользователь успешно добавлен:\n");
	UserGetInfoByName(handle, user_name);

	return 0;
}

/* Получаем список всех пользователей. */
int GetUsersList(LSA_HANDLE handle)
{
	USER_INFO_0 *ui;			// информация о пользователе
	DWORD entries_read;			// количество прочитанных пользователей
	DWORD total_entries;		// общее количество пользователей
	NET_API_STATUS ret_status;	// код возврата из функции

	// Получаем информацию о пользователях
	ret_status = MyNetUserEnum(
		NULL,					// используется локальный компьютер
		0,						// узнаем только имена пользователей
		FILTER_NORMAL_ACCOUNT,	// перечисляем пользователей, зарегистрированных на компьютере
		(LPBYTE*)&ui,			// адрес информации о пользователях
		MAX_PREFERRED_LENGTH,	// перечисляем всех пользователей
		&entries_read,			// количество прочитанных пользователей
		&total_entries,			// общее количество пользователей
		NULL);					// индексации нет

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("Не удалось получить информацию о пользователях.\n");
		printf("Статус сетевого API: %d\n", ret_status);
		MyNetApiBufferFree(ui); // освобождаем буфер
		return ret_status;
	}

	for (DWORD i = 0; i < entries_read; ++i)
	{
		UserGetInfoByName(handle, ui[i].usri0_name);
	}

	MyNetApiBufferFree(ui); // освобождаем буфер
	return 0;
}

/* Устанавливаем членов локальной группы. */
int GroupSetMembers()
{
	wchar_t group_name[GNLEN];				// имя локальной группы
	LOCALGROUP_MEMBERS_INFO_3 *member_info; // информация о членах группы
	DWORD n;								// количество остающихся членов локальной группы
	NET_API_STATUS ret_status;				// код возврата из функции

	printf("Введите имя локальной группы: ");
	wscanf(L"%s", group_name); // вводим имя группы

	printf("Введите количество остающихся членов группы: ");
	scanf("%d", &n);

	// Захватываем память для информации о пользователях
	member_info = new LOCALGROUP_MEMBERS_INFO_3[n];
	for (DWORD i = 0; i < n; ++i)
	{
		// захватываем память под имя пользователя
		member_info[i].lgrmi3_domainandname = new wchar_t[256];
		printf("Введите имя домена: ");
		// вводим имя домена
		wscanf(L"%s", member_info[i].lgrmi3_domainandname);

		// присоединяем символ '\'
		wcscat(member_info[i].lgrmi3_domainandname, L"\\");

		printf("Введите имя пользователя: ");
		// вводим имя пользователя
		wscanf(L"%s", member_info[i].lgrmi3_domainandname +
			wcslen(member_info[i].lgrmi3_domainandname));
	}

	// Вводим имя домена
	for (int i = 0; i < n; ++i)
		wprintf(L"%s\n", member_info[i].lgrmi3_domainandname);

	// Устанавливаем членов в локальной группе
	ret_status = MyNetLocalGroupSetMembers(
		NULL,					// используется локальный компьютер
		group_name,				// имя группы
		3,						// задает учетную запись и доменные имена члена локальной группы
		(LPBYTE)member_info,	// информация о SID
		n);						// количество остающихся членов группы

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось устанавить членов локальной группы.\n");
		return ret_status;
	}
	printf("Члены локальной группы успешно установлены.\n");

	return 0;
}

/* Удаляет члена локальной группы по имени. */
int GroupDelMemberN()
{
	wchar_t group_name[GNLEN]; // имя локальной группы
	wchar_t user_name[UNLEN]; // имя пользователя
	LOCALGROUP_MEMBERS_INFO_3 member_info; // информация о члене группы
	NET_API_STATUS ret_status; // код возврата из функции

	printf("Введите имя локальной группы: ");
	wscanf(L"%s", group_name); // вводим имя группы

	printf("Введите доменное имя: ");
	wscanf(L"%s", user_name); // вводим имя домена
	wcscat(user_name, L"\\"); // присоединяем символ '\'

	printf("Введите имя пользователя: ");
	// вводим имя пользователя
	wscanf(L"%s", user_name + wcslen(user_name));
	// устанавливаем информацию о пользователе
	member_info.lgrmi3_domainandname = user_name;

	// Удаляем пользователя из локальной группы
	ret_status = MyNetLocalGroupDelMembers(
		NULL,					// используется локальный компьютер
		group_name,				// имя группы
		3,						// указывает домен и имя члена локальной группы для удаления
		(LPBYTE)&member_info,	// информация о SID
		1);						// удаляем одного члена группы

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось удалить член локальной группы.\n");
		return ret_status;
	}
	printf("Член удален.\n");

	return 0;
}

/* Удаляет члена локальной группы по идентификатору безопасности. */
int GroupDelMemberID()
{
	wchar_t group_name[GNLEN];				// имя локальной группы
	wchar_t user_name[UNLEN];				// имя пользователя
	LOCALGROUP_MEMBERS_INFO_0 member_info;	// информация о члене группы
	NET_API_STATUS ret_status;				// код возврата из функции

	DWORD dwErrCode;
	DWORD dwLengthOfSID = 0;		// длина SID
	DWORD dwLengthOfDomainName = 0; // длина имени домена
	PSID lpSID = NULL;				// указатель на SID
	PWCHAR lpDomainName = NULL;				// указатель на имя домена
	SID_NAME_USE type_of_SID;		// тип учетной записи

	printf("Введите имя локальной группы: ");
	wscanf(L"%s", group_name); // вводим имя группы

	printf("Введите имя пользователя: ");
	wscanf(L"%s", user_name);	// вводим имя пользователя

	// Определяем длину SID пользователя
	MyLookupAccountNameW(
		NULL,					// ищем имя на локальном компьютере
		(LPCWSTR)user_name,		// имя пользователя
		NULL,					// определяем длину SID
		&dwLengthOfSID,			// длина SID
		NULL,					// определяем имя домена
		&dwLengthOfDomainName,	// длина имени домена
		&type_of_SID);			// тип учетной записи

	// Проверяем, вернула ли функция длину SID
	if (dwLengthOfSID == 0)
	{
		dwErrCode = GetLastError();
		printf("\nНе удалось найти имя учетной записи. DDD\n");
		printf("Код ошибки: %d\n", dwErrCode);
		return dwErrCode;
	}

	// распределяем память для SID и имени домена
	lpSID = (PSID) new char[dwLengthOfSID];
	lpDomainName = (PWCHAR)malloc(dwLengthOfDomainName + 10);

	// Определяем SID и имя домена пользователя
	if (!MyLookupAccountNameW(
		NULL,					// ищем имя на локальном компьютере
		(LPCWSTR)user_name,		// имя пользователя
		lpSID,					// определяем длину SID
		&dwLengthOfSID,			// длина SID
		lpDomainName,			// определяем имя домена
		&dwLengthOfDomainName,	// длина имени домена
		&type_of_SID))			// тип учетной записи
	{
		dwErrCode = GetLastError();
		printf("\nНе удалось найти имя учетной записи.\n");
		printf("Код ошибки: %d\n", dwErrCode);
		return dwErrCode;
	}

	// распечатываем имя домена
	wprintf(L"%s\n", lpDomainName);

	// устанавливаем SID в информацию о члене группы
	member_info.lgrmi0_sid = lpSID;

	// Удаляем члена локальной группы
	ret_status = MyNetLocalGroupDelMembers(
		NULL,					// используется локальный компьютер
		group_name,				// имя группы
		0,						// уровень информации
		(LPBYTE)&member_info,	// информация о SID
		1);						// удаляем одного члена группы

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось удалить член локальной группы.\n");
		return ret_status;
	}
	printf("Член удален.\n");

	return 0;
}

/* Добавляет члена локальной группы по его имени. */
int GroupAddMemberN()
{
	wchar_t group_name[GNLEN];				// имя локальной группы
	wchar_t user_name[UNLEN];				// имя пользователя
	LOCALGROUP_MEMBERS_INFO_3 member_info;	// информация о члене группы
	NET_API_STATUS ret_status;				// код возврата из функции

	printf("Введите имя группы: ");
	wscanf(L"%s", group_name); // вводим имя группы

	printf("Введите имя домена: ");
	wscanf(L"%s", user_name); // вводим имя домена
	wcscat(user_name, L"\\"); // присоединяем символ '\' 

	printf("Введите имя пользователя: ");
	wscanf(L"%s", user_name + wcslen(user_name)); // вводим имя пользователя
	member_info.lgrmi3_domainandname = user_name; // устанавливаем информацию о пользователе

	// Добавляем пользователя в локальную группу
	ret_status = MyNetLocalGroupAddMembers(
		NULL,					// используется локальный компьютер
		group_name,				// имя группы
		3,						// указывает домен и имя нового члена локальной группы
		(LPBYTE)&member_info,	// имя учетной записи
		1);						// добавляем одного члена группы

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось добавить нового участника в группу.\n");
		return ret_status;
	}
	printf("Новый участник добавлен в группу.\n");

	return 0;
}

/* Добавляет члена локальной группы по его идентификатору безопасности. */
int GroupAddMemberID()
{
	wchar_t group_name[GNLEN];				// имя локальной группы
	wchar_t user_name[UNLEN];				// имя пользователя
	LOCALGROUP_MEMBERS_INFO_0 member_info;	// информация о члене группы
	NET_API_STATUS ret_status;				// код возврата из функции

	DWORD dwErrCode;
	DWORD dwLengthOfSID = 0;				// длина SID
	DWORD dwLengthOfDomainName = 0;			// длина имени домена
	PSID lpSID = NULL;						// указатель на SID
	PWCHAR lpDomainName = NULL;				// указатель на имя домена
	SID_NAME_USE type_of_SID;				// тип учетной записи
	
	printf("Введите имя локальной группы: ");
	wscanf(L"%s", group_name); // вводим имя группы

	printf("Введите имя пользователя: ");
	wscanf(L"%s", user_name); // вводим имя пользователя

	// Определяем длину SID пользователя
	MyLookupAccountNameW(
		NULL,					// ищем имя на локальном компьютере
		(LPCWSTR)user_name,		// имя пользователя
		NULL,					// определяем длину SID
		&dwLengthOfSID,			// длина SID
		NULL,					// определяем имя домена
		&dwLengthOfDomainName,	// длина имени домена
		&type_of_SID);			// тип учетной записи

	// Проверяем, вернула ли функция длину SID
	if (dwLengthOfSID == 0)
	{
		dwErrCode = GetLastError();
		printf("\nНе удалось найти имя учетной записи.\n");
		printf("Код ошибки: %d\n", dwErrCode);
		return dwErrCode;
	}

	// Распределяем память для SID и имени домена
	lpSID = (PSID) new char[dwLengthOfSID];
	lpDomainName = (PWCHAR)malloc(dwLengthOfDomainName + 10);

	// Определяем SID и имя домена пользователя
	if (!MyLookupAccountNameW(NULL,	// ищем имя на локальном компьютере
		(LPCWSTR)user_name,			// имя пользователя
		lpSID,						// определяем длину SID
		&dwLengthOfSID,				// длина SID
		lpDomainName,				// определяем имя домена
		&dwLengthOfDomainName,		// длина имени домена
		&type_of_SID))				// тип учетной записи
	{
		dwErrCode = GetLastError();
		printf("Не удалось найти имя учетной записи.\n");
		printf("Код ошибки: %d\n", dwErrCode);
		return dwErrCode;
	}

	// Распечатываем имя домена
	wprintf(L"Имя домена: %s\n", lpDomainName);

	// Устанавливаем SID в информацию о члене группы
	member_info.lgrmi0_sid = lpSID;

	// Добавляем пользователя в локальную группу
	ret_status = MyNetLocalGroupAddMembers(
		NULL,					// используется локальный компьютер
		group_name,				// имя группы
		0,						// указывает идентификатор безопасности (SID) нового члена локальной группы 
		(LPBYTE)&member_info,	// информация о SID
		1);						// добавляем одного члена группы

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось добавить нового участника в группу.\n");
		return ret_status;
	}
	printf("Новый участник добавлен в группу.\n");

	return 0;
}

/* Изменяем имя локальной группы. */
int GroupChangeName()
{
	wchar_t old_name[GNLEN];		// старое имя локальной группы
	wchar_t new_name[GNLEN];		// новое имя локальной группы
	wchar_t comment[MAXCOMMENTSZ];	// новый комментарий
	LOCALGROUP_INFO_0 group_info_0; // информация об имени группы
	LOCALGROUP_INFO_1 group_info_1; // описание новой группы
	NET_API_STATUS ret_status;		// код возврата из функции

	printf("Введите старое имя локальной группы: ");
	wscanf(L"%s", old_name); // вводим старое имя группы

	printf("Введите новое имя локальной группы: ");
	wscanf(L"%s", new_name); // вводим новое имя группы
	group_info_0.lgrpi0_name = new_name; // устанавливаем адрес имени в структуру

	// Изменяем имя группы
	ret_status = MyNetLocalGroupSetInfo(
		NULL,					// используется локальный компьютер
		old_name,				// старое имя группы
		0,						// новое имя группы
		(LPBYTE)&group_info_0,	// новая информация о группе
		NULL);					// индексирования информации нет

	// Проверяем завершение функции
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось ввести информацию о группе.\n");
		return ret_status;
	}
	printf("Имя локальной группы успешно изменено.\n");

	printf("Ввод комментариев группы: ");
	getwchar();								// очищаем поток
	wcin >> comment;						// читаем комментарий о локальной группе
	group_info_1.lgrpi1_comment = comment;	// устанавливаем новый комментарий

	// Изменяем описание группы
	ret_status = MyNetLocalGroupSetInfo(
		NULL,					// используется локальный компьютер
		new_name,				// имя группы
		1,						// получаем имя группы и комментарий
		(LPBYTE)&group_info_1,	// новое описание группы
		NULL);					// индексирования информации нет

	// Проверяем завершение функции
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось ввести информацию о группе.\n");
		return ret_status;
	}
	printf("Описание локальной группы успешно изменено.\n");

	return 0;
}

/* Удаляем локальную группу. */
int GroupDelete()
{
	wchar_t group_name[GNLEN]; // имя удаляемой локальной группы
	NET_API_STATUS ret_status; // код возврата из функции

	printf("Введите имя локальной группы для удаления: ");
	wscanf(L"%s", group_name); // вводим имя группы

	// Удаляем локальную группу
	ret_status = MyNetLocalGroupDel(NULL, group_name);

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось удалить локальную группу.\n");
		printf("Код ошибки: %d\n\n", ret_status);
		return ret_status;
	}
	printf("Локальная группа удалена.\n");

	return 0;
}

/* Создаем новую локальную группу. */
int NewGroupCreate()
{
	wchar_t group_name[GNLEN];		// имя локальной группы
	wchar_t comment[MAXCOMMENTSZ];	// комментарий о группе
	LOCALGROUP_INFO_1 group_info;	// информация о группе
	NET_API_STATUS ret_status;		// код возврата из функции

	printf("Введите имя для локальной группы: ");
	wscanf(L"%s", group_name);				// вводим имя группы
	group_info.lgrpi1_name = group_name;	// устанавливаем адрес имени в структуру

	printf("Ввод комментариев группы: ");
	getwchar();								// очищаем поток
	wcin >> comment;						// читаем комментарий о локальной группе
	group_info.lgrpi1_comment = comment;	// устанавливаем комментарий

	// Создаем локальную группу
	ret_status = MyNetLocalGroupAdd(
		NULL,					// используется локальный компьютер
		1,						// уровень входных данных (имя группы и комментарий)
		(LPBYTE)&group_info,	// имя группы и комментарии (указатель на структуру LOCALGROUP_INFO_1)
		NULL);					// индексацию данных не используем

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось добавить локальную группу.\n\n");
		return ret_status;
	}
	printf("Группа создана.\n");

	return 0;
}

/* Получаем список всех локальных групп. */
int GetLocalGroupsList(LSA_HANDLE handle)
{
	DWORD entries_read;					// количество элементов
	DWORD total_entries;				// нумерация элементов
	LOCALGROUP_INFO_1 *buf_ptr = NULL;	// адрес буфера для данных
	NET_API_STATUS ret_status;			// код возврата из функции

	// Перечисляем локальные группы
	ret_status = MyNetLocalGroupEnum(
		NULL,				  // локальный компьютер
		1,					  // получаем имя группы и комментарий
		(LPBYTE*)&buf_ptr,	  // указатель на адрес буфера для данных
		MAX_PREFERRED_LENGTH, // длина буфера по требованию
		&entries_read,		  // указатель на количество элементов
		&total_entries,		  // указатель на нумерацию элементов
		NULL);

	// Проверяем завершение функции
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось провести вычисление локальных групп.\n");
		MyNetApiBufferFree(buf_ptr); // освобождаем буфер для данных
		return ret_status;
	}

	printf("Перечень локальных групп:\n");

	// Выводим на консоль имена локальных групп и комментарии, их SID и привилегии
	for (DWORD i = 0; i < entries_read; ++i)
	{
		DWORD dwErrCode;
		DWORD dwLengthOfSID = 0;		// длина SID
		DWORD dwLengthOfDomainName = 0; // длина имени домена
		PSID lpSID = NULL;				// указатель на SID
		PWCHAR lpDomainName = NULL;		// указатель на имя домена
		SID_NAME_USE type_of_SID;		// тип учетной записи

		// Определяем длину SID группы
		MyLookupAccountNameW(
			NULL,					// ищем имя на локальном компьютере
			buf_ptr[i].lgrpi1_name,	// имя группы
			NULL,					// определяем длину SID
			&dwLengthOfSID,			// длина SID
			NULL,					// определяем имя домена
			&dwLengthOfDomainName,	// длина имени домена
			&type_of_SID);			// тип учетной записи

		// Проверяем, вернула ли функция длину SID
		if (dwLengthOfSID == 0)
		{
			dwErrCode = GetLastError();
			printf("\nНе удалось найти имя учетной записи. (нет длины) \n");
			printf("Код ошибки: %d\n", dwErrCode);
			return dwErrCode;
		}

		// распределяем память для SID и имени домена
		lpSID = (PSID) new char[dwLengthOfSID];
		lpDomainName = (PWCHAR)malloc(dwLengthOfDomainName + 10);

		// Определяем SID и имя домена группы
		if (!MyLookupAccountNameW(
			NULL,					// ищем имя на локальном компьютере
			buf_ptr[i].lgrpi1_name,	// имя группы
			lpSID,					// определяем длину SID
			&dwLengthOfSID,			// длина SID
			lpDomainName,			// определяем имя домена
			&dwLengthOfDomainName,	// длина имени домена
			&type_of_SID))			// тип учетной записи
		{
			dwErrCode = GetLastError();
			printf("\nНе удалось найти имя учетной записи.\n");
			printf("Код ошибки: %d\n", dwErrCode);
			return dwErrCode;
		}

		LPSTR gr_user_sid;
		// Преобразуем SID в строковой вид
		if (!MyConvertSidToStringSidA(lpSID, &gr_user_sid))
		{
			printf("\nНевозможно определить SID локальной группы.\n");
			return NULL;
		}

		wprintf(L"Имя группы: %s\n", buf_ptr[i].lgrpi1_name);
		wprintf(L"Комментарий: %s\n", buf_ptr[i].lgrpi1_comment);
		printf("SID группы: %s\n", gr_user_sid);
		ShowPrivileges(handle, lpSID);
	}

	MyNetApiBufferFree(buf_ptr); // освобождаем буфер для данных

	return 0;
}

/* Получаем список всех глобальных групп. */
int GetGlobalGroupsList(LSA_HANDLE handle)
{
	DWORD entries_read;					// количество элементов
	DWORD total_entries;				// нумерация элементов
	GROUP_INFO_1 *buf_ptr = NULL;		// адрес буфера для данных
	NET_API_STATUS ret_status;			// код возврата из функции

	// Перечисляем глобальные группы
	ret_status = MyNetGroupEnum(
		NULL,				  // локальный компьютер
		1,					  // получаем имя группы и комментарий
		(LPBYTE*)&buf_ptr,	  // указатель на адрес буфера для данных
		MAX_PREFERRED_LENGTH, // длина буфера по требованию
		&entries_read,		  // указатель на количество элементов
		&total_entries,		  // указатель на нумерацию элементов
		NULL);

	// Проверяем завершение функции
	if ((ret_status != NERR_Success) || (ret_status == ERROR_MORE_DATA))
	{
		printf("\nНе удалось провести вычисление глобальных групп.\n");
		MyNetApiBufferFree(buf_ptr); // освобождаем буфер для данных
		return ret_status;
	}

	printf("Перечень глобальных групп:\n");

	// Выводим на консоль имена глобальных групп и комментарии, их SID и привилегии
	for (DWORD i = 0; i < entries_read; ++i)
	{
		GROUP_INFO_3 *buf_ptr3 = NULL; // адрес буфера для данных

		MyNetGroupGetInfo(
			NULL,					// локальный компьютер
			buf_ptr[i].grpi1_name,	// имя группы 
			3,						// вернуть подробную информацию о глобальной группе
			(LPBYTE*)&buf_ptr3		// указатель на адрес буфера для данных
		);

		LPSTR gr_user_sid;
		// Преобразуем SID в строковой вид
		if (!MyConvertSidToStringSidA(buf_ptr3->grpi3_group_sid, &gr_user_sid))
		{
			printf("\nНевозможно определить SID глобальной группы.");
			return NULL;
		}

		wprintf(L"Имя группы: %s\n", buf_ptr[i].grpi1_name);
		wprintf(L"Комментарий: %s\n", buf_ptr[i].grpi1_comment);
		printf("SID группы: %s\n", gr_user_sid);
		ShowPrivileges(handle, buf_ptr3->grpi3_group_sid);
	}

	MyNetApiBufferFree(buf_ptr); // освобождаем буфер для данных

	return 0;
}

/* Получаем список всех групп. */
void GetGroupsList(LSA_HANDLE handle)
{
	GetLocalGroupsList(handle);
	GetGlobalGroupsList(handle);
}

/* Включаем/выключаем привилегию. */
int EnDisPrivileges()
{
	BOOL bEnablePrivilege = FALSE;
	HANDLE hToken;
	BOOL ret_status;

	LPCTSTR priv_name = (LPCTSTR)malloc(256 * sizeof(CHAR));
	std::cout << "Выберите привилегию: ";
	scanf_s(TEXT("%s"), priv_name);

	std::cout << "Включить/выключить привилегию? Введите 0/1: ";
	std::cin >> bEnablePrivilege;

	// Открываем handle токена доступа для вызывающего процесса
	if (!MyOpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		wprintf(L"\nOpenProcessToken() не удалось %u\n", GetLastError());
		return FALSE;
	}
	else wprintf(L"OpenProcessToken() получил handle\n");

	/*
	DWORD pid;
	pid = GetCurrentProcessId();
	cout << pid << endl; */

	// Включаем/выключаем привилегию
	ret_status = SetPrivilege(hToken, priv_name, bEnablePrivilege);

	if (!ret_status)
	{
		wprintf(L"\nНе удалось включить/выключить привилегию, ошибка %u\n", GetLastError());
		return FALSE;
	}
	else wprintf(L"Удалось включить/выключить привилегию успешно\n");

	return 0;
}

/* Включаем/выключаем привилегии.*/
//	lpszPrivilege - имя привилегии для включения/выключения \
	bEnablePrivilege - включить или отключить привилегию 
BOOL SetPrivilege(HANDLE hToken, LPCTSTR lpszPrivilege, BOOL bEnablePrivilege)
{
	TOKEN_PRIVILEGES new_state;
	LUID luid;

	if (!MyLookupPrivilegeValueA(
		NULL,			// ищем привилегию на локальном компьютере
		lpszPrivilege,	// привилегия, которую ищем
		&luid))			// получает LUID привилегии
	{
		printf("\nОшибка LookupPrivilegeValue: %u\n", GetLastError());
		return FALSE;
	}

	new_state.PrivilegeCount = 1;
	new_state.Privileges[0].Luid = luid;

	if (bEnablePrivilege)
		new_state.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		new_state.Privileges[0].Attributes = 0;

	// Включаем привилегию или отключаем все привилегии.
	if (!MyAdjustTokenPrivileges(
		hToken, 					// дескриптор токена доступа
		FALSE, 						// изменяет привилегии на основе информации из new_state
		&new_state, 				// указатель на структуру, определяющую массив привилегий и их атрибутов
		sizeof(TOKEN_PRIVILEGES),
		(PTOKEN_PRIVILEGES)NULL, 	// указатель на буфер, который функция заполняет
		(PDWORD)NULL))
	{
		printf("\nОшибка AdjustTokenPrivileges: %u\n", GetLastError());
		return FALSE;
	}

	if (GetLastError() == ERROR_NOT_ALL_ASSIGNED)
	{
		printf("Токен не имеет указанной привилегии. \n");
		return FALSE;
	}

	return TRUE;
}

/* Добавляем привилегии пользователю. */
int UserPriv(LSA_HANDLE handle, bool flag)
{
	USER_INFO_4 *ui;			// информация о пользователе
	NET_API_STATUS ret_status;	// код возврата из функции

	WCHAR *user_name = (WCHAR *)malloc(256 * sizeof(WCHAR));
	WCHAR *priv_name = (WCHAR *)malloc(256 * sizeof(WCHAR));

	std::cout << "Введите имя пользователя: ";
	scanf_s(TEXT("%ws"), user_name);

	ret_status = MyNetUserGetInfo(
		NULL,					// используется локальный компьютер
		(LPCWSTR)user_name,		// имя пользователя
		4,						// уровень информации 4
		(LPBYTE*)&ui);			// адрес информации о пользователе

	// Проверяем на успешное завершение
	if (ret_status != NERR_Success)
	{
		printf("\nНе удалось получить информацию о пользователе.\n");
		return ret_status;
	}

	std::cout << "Выберите привилегию: ";
	scanf_s(TEXT("%ws"), priv_name);

	if (flag == 0) { AddPrivileges(ui->usri4_user_sid, handle, priv_name); }
	else { DelPrivileges(ui->usri4_user_sid, handle, priv_name); }

	if (user_name != NULL) free(user_name);
	if (priv_name != NULL) free(priv_name);
	MyNetApiBufferFree(ui); // освобождаем буфер

	return 0;
}

/* Добавляем привилегии группе. */
int GroupPriv(LSA_HANDLE handle, bool flag)
{
	DWORD dwErrCode;
	PSID lpSID;						// указатель на SID
	DWORD dwLengthOfSID = 0;		// длина SID
	DWORD dwLengthOfDomainName = 0; // длина имени домена
	PWCHAR lpDomainName;			// имя домена
	SID_NAME_USE type_of_SID;		// тип учетной записи

	WCHAR *group_name = (WCHAR *)malloc(256 * sizeof(WCHAR));
	WCHAR *priv_name = (WCHAR *)malloc(256 * sizeof(WCHAR));

	std::cout << "Введите имя группы: ";
	scanf_s(TEXT("%ws"), group_name);

	// Определяем длину SID группы
	MyLookupAccountNameW(
		NULL,					// ищем имя на локальном компьютере 
		group_name,				// имя группы
		NULL,					// определяем длину SID
		&dwLengthOfSID,			// длина SID 
		NULL, 					// определяем имя домена
		&dwLengthOfDomainName,	// длина имени домена
		&type_of_SID);			// тип учетной записи

	// Проверяем, вернула ли функция длину SID
	if (dwLengthOfSID == 0)
	{
		dwErrCode = GetLastError();
		printf("\nНе удалось найти имя учетной записи. \n");
		printf("Код ошибки: %d\n", dwErrCode);
		return dwErrCode;
	}

	// распределяем память для SID и имени домена
	lpSID = (PSID)malloc(dwLengthOfSID);
	lpDomainName = (PWCHAR)malloc(dwLengthOfDomainName + 10);

	if (!MyLookupAccountNameW(
		NULL,					// ищем имя на локальном компьютере
		group_name,				// имя группы
		lpSID,					// определяем длину SID
		&dwLengthOfSID,			// длина SID
		lpDomainName,			// определяем имя домена
		&dwLengthOfDomainName,	// длина имени домена
		&type_of_SID))			// тип учетной записи
	{
		dwErrCode = GetLastError();
		printf("\nНе удалось найти имя учетной записи.\n");
		printf("Код ошибки: %d\n", dwErrCode);
		return dwErrCode;
	}

	std::cout << "Выберите привилегию: ";
	scanf_s(TEXT("%ws"), priv_name);

	if (flag == 0) { AddPrivileges(lpSID, handle, priv_name); }
	else { DelPrivileges(lpSID, handle, priv_name); }

	if (group_name != NULL) free(group_name);
	if (priv_name != NULL) free(priv_name);
	if (lpSID != NULL) free(lpSID);

	return 0;
}

/* Удаляем привилегию. */
void DelPrivileges(PSID account_SID, LSA_HANDLE handle, LPCWSTR adding_priv)
{
	NTSTATUS ret_status;
	LSA_UNICODE_STRING user_rights;

	// Создаем массив структур LSA_UNICODE_STRING для имен привилегий.
	if (!InitLsaString(&user_rights, adding_priv))
	{
		wprintf(L"Ошибка InitLsaString\n");
		return;
	}

	bool all_rights = false;
	std::cout << "Может, удалить все привилегии? Введите 0(нет) или 1(да): ";
	std::cin >> all_rights;

	ret_status = MyLsaRemoveAccountRights(
		handle,			// дескриптор открытого объекта Policy
		account_SID,	// указатель на SID учетной записи
		all_rights,		// если TRUE , функция удаляет все привилегии и удаляет учетную запись
		&user_rights,	// указатель на массив структур LSA_UNICODE_STRING
		1				// количество элементов в массиве UserRights
	);

	if (ret_status == STATUS_SUCCESS)
		wprintf(L"Привилегия удалена.\n");
	else
		wprintf(L"\nПривилегия не удалена - %lu \n", MyLsaNtStatusToWinError(ret_status));
}

/* Добавляем привилегию. */
void AddPrivileges(PSID account_SID, LSA_HANDLE handle, LPCWSTR adding_priv)
{
	NTSTATUS ret_status;
	LSA_UNICODE_STRING user_rights;

	// Создаем массив структур LSA_UNICODE_STRING для имен привилегий.
	if (!InitLsaString(&user_rights, adding_priv))
	{
		wprintf(L"Ошибка InitLsaString\n");
		return;
	}

	ret_status = MyLsaAddAccountRights(
		handle,			// дескриптор открытого объекта Policy
		account_SID,	// указатель на SID учетной записи
		&user_rights,	// указатель на массив структур LSA_UNICODE_STRING
		1				// количество элементов в массиве UserRights
	);

	if (ret_status == STATUS_SUCCESS)
		wprintf(L"Привилегия добавлена.\n");
	else
		wprintf(L"\nПривилегия не добавлена - %lu \n", MyLsaNtStatusToWinError(ret_status));
}

/* Отображаем привилегии. */
void ShowPrivileges(LSA_HANDLE handle, PSID userSID)
{
	PLSA_UNICODE_STRING pp_user_rights;
	ULONG count_of_rights = 0;

	MyLsaEnumerateAccountRights(
		handle,		// дескриптор объекта Policy
		userSID,			// указатель на SID учетной записи 
		&pp_user_rights,	// указатель на массив структур LSA_UNICODE_STRING 
		&count_of_rights	// указатель на переменную для количества привилегий
	);

	wprintf(L"Привилегии:\n");
	for (size_t i = 0; i < count_of_rights; i++)
	{
		wprintf(L" %ws\n", pp_user_rights[i].Buffer);
	}

	wprintf(L"\n");
}

/* Открываем дескриптор объекта Policy. */
LSA_HANDLE GetPolicyHandle()
{
	LSA_OBJECT_ATTRIBUTES ObjectAttributes;
	NTSTATUS ret_status;
	LSA_HANDLE lsahPolicyHandle;

	// Атрибуты объекта зарезервированы, поэтому инициализируем их нулями
	ZeroMemory(&ObjectAttributes, sizeof(ObjectAttributes));

	// Получаем ссылку на объект "Политика"
	ret_status = MyLsaOpenPolicy(
		NULL,				// используется локальный компьютер
		&ObjectAttributes,	// атрибуты объекта
		POLICY_ALL_ACCESS,	// необходимые разрешения на доступ
		&lsahPolicyHandle	// получает ссылку на объект политики
	);

	// Проверяем на успешное завершение
	if (ret_status != STATUS_SUCCESS)
	{
		// Произошла ошибка. Отобразить ее как код ошибки win32.
		wprintf(L"\nOpenPolicy не возвращен %lu\n", MyLsaNtStatusToWinError(ret_status));
		return NULL;
	}

	return lsahPolicyHandle;
}