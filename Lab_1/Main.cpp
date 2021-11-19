#include "Main.h"
#include "Info.h"

using namespace std;

LSA_HANDLE policy_handle;

int operations()
{
	cout << endl << "Доступные команды:" << endl;
	
	cout << " 1 - Вывести список пользователей их привилегии и SID" << endl;
	cout << " 2 - Создать нового пользователя" << endl;
	cout << " 3 - Удалить пользователя" << endl;
	cout << " 4 - Изменить имя пользователя" << endl;
	cout << " 5 - Изменить пароль пользователя" << endl;
	
	cout << " 6 - Вывести список групп их привилегии и SID" << endl;
	cout << " 7 - Создать новую группу" << endl;
	cout << " 8 - Удалить группу" << endl;
	cout << " 9 - Изменить имя группы" << endl;
	cout << " 10 - Добавить члена группы по его идентификатору безопасности" << endl;
	cout << " 11 - Добавить члена группы по его  имени" << endl;
	cout << " 12 - Удалиить члена группы по его идентификатору безопасности" << endl;
	cout << " 13 - Удалиить члена группы по его  имени" << endl;
	cout << " 14 - Установить членов группы" << endl;

	cout << " 15 - Добавить/удалить привилегии пользователя" << endl;
	cout << " 16 - Добавить/удалить привилегии группы" << endl;
	cout << " 17 - Включить/выключить привилегию" << endl;

	cout << " 0 - Выход" << endl;

	int command;
	cout << endl << "Выберите команду:";
	std::cin >> command;

	bool flag = 0;

	printf("\n");
	switch (command)
	{
	case 0:
		return 0;
	case 1:
		GetUsersList(policy_handle);
		break;
	case 2:
		NewUserCreate(policy_handle);
		break;
	case 3:
		UserDelete();
		break;
	case 4:
		UserChangeName();
		break;
	case 5:
		UserChangePassword();
		break;
	case 6:
		GetGroupsList(policy_handle);
		break;
	case 7:
		NewGroupCreate();
		break;
	case 8:
		GroupDelete();
		break;
	case 9:
		GroupChangeName();
		break;
	case 10:
		GroupAddMemberID();
		break;
	case 11:
		GroupAddMemberN();
		break;
	case 12:
		GroupDelMemberID();
		break;
	case 13:
		GroupDelMemberN();
		break;
	case 14:
		GroupSetMembers();
		break;
	case 15:
		cout << "Добавить привилегию - 0, удалить привилегию - 1:";
		std::cin >> flag;
		UserPriv(policy_handle, flag);
		break;
	case 16:
		cout << "Добавить привилегию - 0, удалить привилегию - 1:";
		std::cin >> flag;
		GroupPriv(policy_handle, flag);
		break;
	case 17:
		EnDisPrivileges();
		break;
	default:
		break;
	}

	return 1;
}


int main()
{
	setlocale(LC_ALL, "rus");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	UploadMyDll();

	policy_handle = GetPolicyHandle();
	if (!policy_handle)
	{
		wprintf(L"Ошибка: не удалось открыть дескриптор объекта Policy!\n");
		system("pause");
		return 0;
	}

	int code;
	while ((code = operations()) > 0)
		Sleep(1000);

	close(code);
}

void close(int code)
{
	if (code != 0)
	{
		printf("\nПрограмма будет закрыта кодом %d. Нажмите любую клавишу.\n", code);
		system("pause");
	}

	OffloadMyDll();
	exit(code);
}