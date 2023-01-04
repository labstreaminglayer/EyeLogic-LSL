
#ifndef EL_EXPORT_H
#define EL_EXPORT_H

#ifdef SHARED_EXPORTS_BUILT_AS_STATIC
#  define EL_EXPORT
#  define EL_NO_EXPORT
#else
#  ifndef EL_EXPORT
#    ifdef ELApi_EXPORTS
        /* We are building this library */
#      define EL_EXPORT __declspec(dllexport)
#    else
        /* We are using this library */
#      define EL_EXPORT __declspec(dllimport)
#    endif
#  endif

#  ifndef EL_NO_EXPORT
#    define EL_NO_EXPORT 
#  endif
#endif

#ifndef EL_DEPRECATED
#  define EL_DEPRECATED 
#endif

#ifndef EL_DEPRECATED_EXPORT
#  define EL_DEPRECATED_EXPORT EL_EXPORT EL_DEPRECATED
#endif

#ifndef EL_DEPRECATED_NO_EXPORT
#  define EL_DEPRECATED_NO_EXPORT EL_NO_EXPORT EL_DEPRECATED
#endif

#if 0 /* DEFINE_NO_DEPRECATED */
#  ifndef EL_NO_DEPRECATED
#    define EL_NO_DEPRECATED
#  endif
#endif

#endif /* EL_EXPORT_H */
