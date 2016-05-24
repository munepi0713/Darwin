
#include <cstdint>
#include <cstdio>

#include <string>
#include <vector>

#include <Windows.h>

#define throw_error(err)  { throw (uint32_t)(err); } while ( 0 )

// エラーの表示
static void print_error(const std::string& msg)
{
	DWORD errcode = GetLastError();
    printf("%s errorcode: %lx\n", msg.c_str(), errcode);
}

void  move_file(const std::wstring& src, const std::wstring& dst, int retry = 100)
{
    DWORD last_error = 0;
    int r = 0;
    while ( r < retry ) {
        printf("try to move file...\n");
        if ( MoveFileExW(src.c_str(), dst.c_str(), MOVEFILE_COPY_ALLOWED | MOVEFILE_WRITE_THROUGH | MOVEFILE_REPLACE_EXISTING) ) {
            break;
        }

        last_error = GetLastError();
        printf("err code = %x\n", last_error);
        switch ( last_error ) {
        case ERROR_ACCESS_DENIED :
            if ( r >= retry - 1 ) throw_error(last_error);
            break;
        case ERROR_FILE_NOT_FOUND :
            throw_error(last_error);
        }
        Sleep(100);
        r++;
    }
}


class WatchDir
{
public :
    virtual ~WatchDir();

    void                       open(const std::wstring& root_dir);
    std::vector<std::wstring>  watch();
    void                       close();
    std::vector<std::wstring>  get_list(const std::wstring& root_dir);
private :
    std::wstring          _dir;
    HANDLE                _dir_handle;
    std::vector<uint8_t>  _buf;
};

WatchDir::~WatchDir()
{
    close();
};

#if 1
void  WatchDir::open(const std::wstring& root_dir)
{
    _dir = root_dir;
    _dir_handle = 0;
}

void  WatchDir::close()
{
	// Release handles.
	FindCloseChangeNotification(_dir_handle);
}

// メインエントリ
std::vector<std::wstring> WatchDir::watch()
{
    std::vector<std::wstring> ret;

    if ( _dir_handle == 0 ) {
	    // Monitor conditions.
	    DWORD filter = 
		    FILE_NOTIFY_CHANGE_FILE_NAME   |
		    FILE_NOTIFY_CHANGE_DIR_NAME    |
		    FILE_NOTIFY_CHANGE_ATTRIBUTES  |
		    FILE_NOTIFY_CHANGE_SIZE        |
		    FILE_NOTIFY_CHANGE_LAST_WRITE;

        // Open the root directory to be monitored.
	    _dir_handle = FindFirstChangeNotification(
            _dir.c_str(),  // 監視先ディレクトリ
		    TRUE,             // サブディレクトリも監視
		    filter);          // 監視条件
	    if ( _dir_handle == INVALID_HANDLE_VALUE ) {
		    print_error("FindFirstChangeNotificationでの失敗");
		    throw_error(1);
	    }
    }
    else {
        if ( !FindNextChangeNotification(_dir_handle) ) {
			print_error("FindNextChangeNotificationでの失敗");
		}
    }

    //
    DWORD waitResult = WaitForSingleObject(_dir_handle, INFINITE);
    if ( waitResult != WAIT_OBJECT_0 ) {
		print_error("WaitForSingleObjectでの失敗");
        throw_error(GetLastError());
    }

    return ret;
}

#else
void  WatchDir::open(const std::wstring& root_dir)
{
    size_t bufsize = 1024 * 8;
	bool hasError = false;

    // Open the root directory to be monitored.
	_dir_handle = CreateFile(
		root_dir.c_str(), // Path to the root dir to be monitored.
		FILE_LIST_DIRECTORY,
		FILE_SHARE_READ,// | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_BACKUP_SEMANTICS,
		NULL
		);
	if ( _dir_handle == INVALID_HANDLE_VALUE ) {
		print_error("CreateFileでの失敗");
		throw_error(1);
	}

    // Buffer for record files changed between ReadDirectoryChangesW notifications.
    _buf.assign(bufsize, 0);
}

void  WatchDir::close()
{
	// Release handles.
	CloseHandle(_dir_handle);
}

// メインエントリ
std::vector<std::wstring> WatchDir::watch()
{

	// Monitor conditions.
	DWORD filter = 
		FILE_NOTIFY_CHANGE_FILE_NAME   |
		FILE_NOTIFY_CHANGE_DIR_NAME    |
		FILE_NOTIFY_CHANGE_ATTRIBUTES  |
		FILE_NOTIFY_CHANGE_SIZE        |
		FILE_NOTIFY_CHANGE_LAST_WRITE;

    // Start monitoring directory changes.
    DWORD written;
	BOOL result = ReadDirectoryChangesW(
			_dir_handle,       // Root directory to be monitored.
            _buf.data(), // Buffer for notifications
            _buf.size(), // Buffer size.
			TRUE,       // Is subdirectory monitored?
			filter,     // Filter for what changes to be monitored.
			&written,   // Bytes written
			NULL,       // Unused (only for overlapped IO)
			NULL);       // Unused (only for overlapped IO)
	if ( !result ) {
		print_error("ReadDirectoryChangesWでの失敗");
        throw_error(GetLastError());
	}

	// 変更通知をコンソールにダンプする.
	printf("returned size=%lu\n", written);

    std::vector<std::wstring> ret;
	if ( written == 0 ) {
		// 返却サイズ、0ならばバッファオーバーを示す
		print_error("buffer overflow!!\n");

	} else {
		// 最初のエントリに位置付ける
		const FILE_NOTIFY_INFORMATION *info = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(_buf.data());
			
		// エントリの末尾まで繰り返す
		for (;;) {
			// アクションタイプを可読文字に変換
			std::wstring action = L"UNKNOWN";
			switch ( info->Action ) {
			case FILE_ACTION_ADDED:
				action = L"Added";
				break;
			case FILE_ACTION_REMOVED:
				action = L"Removed";
				break;
			case FILE_ACTION_MODIFIED:
				action = L"Modified";
				break;
			case FILE_ACTION_RENAMED_OLD_NAME:
				action = L"Rename Old";
				break;
			case FILE_ACTION_RENAMED_NEW_NAME:
				action = L"Rename New";
				break;
			}

			// ファイル名はヌル終端されていないので
			// 長さから終端をつけておく.
			DWORD len = info->FileNameLength / sizeof(WCHAR); // In bytes. (Not number of charactors)
            std::wstring filename(len + 1, 0); // length + NULL termination
            std::copy(info->FileName, info->FileName + len, filename.begin());
            //filename[len] = 0;

            // Save filename.
            ret.push_back(filename);

            // アクションと対象ファイルを表示.
			// (ファイル名は指定ディレクトリからの相対パスで通知される.)
            wprintf(L"[%s]<%s>\r\n", action.c_str(), filename.c_str());

			if ( info->NextEntryOffset == 0 ) {
				// 次のエントリは無し
				break;
			}
            else {
				// 次のエントリの位置まで移動する. (現在アドレスからの相対バイト数)
				info = reinterpret_cast<const FILE_NOTIFY_INFORMATION*>(
					reinterpret_cast<const unsigned char*>(info) + info->NextEntryOffset);
            }
		}
	}

    return ret;
}
#endif

std::vector<std::wstring>  WatchDir::get_list(const std::wstring& filter)
{
    std::vector<std::wstring> ret;

    // 全てのファイルを列挙する
	HANDLE hSearch;
	WIN32_FIND_DATA fd;

    hSearch = FindFirstFile(filter.c_str(), &fd);
	if( hSearch == INVALID_HANDLE_VALUE ) {
        DWORD last_error = GetLastError();
        if ( last_error == ERROR_FILE_NOT_FOUND ) {
            return ret;
        }
        else {
            throw_error(GetLastError());
        }
    }

    for (;;) {
        ret.push_back(fd.cFileName);
        if ( !FindNextFile(hSearch, &fd) ) {
            DWORD last_error = GetLastError();
            if ( last_error == ERROR_NO_MORE_FILES ) {
                break;
            }
            else {
                throw_error(last_error);
            }
        }
    }

	FindClose( hSearch );

    return ret;
}

std::wstring to_wstring(const std::string& str)
{
    std::wstring wstr(str.size(), 0);

    MultiByteToWideChar(
            CP_OEMCP,
            MB_PRECOMPOSED,
            str.data(),
            str.size(),
            &wstr[0],
            wstr.size() * sizeof(wchar_t));

    return wstr;
}


int  main(int argc, const char* argv[])
{
    std::wstring src_dir = L"E:\\Garnet\\JobResult";
    std::wstring dst_dir = L"W:\\garnet\\jobresult";
    std::wstring filter = L"*.garnet.txt";

    int mode = 0;
    for (int i = 1; i < argc; i++) {
        if ( argv[i][0] == '-' ) {
            switch ( argv[i][1] ) {
            case 'i' :
            case 'o' :
            case 'f' :
                mode = argv[i][1];
                break;
            }
        }
        else {
            switch ( mode ) {
            case 'i' :
                src_dir = to_wstring(argv[i]);
                break;
            case 'o' :
                dst_dir = to_wstring(argv[i]);
                break;
            case 'f' :
                filter = to_wstring(argv[i]);
                break;
            }
            mode = 0;
        }
    }

	// コンソール出力を日本語可能に
	setlocale(LC_ALL, "");

    try {
        WatchDir watch;
        watch.open(src_dir);

        for (;;) {
            std::vector<std::wstring> files;

            // List up all files.
            files = watch.get_list(src_dir + L"\\" + filter);

            // Copy all files.
            for (std::vector<std::wstring>::const_iterator it = files.begin(); it != files.end(); it++) {
                move_file(src_dir + L'\\' + *it, dst_dir + L'\\' + *it);
            }

            // Wait for any drive modification. This is blocking operation.
            files = watch.watch();

            if ( files.size() == 0 ) {
                // Overflow detected. 
                // TODO: List up all files by alternative way.
                printf("Overflow detected.\n");
            }
        }
    }
    catch ( uint32_t errcode ) {
        printf("Error. Code=%d\n", errcode);
        abort();
    }

    return 0;
}
