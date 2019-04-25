#if !defined SAFE_STRING
#define SAFE_STRING

int strcpy_s(
   char *dest,
   rsize_t dest_size,
   const char *src
);

int wcscpy_s(
   wchar_t *dest,
   rsize_t dest_size,
   const wchar_t *src
);

int _mbscpy_s(
   unsigned char *dest,
   rsize_t dest_size,
   const unsigned char *src
);

int _mbscpy_s_l(
   unsigned char *dest,
   rsize_t dest_size,
   const unsigned char *src,
   _locale_t locale
);

#endif
