#ifndef SAGLOBALS_H
#define SAGLOBALS_H
class SAAbstractDatas;
#include <QScopedPointer>
//需要预定义SA_DEBUG_PRINT 才会打印
#ifdef QT_NO_DEBUG
#include <stdlib.h>
#include <stdio.h>
#endif
///
/// \def 前置声明的定义
///
#ifndef SA_IMPL_FORWARD_DECL
#define SA_IMPL_FORWARD_DECL(ClassName)	\
    class ClassName ## Private;
#endif
///
/// \def impl的简易实现
///
#ifndef SA_IMPL
#define SA_IMPL(Class)								 \
private:									 \
    inline Class ## Private *d_func() { return (d_ptr.data()); }		 \
    inline const Class ## Private *d_func() const { return (d_ptr.data()); } \
    friend class Class ## Private;						 \
    QScopedPointer<Class ## Private> d_ptr;
#endif
///
/// \def impl的定义
///
#ifndef SA_IMPL_PUBLIC
#define SA_IMPL_PUBLIC(Class)								   \
    friend class Class;								   \
public:											   \
    inline Class *q_func() { return (static_cast<Class *>(q_ptr)); }		   \
    inline const Class *q_func() const { return (static_cast<const Class *>(q_ptr)); } \
private:										   \
    Class *q_ptr;
#endif
///
/// \def impl获取指针，参考Q_D
///
#ifndef SA_D
#define SA_D(Class) \
    Class ## Private *d = d_func()
#endif
///
/// \def impl获取指针，参考Q_D
///
#ifndef SA_DC
#define SA_DC(Class) \
    const Class ## Private *d = d_func()
#endif
///
/// \def impl获取指针，参考Q_Q
///
#ifndef SA_Q
#define SA_Q(Class) \
    Class *q = q_func()
#endif
///
/// \def impl获取指针，参考Q_Q
///
#ifndef SA_QC
#define SA_QC(Class) \
    const Class *q = q_func()
#endif


#ifndef QT_NO_DEBUG_OUTPUT
/// \def 打印字节十六进制
#ifndef printByteArray
#define printByteArray(dataPtr, dataLen)					 \
    do{									 \
        const char *__c__char__ptr__ = (const char *)(dataPtr);		 \
        fprintf(stderr, "[");						 \
        for (int M__i = 0; M__i < (dataLen); ++M__i)			 \
        {								 \
            if (M__i % 10 == 0)					 \
            {							 \
                fprintf(stderr, "\n");				 \
                fprintf(stderr, ">> [%d]-[%d]: ", M__i, M__i+9); \
            }							 \
            fprintf(stderr, "%02X,", (int)__c__char__ptr__[M__i]);	 \
        }								 \
        fprintf(stderr, "\n]");						 \
    }while(0)
#endif
/// \def 打印QByteArray的十六进制
#ifndef printQByteArray
#define printQByteArray(qBytearr)						 \
    do{									 \
        QByteArray __tmp_byte_array__ = qBytearr;			 \
        fprintf(stderr, "[");						 \
        for (int M__i = 0; M__i < __tmp_byte_array__.size(); ++M__i)	 \
        {								 \
            if (M__i % 10 == 0)					 \
            {							 \
                fprintf(stderr, "\n");				 \
                fprintf(stderr, ">> [%d]-[%d]: ", M__i, M__i+9); \
            }							 \
            fprintf(stderr, "%02X,", (int)__tmp_byte_array__[M__i]); \
        }								 \
        fprintf(stderr, "\n]");						 \
    }while(0)
#endif
#else
#ifndef saPrint
#define saPrint()
#endif
#ifndef printByteArray
#define printByteArray(dataPtr, dataLen)	Q_UNUSED(dataPtr); Q_UNUSED(dataLen);
#endif
#ifndef printQByteArray
#define printQByteArray(qBytearr)		Q_UNUSED(qBytearr);
#endif
#endif

#endif // GLOBALS_H
