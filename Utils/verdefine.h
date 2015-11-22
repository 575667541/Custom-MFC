#ifndef _VERSION_DEFINE_FILE_H
#define _VERSION_DEFINE_FILE_H

#define VER_MAJOR				1			// ���汾
#define VER_MINOR				0			// �Ӱ汾
#define VER_REVISION			0			// �޶��汾
#define VER_BUILD				4			// ����汾
#define VER_TYPE				_T("Beta")		// �汾����

#define _STRINGIZING(v)			#v				
#define _VERJOIN(a,b,c,d)		_STRINGIZING(a.b.c.d)
#define STR_VERSION				TEXT(_VERJOIN(VER_MAJOR,VER_MINOR,VER_REVISION,VER_BUILD))

#define STR_YEAR				2014			
#define STR_MONTH				02		
#define STR_DAY					13
#define _STR_DATA(a,b,c)		_STRINGIZING(a-b-c)
//#define STR_BUILD_DATE			TEXT(_STR_DATA(STR_YEAR,STR_MONTH,STR_DAY))

#define STR_BUILD_DATE			TEXT(__DATE__)
#define STR_BUILD_TIME			TEXT(__TIME__)
#define STR_BUILD_DATETIME		STR_BUILD_DATE TEXT(" ") TEXT(__TIME__)		// ����ʱ��

#endif
