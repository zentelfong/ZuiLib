#ifndef BASE_TLS_H__
#define BASE_TLS_H__

#include <assert.h>
#ifdef WIN32
	#define TLSVAR			DWORD
	#define TLSALLOC(k)		(*(k)=TlsAlloc(), TLS_OUT_OF_INDEXES==*(k))
	#define TLSFREE(k)		(!TlsFree(k))
	#define TLSGET(k)		TlsGetValue(k)
	#define TLSSET(k, a)	(!TlsSetValue(k, a))
	#ifdef _DEBUG
		static LPVOID ChkedTlsGetValue(DWORD idx)
		{
			LPVOID ret=TlsGetValue(idx);
			assert(S_OK==GetLastError());
			return ret;
		}
		#undef TLSGET
		#define TLSGET(k) ChkedTlsGetValue(k)
	#endif
#else
	#define TLSVAR			pthread_key_t
	#define TLSALLOC(k)		pthread_key_create(k, 0)
	#define TLSFREE(k)		pthread_key_delete(k)
	#define TLSGET(k)		pthread_getspecific(k)
	#define TLSSET(k, a)	pthread_setspecific(k, a)
#endif

#endif //BASE_TLS_H__